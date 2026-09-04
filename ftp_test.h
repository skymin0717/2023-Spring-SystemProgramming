#ifndef FTP_TEST_H
#define FTP_TEST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define True  1
#define False 0

#define BUF_SIZE 256

int  init(int argc, char *argv[]);
void ls(void);
void changedir(const char *name);
int  upload(int sock, const char *file_name);
int  download(int sock, const char *file_name);
void terminate(int sock);

#endif
