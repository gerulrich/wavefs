#ifndef WAVEFS_OAUTH_REST_CLIENT_H
#define WAVEFS_OAUTH_REST_CLIENT_H
#include "../config/config.h"
#include <chrono>
#include <ctime>
#include <nlohmann/json.hpp>
#include <utility>

using json = nlohmann::json;
using namespace std;

class OAuthRestClient {
public:
    explicit OAuthRestClient(string  refreshToken, string accessToken, string clientId) :
        refreshToken(std::move(refreshToken)),
        accessToken(std::move(accessToken)),
        clientId(std::move(clientId))
    {
        // Inicializar el accessToken y accessTokenExpiration al crear el objeto
        // obtainAccessToken();
        // TODO eliminar
        accessTokenExpiration = std::chrono::system_clock::now() + std::chrono::seconds(86400);
    }

    json get(const std::string &url);
private:
    string refreshToken;
    string accessToken;
    string clientId;
    chrono::time_point<std::chrono::system_clock> accessTokenExpiration;

    void refreshTokenIfNeeded();
    void obtainAccessToken();

};

#endif //WAVEFS_OAUTH_REST_CLIENT_H
