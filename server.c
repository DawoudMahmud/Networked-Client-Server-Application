//Ramsey Shafi  ID# 51476181
//Dawoud Mahmud ID# 46564710
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <netdb.h>
#include <math.h>
#define LISTENQ 1
#define MAXLINE 80

int i = 0;
int n = 0;
int j = 0;
int counter = 0;
int len = 0;
int stockPos = 0;
char* aapl[503][2];
char* twtr[503][2];
char maxProfitVal[256];
char priceVal[256];
char stock[4];
char date[10];

int open_listenfd(char *port);
void prices(char* stock, char* date);
void maxProfit(char *stock);

int main(int argc, char *argv[]) {
    char buf[256];
    char temp[256];
    char* tok;
    int listenfd, connfd;
    socklen_t clientlen;
    struct sockaddr_storage clientaddr;
    typedef struct sockaddr SA;
    char client_hostname[MAXLINE], client_port[MAXLINE];
    FILE* aaplFile = fopen(argv[1], "r");
    fgets(buf, sizeof(buf), aaplFile);
    FILE* twtrFile = fopen(argv[2], "r");
    fgets(buf, sizeof(buf), twtrFile);
    for (i = 0; i < 503; ++i) {
        fgets(buf, sizeof(buf), aaplFile);
        tok = strtok(buf, ",");
        while(tok != NULL) {
            if (counter == 0) {
                aapl[i][0] = strdup(tok);
            }
            else if (counter == 4) {
                aapl[i][1] = strdup(tok);
            }
            tok = strtok(NULL, ",");
            counter++;
        }
        counter = 0;
        fgets(buf, sizeof(buf), twtrFile);
        tok = strtok(buf, ",");
        while(tok != NULL) {
            if (counter == 0) {
                twtr[i][0] = strdup(tok);
            }
            else if (counter == 4) {
               twtr[i][1] = strdup(tok);
            }
            tok = strtok(NULL, ",");
            counter++;
        }
        counter = 0;
    }

    printf("server started\n");
    listenfd = open_listenfd(argv[3]);

    while(1) {
        memset(&buf[0], 0, sizeof(buf));
        memset(&temp[0], 0, sizeof(temp));
        clientlen = sizeof(struct sockaddr_storage);
        connfd = accept(listenfd, (SA *)&clientaddr, &clientlen);
        getnameinfo((SA *) &clientaddr, clientlen,client_hostname, MAXLINE, client_port, MAXLINE, 0);
        while ((n = read(connfd, temp, MAXLINE)) != 0) {
            for (i = 1; i < strlen(temp); ++i) {
                temp[i-1] = temp[i];
            }
            temp[i-1] = '\0';
            for (i = 0; i < strlen(temp); ++i) {
                if (temp[i] == 'A' || temp[i] == 'T') {
                    break;
                } 
            }
            for (j=0; j < strlen(temp); ++j) {
                buf[j] = temp[i++];
            }
            printf("%s\n", temp);
            memset(&temp[0], 0, sizeof(temp));
            if (strcmp(buf, "AAPL") == 0) {
                maxProfit("AAPL");
                strcpy(buf,maxProfitVal);
                len = strlen(buf);
                buf[0] = len;
                for (i = 1; i < len + 1; ++i) {
                    buf[i] = maxProfitVal[i-1];
                }
                write(connfd, buf, strlen(buf));
                memset(&buf[0], 0, sizeof(buf));
            }
            else if (strcmp(buf, "TWTR") == 0) {
                maxProfit("TWTR");
                strcpy(buf,maxProfitVal);
                len = strlen(buf);
                buf[0] = len;
                for (i = 1; i < len + 1; ++i) {
                    buf[i] = maxProfitVal[i-1];
                }
                write(connfd, buf, strlen(buf));
                memset(&buf[0], 0, sizeof(buf));
            }  
            else {
                for (i = 0; i < 15; ++i) {
                    if (i < 4) {
                        stock[i] = buf[i];
                    }
                    else if (i > 4) {
                        date[i-5] = buf[i];
                    }
                }
                memset(&buf[0], 0, sizeof(buf));
                prices(stock,date);
                strcpy(buf,priceVal);
                len = strlen(buf);
                buf[0] = len +'0';
                for (i = 1; i < len + 1; ++i) {
                    buf[i] = priceVal[i-1];
                }
                write(connfd, buf, strlen(buf));
                memset(&buf[0], 0, sizeof(buf));
            }
        }
        close(connfd);
    }
    exit(0);
}

int open_listenfd(char* port)  {
    struct addrinfo hints, *listp, *p;
    int listenfd, optval=1;
    
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_socktype= SOCK_STREAM; 
    hints.ai_flags= AI_PASSIVE | AI_ADDRCONFIG;
    hints.ai_flags|= AI_NUMERICSERV; 
    getaddrinfo(NULL,port,&hints,&listp);

    for (p = listp; p; p = p->ai_next) {
        if ((listenfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) < 0) {
            continue;
        }
        setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (const void*)&optval, sizeof(int));   
        if (bind(listenfd, p->ai_addr, p->ai_addrlen) == 0)
            break;
        close(listenfd);
    }
    freeaddrinfo(listp);
    if (!p) {
        return -1;
    }
    if(listen(listenfd, LISTENQ) < 0) {
        close(listenfd);
        return -1;
    }
    return listenfd;

}

void prices(char* stock, char* date) {
    if (strcmp(stock, "AAPL") == 0) {
        for (i = 0; i < 503; ++i) {
            if (strcmp(aapl[i][0], date) == 0) {
                strcpy(priceVal,aapl[i][1]);
                return;
            }
        }
        strcpy(priceVal,"Unknown");
    }
    else {
        for (i = 0; i < 503; ++i) {
            if (strcmp(twtr[i][0], date) == 0) {
                strcpy(priceVal,twtr[i][1]);
                return;
            }
        }
        strcpy(priceVal,"Unknown");
    }
}

void maxProfit(char* stock) {
    int mindex = 0;
    float cur_profit = 0.0;
    float max_profit = 0.0;
    if (strcmp(stock, "AAPL") == 0) {
        for (i = 1; i < 503; ++i) {
            cur_profit = atof(aapl[i][1]) - atof(aapl[mindex][1]);
            if (cur_profit > max_profit) {

                max_profit = cur_profit;
            }
            else if (atof(aapl[i][1]) < atof(aapl[mindex][1])) {
                mindex = i;
            }
        }
    }
    else {
        for (i = 0; i < 503; ++i) {
            cur_profit = atof(twtr[i][1]) - atof(twtr[mindex][1]);
            if (cur_profit > max_profit) {
                max_profit = cur_profit;
            }
            else if (atof(twtr[i][1]) < atof(twtr[mindex][1])) {
                mindex = i;
            }
        }
    }
    gcvt(max_profit, 5, maxProfitVal);
}