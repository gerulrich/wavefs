#include "music_directory.h"
#include "../file_downloader.h"
#include "../encode/base64.h"
#include "../cypher/cypher.h"
#include <unistd.h>
#include <cstring>
#include <unordered_set>
#include <regex>
#include "nlohmann/json.hpp"
#include <vector>
#include <openssl/evp.h>

using json = nlohmann::json;

std::string MusicDirectory::getBasePath()
{
    return "/music";
}

int MusicDirectory::getattr(const char *path, struct stat *st)
{
    st->st_uid = getuid();
    st->st_gid = getgid();
    st->st_atime = time( nullptr );
    st->st_mtime = time( nullptr );

    if ( !endsWithAny(path, {".flac", ".jpeg", ".jpg", ".png", ".json", ".txt"}))
    {
        st->st_mode = S_IFDIR | 0755;
        st->st_nlink = 2;
    }
    else
    {
        st->st_mode = S_IFREG | 0644;
        st->st_nlink = 1;
        st->st_size = 1000;
    }
    return 0;
}

int MusicDirectory::readdir(const char *path, void *buffer, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi) {
    std::smatch match;
    std::string path_str(path);
    if (isRootMusicFolder(path_str))
    {
        filler(buffer, "by-artist", nullptr, 0);
        return 0;
    }
    else if (isMusicByArtistFolder(path_str))
    {
        return listArtists(buffer, filler);
    }
    else if (isArtistFolder(path_str, match))
    {
        return listArtistAlbums(match[1].str(), buffer, filler);
    }
    else if (isArtistAlbumFolder(path_str, match))
    {
        return listAlbumFiles(match[1].str(), match[2].str(), buffer, filler);
    }
    return -ENOENT;
}

int MusicDirectory::listArtists(void *buffer, fuse_fill_dir_t filler) {
    std::unordered_set<std::string> artists = repository.getAllArtists();
    for (const auto & artist : artists) {
        filler( buffer, artist.c_str(), nullptr, 0 );
    }
    return 0;
}

int MusicDirectory::listArtistAlbums(const std::string& artist, void *buffer, fuse_fill_dir_t filler) {
    std::unordered_set<std::string> albums = repository.getAlbumsByArtistName(artist);
    for (const auto & album : albums) {
        filler( buffer, album.c_str(), nullptr, 0 );
    }
    return 0;
}

int MusicDirectory::listAlbumFiles(const std::string& artist, const std::string& album, void *buffer, fuse_fill_dir_t filler) {
    auto source_id = repository.getAlbumSourceId(artist, album);
    auto tracks = appClient.getTracks(source_id, "AR");

    // std::list<std::string> tracks = repository.getAlbumTracks(artist, album);
    for (const auto & track : tracks) {
        std::stringstream ss;
        ss << std::setw(2) << std::setfill('0') << track.trackNumber;
        std::string number = ss.str();
        std::string filename = number + " " + track.title + ".flac";


        filler( buffer, filename.c_str(), nullptr, 0 );
    }
    filler(buffer, "folder.jpg", nullptr, 0);
    filler(buffer, "info.json", nullptr, 0);
    return 0;
}

int MusicDirectory::read(const char *path, char *buffer, size_t size, off_t offset, struct fuse_file_info *fi) {
    std::smatch match;
    std::string path_str(path);
    if (isAlbumFile(path_str, match))
    {
        std::string artist = match[1].str();
        std::string album = match[2].str();
        if (path_str.ends_with(".json"))
        {
            return read_album_json(artist, album, buffer, size, offset, fi);
        }
        return -ENOENT;
    }
    return -ENOENT;
}

int MusicDirectory::read_album_json(const std::string& artist, const std::string& album, char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
    const std::string json = repository.getAlbumJson(artist, album);
    const char *content = json.c_str();

    size_t len = strlen(content);
    if (offset >= len)
    {
        return 0;
    }

    if (offset + size > len)
    {
        memcpy(buf, content + offset, len - offset);
        return len - offset;
    }

    memcpy(buf, content + offset, size);
    return size;
}

int MusicDirectory::open_album_img(const std::string& artist, const std::string& album,struct fuse_file_info *fi)
{
    std::string url = repository.getAlbumImage(artist, album);
    if (url.empty())
    {
        return -ENOENT;
    }

    FileDownloader downloader;
    auto file = downloader.download(url);
    file->file = ::open(file->filename.c_str(), O_RDONLY);
    fi->fh = reinterpret_cast<uint64_t>(file);
    return 0;
}

int MusicDirectory::open_album_flac(const std::string& artist, const std::string& album, const std::string& file, struct fuse_file_info *fi) {
    std::string source_id = repository.getAlbumSourceId(artist, album);
    std::smatch match;
    if (source_id.empty() || !std::regex_search(file, match, std::regex("^\\d+"))) {
        return -ENOENT;
    }

    int number = std::stoi(match[0]);
    auto tracks = appClient.getTracks(source_id, "AR");
    auto it = std::find_if(tracks.begin(), tracks.end(), [number](const wavefs::types::TrackInfo& track) {
        return track.trackNumber == number;
    });

    if (it != tracks.end())
    {
        // Se encontró un TrackInfo con el trackNumber deseado
        wavefs::types::TrackInfo desiredTrack = *it;
        auto data = appClient.getStreamUrl(desiredTrack.id, "AR", "LOSSLESS");
        json stream = json::parse(base64_decode(data.manifest, false));
        std::string fileUrl = stream["urls"][0];
        FileDownloader downloader;
        auto encrypted_file = downloader.download(fileUrl);

        cypher c(encodedMasterKey);
        std::pair<vector<unsigned char>, vector<unsigned char>> keys = c.decrypt_key(stream["keyId"]);
        c.decrypt_file(keys, *encrypted_file);

        //Reapuntamos al nuevo file
        encrypted_file->file = ::open(encrypted_file->filename.c_str(), O_RDONLY);
        // TODO reapuntar al nuevo file
        fi->fh = reinterpret_cast<uint64_t>(encrypted_file);
        return 0;
    }
    return -ENOENT;
}

int MusicDirectory::open(const char *path, struct fuse_file_info *fi)
{
    std::string path_str(path);
    std::smatch  match;
    if (isAlbumImageFile(path_str, match))
    {
        std::string artist = match[1];
        std::string album = match[2];
        return open_album_img(artist, album, fi);
    }
    if (isAlbumTrackFile(path_str, match))
    {
        std::string artist = match[1];
        std::string album = match[2];
        std::string file = match[3];
        return open_album_flac(artist, album, file, fi);
    }
    return 0;
}

bool MusicDirectory::isAlbumTrackFile(const string &path_str, smatch &match)
{
    return regex_match(path_str, match, regex("^/music/by-artist/([^/]+)/([^/]+)/([^/]+\\.(?:flac))$"));
}

bool MusicDirectory::isAlbumImageFile(const string &path_str, smatch &match)
{
    return regex_match(path_str, match, regex("^/music/by-artist/([^/]+)/([^/]+)/folder\\.jpg$"));
}

bool MusicDirectory::isRootMusicFolder(const string &path)
{
    return regex_match(path, regex(MUSIC_FOLDER));
}

bool MusicDirectory::isArtistAlbumFolder(const string &path, smatch &match)
{
    return regex_match(path, match, regex(ARTIST_ALBUM_REGEX_FOLDER));
}

bool MusicDirectory::isArtistFolder(const string &path, smatch &match)
{
    return regex_match(path, match, regex(ARTIST_REGEX_FOLDER));
}

bool MusicDirectory::isMusicByArtistFolder(const string &path_str)
{
    return regex_match(path_str, regex(MUSIC_BY_ARTIST_FOLDER));
}

bool MusicDirectory::isAlbumFile(const string &path_str, smatch &match)
{
    return regex_match(path_str, match, regex("^/music/by-artist/([^/]+)/([^/]+)/([^/]+\\.(?:flac|jpg|json|txt))$"));
}

bool MusicDirectory::endsWithAny(const char *_path, const std::initializer_list<const char *> &extensions) {
    std::string path(_path);

    for (const auto& extension : extensions)
    {
        std::string strExtension(extension);
        if (path.ends_with(strExtension))
        {
            return true;
        }
    }

    return false;
}


