#ifndef WAVEFS_REPOSITORY_H
#define WAVEFS_REPOSITORY_H

#include <mongocxx/client.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/document/view_or_value.hpp>
#include <mongocxx/pool.hpp>
#include <unordered_set>
#include <mongocxx/instance.hpp>
#include "../config/config.h"

class ArtistRepository {
public:
    explicit ArtistRepository(wavefs::config &config) :
    //inst{},
    pool{mongocxx::uri{config["core"]["mongo.url"]}}
    { };
    std::unordered_set<std::string> getAllArtists();
    std::unordered_set<std::string> getAlbumsByArtistName(const std::string &artist);
    std::list<std::string> getAlbumTracks(const std::string &artist, const std::string &album);
    std::string getAlbumImage(const std::string &artist, const std::string &album);
    std::string getAlbumJson(const std::string &artist, const std::string &album);
    std::string getAlbumSourceId(const std::string &artist, const std::string &album);
private:
    //mongocxx::instance inst;
    mongocxx::pool pool;
};


#endif //WAVEFS_REPOSITORY_H
