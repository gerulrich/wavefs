#ifndef WAVEFS_WAVEFS_TYPES_H
#define WAVEFS_WAVEFS_TYPES_H

#include <string>

enum FileType {
    PARTIAL,
    FULL
};

struct VirtualFile
{
    int file;
    FileType type;
    std::string filename;
};

#endif //WAVEFS_WAVEFS_TYPES_H
