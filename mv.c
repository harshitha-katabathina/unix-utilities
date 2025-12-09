#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <limits.h>
#include <errno.h>

int copy_file(const char *src, const char *dst) {
    int s = open(src, O_RDONLY);
    if (s < 0) return -1;
    struct stat st;
    if (fstat(s, &st) < 0) {
        close(s);
        return -1;
    }
    int d = open(dst, O_WRONLY | O_CREAT | O_TRUNC, st.st_mode & 0777);
    if (d < 0) { close(s); return -1; }

    char buf[4096];
    ssize_t n;
    while ((n = read(s, buf, sizeof(buf))) > 0) {
        ssize_t off = 0;
        while (off < n) {
            ssize_t w = write(d, buf + off, n - off);
            if (w < 0) { close(s); close(d); return -1; }
            off += w;
        }
    }
    if (n < 0) { close(s); close(d); return -1; }
    close(s);
    close(d);
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: mv <source> <destination>\n");
        return 1;
    }

    const char *src = argv[1];
    const char *dst = argv[2];

    struct stat st;
    char dstpath[PATH_MAX];

    if (stat(dst, &st) == 0 && S_ISDIR(st.st_mode)) {
        const char *base = strrchr(src, '/');
        if (base) base++; else base = src;
        if (snprintf(dstpath, sizeof(dstpath), "%s/%s", dst, base) >= (int)sizeof(dstpath)) {
            fprintf(stderr, "mv: destination path too long\n");
            return 1;
        }
    } else {
        if (strlen(dst) >= sizeof(dstpath)) {
            fprintf(stderr, "mv: destination path too long\n");
            return 1;
        }
        strcpy(dstpath, dst);
    }

    if (rename(src, dstpath) == 0) return 0;
    if (errno != EXDEV) { perror("mv"); return 1; }

    if (copy_file(src, dstpath) != 0) {
        perror("mv");
        unlink(dstpath);
        return 1;
    }

    if (unlink(src) != 0) {
        perror("mv");
        return 1;
    }
    return 0;
}
