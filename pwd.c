#include <unistd.h>
#include <stdio.h>
#include <limits.h>

int main(void) {
    char buf[PATH_MAX];
    if (getcwd(buf, sizeof(buf)) == NULL) {
        perror("pwd");
        return 1;
    }
    printf("%s\n", buf);
    return 0;
}
