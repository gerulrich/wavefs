#include "file_downloader.h"
#include <unordered_set>
#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <fstream>
#include <fcntl.h>
#include <random>
#include <ctime>


FileDownloader::FileDownloader() {
}

FileDownloader::~FileDownloader() = default;

VirtualFile* FileDownloader::download(const std::string& url)
{
    std::string tempFileName = generateTempFileName();

    try
    {
        std::ofstream outfile(tempFileName, std::ios::binary);

        if (outfile)
        {
            curlpp::Cleanup cleanup;
            curlpp::Easy request;
            request.setOpt(new curlpp::options::Url(url));
            request.setOpt(new curlpp::options::WriteStream(&outfile));

            request.perform();
            outfile.close();

            auto * fileInfo = new VirtualFile;
            fileInfo->filename = tempFileName;
            return fileInfo;
        }
        else
        {
            std::cerr << "Error al abrir el archivo temporal" << std::endl;
        }
    }
    catch (curlpp::RuntimeError& e)
    {
        std::cerr << "Error de tiempo de ejecución: " << e.what() << std::endl;
    }
    catch (curlpp::LogicError& e)
    {
        std::cerr << "Error lógico: " << e.what() << std::endl;
    }
    auto * fileInfo = new VirtualFile;
    fileInfo->file = -1;
    fileInfo->filename = "";
    return fileInfo;
}

std::string FileDownloader::generateTempFileName() {
    std::srand(std::time(nullptr));
    std::string tempDir = "/tmp/";
    std::string prefix = "temp_file_";
    std::string suffix = std::to_string(std::rand());

    return tempDir + prefix + suffix;
}
