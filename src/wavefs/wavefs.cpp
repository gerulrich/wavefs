#define FUSE_USE_VERSION 31

#include "wavefs.h"
#include "../virtual/music_directory.h"
#include <fuse.h>
#include <string>
#include <memory>
#include <unistd.h>
#include <cstring>
#include <mongocxx/uri.hpp>

#define NO_OP(param) do { (void)(param); } while(0)


namespace wavefs {

    WaveFs::WaveFs(config& config) :
        inst{}
    {
        components.push_back(std::make_unique<MusicDirectory>(config));
    }

    int WaveFs::getattr(const char *path, struct stat *st) {
        st->st_uid = getuid();
        st->st_gid = getgid();
        st->st_atime = time(nullptr); // The last "a"ccess of the file/directory is right now
        st->st_mtime = time(nullptr); // The last "m"odification of the file/directory is right now

        if (strcmp(path, "/") == 0) {
            st->st_mode = S_IFDIR | 0755;
            st->st_nlink = 2; // Why "two" hardlinks instead of "one"? The answer is here: http://unix.stackexchange.com/a/101536
            return 0;
        }

        for (const auto &component: components) {
            if (isResponsible(component->getBasePath())) {
                return component->getattr(path, st);
            }
        }

        return -ENOENT;
    }

    int
    WaveFs::readdir(const char *path, void *buffer, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi) {
        filler(buffer, ".", nullptr, 0);
        filler(buffer, "..", nullptr, 0);

        if (strcmp(path, "/") == 0) {
            filler(buffer, "music", nullptr, 0);
            return 0;
        }

        for (const auto &component: components) {
            if (isResponsible(component->getBasePath())) {
                return component->readdir(path, buffer, filler, offset, fi);
            }
        }

        return -ENOENT;
    }

    int WaveFs::read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
        auto * file = reinterpret_cast<VirtualFile*>(fi->fh);
        if (file) {
            if (file->file != -1) // O cualquier otro valor que indique un descriptor de archivo válido
            {
                // Usar lseek para establecer la posición del descriptor de archivo
                off_t result = ::lseek(file->file, offset, SEEK_SET);

                if (result == -1)
                {
                    perror("lseek error");
                    return -errno;
                }

                // Leer desde el descriptor de archivo
                ssize_t bytes_read = ::read(file->file, buf, size);

                if (bytes_read == -1)
                {
                    perror("read error");
                    return -errno;
                }
                return  static_cast<int>(bytes_read);
            }
        }

        for (const auto &component: components) {
            if (isResponsible(component->getBasePath())) {
                return component->read(path, buf, size, offset, fi);
            }
        }
        return -ENOENT;
    }

    int WaveFs::open(const char *path, struct fuse_file_info *fi) {
        // TODO mover fi->direct_io = 1;
        // Your logic for opening the file goes here
        // Set the direct_io flag to bypass the page cache
        fi->direct_io = 1;
        // Return any necessary flags (e.g., O_RDONLY)
        fi->flags |= O_RDONLY;
        // If you need to store information about the open file, you can use fi->fh
        // Return 0 to indicate success

        for (const auto &component: components) {
            if (isResponsible(component->getBasePath())) {
                return component->open(path, fi);
            }
        }

        return 0;
    }

    int WaveFs::release(const char *path, fuse_file_info *fi) {
        NO_OP(path);
        auto * file = reinterpret_cast<VirtualFile*>(fi->fh);
        if (file)
        {
            if (file->file != -1) // O cualquier otro valor que indique un descriptor de archivo válido
            {
                ::close(file->file);
            }

            if (!file->filename.empty())
            {
                if (remove(file->filename.c_str()) != 0)
                {
                    perror("Error al eliminar el archivo temporal");
                }
            }
            fi->fh = 0;
        }
        return 0;
    }

    bool WaveFs::isResponsible(const string &path) {
        return std::string(path).find(path) == 0;
    }

}