#include "oauth_restclient.h"
#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>

#ifdef NDEBUG
    #define VERBOSE_MODE true
#else
    #define VERBOSE_MODE false
#endif


void OAuthRestClient::refreshTokenIfNeeded()
{
    auto currentTime = std::chrono::system_clock::now();
    if (accessToken.empty() || currentTime >= accessTokenExpiration)
    {
        obtainAccessToken();
    }
}

void OAuthRestClient::obtainAccessToken()
{
    try
    {
        // TODO fix authUrl
        std::string url = "https://auth.tidal.com/v1/oauth2/token";
        std::string grantType = "refresh_token";
        std::string scope = "r_usr+w_usr";
        std::string postData = "client_id=" + clientId +
                               "&refresh_token=" + refreshToken +
                               "&grant_type=" + grantType +
                               "&scope=" + scope;

        curlpp::Cleanup cleanup;
        curlpp::Easy request;
        request.setOpt(new curlpp::options::Url(url));
        request.setOpt(new curlpp::options::Verbose(VERBOSE_MODE));

        request.setOpt(new curlpp::options::PostFields(postData));
        request.setOpt(new curlpp::options::PostFieldSize(postData.length()));

        std::ostringstream responseStream;
        curlpp::options::WriteStream ws(&responseStream);
        request.setOpt(ws);

        request.perform();

        json response = json::parse(responseStream.str());
        accessToken = response["access_token"];
        int expiresIn = response["expires_in"];
        accessTokenExpiration = std::chrono::system_clock::now() + std::chrono::seconds(expiresIn);
    }
    catch (curlpp::RuntimeError& e)
    {
        std::cerr << "Error de tiempo de ejecución en obtainAccessToken: " << e.what() << std::endl;
    }
    catch (curlpp::LogicError& e)
    {
        std::cerr << "Error lógico en obtainAccessToken: " << e.what() << std::endl;
    }
}

json OAuthRestClient::get(const std::string &url)
{
    refreshTokenIfNeeded();
    try
    {
        curlpp::Cleanup cleanup;
        curlpp::Easy request;
        request.setOpt(new curlpp::options::Url(url));
        request.setOpt(new curlpp::options::Verbose(true));

        std::list<std::string> headers;
        headers.push_back("Authorization: Bearer " + accessToken);
        headers.push_back("Origin: https://listen.tidal.com"); // TODO fix Origin
        request.setOpt(new curlpp::options::HttpHeader(headers));

        std::ostringstream responseStream;
        curlpp::options::WriteStream ws(&responseStream);
        request.setOpt(ws);

        request.perform();

        json jsonResponse = json::parse(responseStream.str());
        return jsonResponse;
    } catch (curlpp::RuntimeError& e) {
        std::cerr << "Error de tiempo de ejecución en getTracks: " << e.what() << std::endl;
    } catch (curlpp::LogicError& e) {
        std::cerr << "Error lógico en getTracks: " << e.what() << std::endl;
    }
    return {};
}
