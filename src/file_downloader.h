#ifndef FILE_DOWNLOADER_H
#define FILE_DOWNLOADER_H

#include <string>
#include <random>
#include "wavefs/wavefs_types.h"

class FileDownloader {
public:
    FileDownloader();
    ~FileDownloader();

    VirtualFile * download(const std::string& url);

private:
    std::string generateTempFileName();

};

#endif // FILE_DOWNLOADER_H