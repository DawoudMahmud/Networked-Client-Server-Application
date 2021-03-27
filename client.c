//Ramsey Shafi  ID# 51476181
//Dawoud Mahmud ID# 46564710
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <netdb.h>
#include <math.h>
#define MAXLINE 80
#define MAXARGC 80 

int i = 0;
int j = 0;
int k = 0;
int counter = 0;
int invalid = 0;
int len = 0;

void parseline(char cmdline[], char *argv[]);
int open_clientfd(char* host, char* port);

int main(int argc, char *argv[]) {
    int offset;
    char *host = argv[1];
    char *port = argv[2];
    char buf[256];

    int clientfd = open_clientfd(host,port);
    while (1) {
        char cmdline[MAXLINE];
        char *argvp[MAXARGC];
        printf("> ");
        fgets(cmdline, MAXLINE, stdin);
        int len = strlen(cmdline);
        if (feof(stdin)) {
            exit(0);
        }
        if (len > 0 && cmdline[len-1] == '\n') {
            cmdline[len-1] = '\0';
        }
        if (strcmp(cmdline,"quit") == 0) {
            exit(0);
        }
        parseline(cmdline, argvp);

        if (strcmp(argvp[0], "Prices") == 0) {
            if (strcmp(argvp[1], "AAPL") != 0 && strcmp(argvp[1], "TWTR") != 0) {
                printf("Invalid syntax\n");
                continue;
            }
            for (i = 0; i < strlen(argvp[2]); ++i) {
                if ((i == 4 && argvp[2][i] != '-') ||
                    (i == 7 && argvp[2][i] != '-') ||
                    ((i!=4 && i!=7) && !isdigit(argvp[2][i]))) {
                        printf("Invalid syntax\n");
                        invalid = 1;
                        break;
                    }
            }
            if (invalid == 1) {
                invalid = 0;
                continue;
            }
            memset(&buf[0], 0, sizeof(buf));
            counter = 0;
            for (i = 1; i < strlen(argvp[0])+1; ++i) {
                buf[i] = argvp[0][i-1];
                counter++;
            }
            buf[i++] = ' ';
            counter++;
            for (j = 0; j < strlen(argvp[1]); ++j) {
                buf[i++] = argvp[1][j];
                counter++;
            }
            buf[i++] = ' ';
            counter++;
            for (k = 0; k < strlen(argvp[2]); ++k) {
                buf[i++] = argvp[2][k];
                counter++;
            }
            buf[0] = counter;
            write(clientfd, buf, strlen(buf));
            memset(&buf[0], 0, sizeof(buf));
            read(clientfd, buf, MAXLINE);
            for (i = 1; i < strlen(buf); ++i) {
                buf[i-1] = buf[i];
            }
            buf[i-1] = '\0';
            printf("%s\n", buf);
        }
        else if(strcmp(argvp[0], "MaxProfit") == 0) {
            if (strcmp(argvp[1], "AAPL") != 0 && strcmp(argvp[1], "TWTR") != 0) {
                printf("Invalid syntax\n");
                continue;
            }
            if (argvp[2]) {
                printf("Invalid syntax\n");
                continue;
            }
            memset(&buf[0], 0, sizeof(buf));
            counter = 0;
            for (i = 1; i < strlen(argvp[0])+1; ++i) {
                buf[i] = argvp[0][i-1];
                counter++;
            }
            buf[i++] = ' ';
            counter++;
            for (j = 0; j < strlen(argvp[1]); ++j) {
                buf[i++] = argvp[1][j];
                counter++;
            }
            buf[0] = counter;
            write(clientfd, buf, strlen(buf));
            memset(&buf[0], 0, sizeof(buf));
            read(clientfd, buf, MAXLINE);
            for (i = 1; i < strlen(buf); ++i) {
                buf[i-1] = buf[i];
            }
            buf[i-1] = '\0';
            printf("Maximum profit for %s: %s\n", argvp[1], buf);
        }

        else {
            printf("Invalid syntax\n");
        }
    }
}

int open_clientfd(char* host, char* port) {
    int clientfd;
    struct addrinfo hints, *listp, *p;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_socktype= SOCK_STREAM;
    hints.ai_flags= AI_NUMERICSERV;
    hints.ai_flags|= AI_ADDRCONFIG;
    getaddrinfo(host, port, &hints, &listp);
 
    for (p = listp; p; p = p->ai_next) {
        if ((clientfd= socket(p->ai_family, p->ai_socktype,p->ai_protocol)) < 0) {
            continue;
        }
        if (connect(clientfd, p->ai_addr, p->ai_addrlen) != -1) {
            break;
        }
        close(clientfd);
    }
    freeaddrinfo(listp);
    if (!p) {
        return 1;
    }
    else {
        return clientfd;
    }
}

void parseline(char cmdline[], char *argvp[]) {
    counter = 0;
    char* token = strtok(cmdline, " ");
    while (token != NULL) {
        argvp[counter++] = token;
        token = strtok(NULL, " ");
    }
    argvp[counter] = NULL;
}