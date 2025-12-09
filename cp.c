#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: cp <source> <destination>\n");
        return 1;
    }

    int s = open(argv[1], O_RDONLY);
    if (s < 0) {
        perror("cp");
        return 1;
    }

    int d = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (d < 0) {
        perror("cp");
        close(s);
        return 1;
    }

    char buf[4096];
    ssize_t n;
    while ((n = read(s, buf, sizeof(buf))) > 0) {
        ssize_t wrote = 0;
        while (wrote < n) {
            ssize_t w = write(d, buf + wrote, n - wrote);
            if (w < 0) {
                perror("cp");
                close(s);
                close(d);
                return 1;
            }
            wrote += w;
        }
    }
    if (n < 0) perror("cp");
    close(s);
    close(d);
    return (n < 0) ? 1 : 0;
}
