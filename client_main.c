#include "ftp_test.h"

static void usage(void) {
    printf("commands:\n");
    printf("  ls\n");
    printf("  cd <dir>\n");
    printf("  put <file>     (upload)\n");
    printf("  get <file>     (download)\n");
    printf("  quit\n");
}

int main(int argc, char *argv[]) {
    int sock = init(argc, argv);
    if (sock == False) {
        printf("init failed\n");
        return 1;
    }
    printf("connected. (sock=%d)\n", sock);
    usage();

    char line[512];
    while (1) {
        printf("ftp> ");
        fflush(stdout);
        if (!fgets(line, sizeof(line), stdin)) break;

        line[strcspn(line, "\r\n")] = '\0';
        if (line[0] == '\0') continue;

        char *cmd = strtok(line, " \t");
        char *arg = strtok(NULL, " \t");
        if (!cmd) continue;

        if (!strcmp(cmd, "ls")) {
            ls();
        } else if (!strcmp(cmd, "cd")) {
            if (!arg) { printf("cd needs a directory\n"); continue; }
            changedir(arg);
        } else if (!strcmp(cmd, "put")) {
            if (!arg) { printf("put needs a file name\n"); continue; }
            printf(upload(sock, arg) == True ? "upload ok\n" : "upload failed\n");
        } else if (!strcmp(cmd, "get")) {
            if (!arg) { printf("get needs a file name\n"); continue; }
            printf(download(sock, arg) == True ? "download ok\n" : "download failed\n");
        } else if (!strcmp(cmd, "quit") || !strcmp(cmd, "exit")) {
            terminate(sock);
            printf("bye\n");
            return 0;
        } else {
            usage();
        }
    }

    terminate(sock);
    return 0;
}
