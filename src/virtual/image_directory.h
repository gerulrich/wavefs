#ifndef IMAGES_COMPONENT_H
#define IMAGES_COMPONENT_H

#include "../wavefs/wavefs.h"

class ImageDirectory : public VirtualDirectory {
public:
    std::string getBasePath() override;
    int getattr(const char *path, struct stat *st) override;
    int readdir(const char *path, void *buffer, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi) override;
    int read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi) override;
    int open(const char *path, struct fuse_file_info *fi) override;
};

#endif // IMAGES_COMPONENT_H
