#ifndef MUSIC_COMPONENT_H
#define MUSIC_COMPONENT_H

#include "../wavefs/wavefs.h"
#include "../rest/tidal_client.h"
#include "../repository/repository.h"
#include <mongocxx/client.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/document/view_or_value.hpp>
#include <regex>

#define MUSIC_FOLDER "/music/?"
#define MUSIC_BY_ARTIST_FOLDER "/music/by-artist/?"
#define ARTIST_REGEX_FOLDER "/music/by-artist/([^/]+)/?"
#define ARTIST_ALBUM_REGEX_FOLDER "/music/by-artist/([^/]+)/([^/]+)/?"

class MusicDirectory : public VirtualDirectory {
public:
    explicit MusicDirectory(wavefs::config &config) :
        repository(config),
        appClient(config),
        encodedMasterKey(config["tidal"]["master.key"])
        { }
    std::string getBasePath() override;
    int getattr(const char *path, struct stat *st) override;
    int readdir(const char *path, void *buffer, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi) override;
    int read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi) override;
    int open(const char *path, struct fuse_file_info *fi) override;
private:
    ArtistRepository repository;
    TidalClient appClient;
    std::string encodedMasterKey;
    int listArtists(void *buffer, fuse_fill_dir_t filler);
    int listArtistAlbums(const std::string& artist, void *buffer, fuse_fill_dir_t filler);
    int listAlbumFiles(const std::string& artist, const std::string& album, void *buffer, fuse_fill_dir_t filler);
    int read_album_json(const std::string& artist, const std::string& album, char *buf, size_t size, off_t offset, struct fuse_file_info *fi);

    int open_album_img(const std::string &artist, const std::string &album, fuse_file_info *fi);
    int open_album_flac(const std::string& artist, const std::string& album, const std::string& path, struct fuse_file_info *fi);

    static bool isRootMusicFolder(const string &path);
    static bool isMusicByArtistFolder(const string &path);
    static bool isArtistAlbumFolder(const string &path, smatch &match);
    static bool isArtistFolder(const string &path, smatch &match);
    static bool isAlbumFile(const string &path_str, smatch &match);
    static bool isAlbumImageFile(const string &path_str, smatch &match);
    static bool isAlbumTrackFile(const string &path_str, smatch &match);
    static bool endsWithAny(const char* path, const std::initializer_list<const char*>& extensions);
};

#endif // MUSIC_COMPONENT_H
