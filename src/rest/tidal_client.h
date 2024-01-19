#ifndef APP_CLIENT_DOWNLOADER_H
#define APP_CLIENT_DOWNLOADER_H

#include "../config/config.h"
#include "oauth_restclient.h"
#include "types.h"
#include <string>
#include <chrono>
#include <ctime>
#include <curlpp/cURLpp.hpp>
#include <curlpp/Options.hpp>

class TidalClient : public OAuthRestClient {
public:
    explicit TidalClient(wavefs::config &config) :
            OAuthRestClient(
            config["tidal"]["refresh.token"],
            config["tidal"]["access.token"],
            config["tidal"]["client.id"])
    {
    }
    ~TidalClient() = default;
    std::vector<wavefs::types::TrackInfo> getTracks(const std::string& albumId, const std::string& country);
    wavefs::types::StreamInfo getStreamUrl(int trackId, const std::string& country, const std::string& quality);

};

#endif // APP_CLIENT_DOWNLOADER_H
