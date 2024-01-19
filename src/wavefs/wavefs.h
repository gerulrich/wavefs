#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include "../config/config.h"
#include "wavefs_types.h"
#include <fuse.h>
#include <string>
#include <vector>
#include <memory>

#include <mongocxx/pool.hpp>
#include <mongocxx/instance.hpp>
#include <bsoncxx/json.hpp>


class VirtualDirectory {
public:
    virtual int getattr(const char *path, struct stat *st) = 0;
    virtual int readdir(const char *path, void *buffer, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi) = 0;
    virtual int read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi) = 0;
    virtual int open(const char *path, struct fuse_file_info *fi) = 0;
    virtual std::string getBasePath() = 0;
    virtual ~VirtualDirectory() = default;
};


// https://www.cs.hmc.edu/~geoff/classes/hmc.cs135.201001/homework/fuse/fuse_doc.html
// https://stackoverflow.com/questions/46267972/fuse-avoid-calculating-size-in-getattr
namespace wavefs {
    class WaveFs {
        public:
            explicit WaveFs(config& config);
            int open(const char *path, struct fuse_file_info *fi);
            int getattr(const char *path, struct stat *st);
            int readdir(const char *path, void *buffer, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi);
            int read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi);
            int release(const char *path, fuse_file_info *fi);
    private:
        std::vector<std::unique_ptr<VirtualDirectory>> components;
        mongocxx::instance inst;
        static bool isResponsible(const std::string &path);
    };
}

#endif // FILESYSTEM_H
