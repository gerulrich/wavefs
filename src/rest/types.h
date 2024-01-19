#ifndef WAVEFS_TYPES_H
#define WAVEFS_TYPES_H
#include <string>

namespace wavefs::types {

    struct TrackInfo {
        int id;
        std::string title;
        int trackNumber;
        int volumeNumber;
        std::string audioQuality;
    };

    struct StreamInfo {
        int trackId;
        std::string assetPresentation;
        std::string audioMode;
        std::string audioQuality;
        std::string manifestMimeType;
        std::string manifestHash;
        std::string manifest;
        //double albumReplayGain;
        //double albumPeakAmplitude;
        //double trackReplayGain;
        //double trackPeakAmplitude;
        //int bitDepth;
        int sampleRate;
    };
}

#endif //WAVEFS_TYPES_H
