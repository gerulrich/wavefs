#include "tidal_client.h"
#include "types.h"
#include <iostream>
#include "nlohmann/json.hpp"

using json = nlohmann::json;

namespace wavefs::types {

    void from_json(const json &j, TrackInfo &t) {
        j.at("id").get_to(t.id);
        j.at("title").get_to(t.title);
        j.at("trackNumber").get_to(t.trackNumber);
        j.at("volumeNumber").get_to(t.volumeNumber);
        j.at("audioQuality").get_to(t.audioQuality);
    }

    void from_json(const json &j, StreamInfo &streamInfo) {
        j.at("trackId").get_to(streamInfo.trackId);
        j.at("assetPresentation").get_to(streamInfo.assetPresentation);
        j.at("audioMode").get_to(streamInfo.audioMode);
        j.at("audioQuality").get_to(streamInfo.audioQuality);
        j.at("manifestMimeType").get_to(streamInfo.manifestMimeType);
        j.at("manifestHash").get_to(streamInfo.manifestHash);
        j.at("manifest").get_to(streamInfo.manifest);
        //j.at("albumReplayGain").get_to(streamInfo.albumReplayGain);
        //j.at("albumPeakAmplitude").get_to(streamInfo.albumPeakAmplitude);
        //j.at("trackReplayGain").get_to(streamInfo.trackReplayGain);
        //j.at("trackPeakAmplitude").get_to(streamInfo.trackPeakAmplitude);
        //j.at("bitDepth").get_to(streamInfo.bitDepth);
        //j.at("sampleRate").get_to(streamInfo.sampleRate);
    }
}



std::vector<wavefs::types::TrackInfo> TidalClient::getTracks(const std::string &albumId, const std::string &countryCode)
{
    try
    {
        std::string url = "https://api.tidalhifi.com/v1/albums/" + albumId + "/tracks?countryCode=" + countryCode;
        json jsonResponse = get(url);
        std::vector<wavefs::types::TrackInfo> tracks;

        for (const auto& item : jsonResponse["items"])
        {
            auto track = item.template get<wavefs::types::TrackInfo>();
            tracks.push_back(track);
        }

        return tracks;
    } catch (curlpp::RuntimeError& e) {
        std::cerr << "Error de tiempo de ejecución en getTracks: " << e.what() << std::endl;
    } catch (curlpp::LogicError& e) {
        std::cerr << "Error lógico en getTracks: " << e.what() << std::endl;
    }

    // Devolver una lista vacía en caso de error
    return {};
}

wavefs::types::StreamInfo TidalClient::getStreamUrl(int trackId, const std::string &country, const std::string &quality)
{
    try
    {
        std::string url = "https://api.tidalhifi.com/v1/tracks/" + std::to_string(trackId)
                + "/playbackinfopostpaywall?countryCode=" + country
                + "&audioquality=" + quality + "&playbackmode=STREAM&assetpresentation=FULL";
        json jsonResponse = get(url);
        wavefs::types::StreamInfo streamInfo = jsonResponse.template get<wavefs::types::StreamInfo>();
        return streamInfo;
    } catch (curlpp::RuntimeError& e) {
        std::cerr << "Error de tiempo de ejecución en getStreamUrl: " << e.what() << std::endl;
    } catch (curlpp::LogicError& e) {
        std::cerr << "Error lógico en getStreamUrl: " << e.what() << std::endl;
    }
    return {};
}
