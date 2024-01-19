#include "image_directory.h"

std::string ImageDirectory::getBasePath() {
    return "/image";
}

int ImageDirectory::getattr(const char *path, struct stat *st) {
    // Implementation for getattr in ImageDirectory
    return 0;
}

int ImageDirectory::readdir(const char *path, void *buffer, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi) {
    // Implementation for readdir in ImageDirectory
    return 0;
}

int ImageDirectory::read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
    return 0;
}

int ImageDirectory::open(const char *path, struct fuse_file_info *fi) {
    return 0;
}
