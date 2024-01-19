#include <filesystem>
#include <list>
#include <bsoncxx/json.hpp>
#include "repository.h"
#define MONGO_DB_NAME "backhome"
#define MONGO_COLLECTION "tidal_albums"

std::unordered_set<std::string> ArtistRepository::getAllArtists() {
    auto client = pool.acquire();
    auto collection = (*client)[MONGO_DB_NAME][MONGO_COLLECTION];
    std::unordered_set<std::string> uniqueArtists;
    auto cursor = collection.find({});
    for (auto&& doc : cursor)
    {
        if (doc["artist"])
        {
            std::string artist = doc["artist"].get_string().value.to_string();
            if (uniqueArtists.find(artist) == uniqueArtists.end())
            {
                uniqueArtists.insert(artist);
            }
        }
    }
    return uniqueArtists;
}

std::unordered_set<std::string> ArtistRepository::getAlbumsByArtistName(const std::string &name) {
    auto client = pool.acquire();
    auto collection = (*client)[MONGO_DB_NAME][MONGO_COLLECTION];

    bsoncxx::builder::stream::document filter_builder;
    filter_builder << "artist" << name << "source" << "TIDAL";
    auto filter = filter_builder.view();
    auto cursor = collection.find(filter);

    std::unordered_set<std::string> uniqueAlbums;
    for (auto&& doc : cursor)
    {
        if (doc["title"])
        {
            std::string title = doc["title"].get_string().value.to_string();
            uniqueAlbums.insert(title);
        }
    }
    return uniqueAlbums;
}

std::string ArtistRepository::getAlbumImage(const std::string &artist, const std::string &album)
{
    auto client = pool.acquire();
    auto collection = (*client)[MONGO_DB_NAME][MONGO_COLLECTION];

    bsoncxx::builder::stream::document filter_builder;
    filter_builder << "artist" << artist << "title" << album;
    auto filter = filter_builder.view();

    auto cursor = collection.find(filter);
    if (cursor.begin() == cursor.end())
    {
        return "";
    }

    auto doc = *cursor.begin();
    return doc["cover_url"].get_string().value.to_string();
}

std::list<std::string> ArtistRepository::getAlbumTracks(const std::string &artist, const std::string &album)
{
    auto client = pool.acquire();
    auto collection = (*client)[MONGO_DB_NAME][MONGO_COLLECTION];

    bsoncxx::builder::stream::document filter_builder;
    filter_builder << "artist" << artist << "title" << album;
    auto filter = filter_builder.view();




    std::list<std::string> tracks;
    auto cursor = collection.find(filter);
    auto doc = *cursor.begin();  // Obtiene el primer elemento del cursor
    if (doc["tracks"] && doc["tracks"].type() == bsoncxx::type::k_array)
    {
        auto tracks_array = doc["tracks"].get_array().value;
        for (const auto& track_value : tracks_array)
        {
            if (track_value["media_url"])
            {
                std::string path_str = track_value["media_url"].get_string().value.to_string();
                std::filesystem::path path(path_str);
                std::string file_name = path.filename().string();
                tracks.push_back(file_name);
            }
        }
    }
    return tracks;
}

std::string ArtistRepository::getAlbumJson(const std::string &artist, const std::string &album) {
    auto client = pool.acquire();
    auto collection = (*client)[MONGO_DB_NAME][MONGO_COLLECTION];

    bsoncxx::builder::stream::document filter_builder;
    filter_builder << "artist" << artist << "title" << album;
    auto filter = filter_builder.view();

    auto cursor = collection.find(filter);
    if (cursor.begin() == cursor.end())
    {
        return "";
    }
    auto doc = *cursor.begin();
    return bsoncxx::to_json(doc);
}

std::string ArtistRepository::getAlbumSourceId(const std::string &artist, const std::string &album) {
    auto client = pool.acquire();
    auto collection = (*client)[MONGO_DB_NAME][MONGO_COLLECTION];

    bsoncxx::builder::stream::document filter_builder;
    filter_builder << "artist" << artist << "title" << album << "source" << "TIDAL";
    auto filter = filter_builder.view();

    auto cursor = collection.find(filter);
    if (cursor.begin() == cursor.end())
    {
        return "";
    }
    auto doc = *cursor.begin();
    return std::to_string(doc["source_id"].get_int32());
}
