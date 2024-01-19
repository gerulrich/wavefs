#define FUSE_USE_VERSION 31

#include "wavefs/wavefs.h"
#include <fuse.h>
#include <cstring>
#include <cassert>


/*
 * Command line options
 *
 * We can't set default values for the char* fields here because
 * fuse_opt_parse would attempt to free() them when the user specifies
 * different values on the command line.
 */
static struct options {
    const char *filename;
    int show_help;
} options;

#define OPTION(t, p)                           \
    { t, offsetof(struct options, p), 1 }
static const struct fuse_opt option_spec[] = {
        OPTION("--name=%s", filename),
        OPTION("-h", show_help),
        OPTION("--help", show_help),
        FUSE_OPT_END
};

static void show_help(const char * app_name)
{
    printf("usage: %s [options] <mountpoint>\n\n", app_name);
    printf("File-system specific options:\n"
           "    --config=<s>          path of the \"config\" file\n"
           "                        (default: \"/etc/wave/config.ini\")\n"
           "\n");
}

static struct fuse_operations fs_operations = {
    .getattr = [](const char *path, struct stat *buf) {
        return reinterpret_cast<wavefs::WaveFs *>(fuse_get_context()->private_data)->getattr(path, buf);
    },
    .open = [](const char *path, struct fuse_file_info *fi) {
        return reinterpret_cast<wavefs::WaveFs *>(fuse_get_context()->private_data)->open(path, fi);
    },
    .read = [](const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi)  {
        return reinterpret_cast<wavefs::WaveFs *>(fuse_get_context()->private_data)->read(path, buf, size, offset, fi);
    },
    .release = [](const char *path, struct fuse_file_info *fi) {
        return reinterpret_cast<wavefs::WaveFs *>(fuse_get_context()->private_data)->release(path, fi);
    },
    .readdir = [](const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi) {
        return reinterpret_cast<wavefs::WaveFs *>(fuse_get_context()->private_data)->readdir(path, buf, filler, offset, fi);
    },
};

int main(int argc, char *argv[]) {

    struct fuse_args args = FUSE_ARGS_INIT(argc, argv);

    // set defaults
    options.filename = strdup("/home/user/config.ini");

    if (fuse_opt_parse(&args, &options, option_spec, nullptr) == -1)
        return 1;

    if (options.show_help) {
        show_help(argv[0]);
        assert(fuse_opt_add_arg(&args, "--help") == 0);
        fuse_main(argc, argv, &fs_operations, nullptr);
        return 0;
    }

    wavefs::config config(options.filename);
    wavefs::WaveFs fs(config);
    int ret = fuse_main(argc, argv, &fs_operations, &fs);
    fuse_opt_free_args(&args);
    return ret;
}