#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "lazc.h"
#include <locale.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <netdb.h>


#define PORT 2122            /* the port client will be connecting to  */
#define MAXDATASIZE 100      /* max number of bytes we can get at once */
#define LOGIC_REVERSE 1      /* zamienia miejscami - 0 to 1, a 1 to 0  */

int test()
{
    char wlaczone,start_or_stop;
    wlaczone = strtobool(GetDefault("/etc/default/gpio.client","AUTO_ON_OFF","yes"));
    start_or_stop = atoi(GetDefault("/etc/default/gpio.client","START_OR_STOP","0"));
    printf("%d\n",wlaczone);
    printf("%d\n",start_or_stop);
    return 1;
}

int main(int argc, char *argv[])
{
    int sockfd, numbytes;
    char buf[MAXDATASIZE];
    struct hostent *he;
    struct sockaddr_in their_addr; /* connector's address information */
    char *komenda;
    char wlaczone,start_or_stop;

    //if (test()==1) return 0;

    if (argc != 2)
    {
        fprintf(stderr,"usage: clieent ON|OFF|STATUS\n");
        exit(1);
    }

    wlaczone = strtobool(GetDefault("/etc/default/gpio.client","AUTO_ON_OFF","yes"));
    start_or_stop = atoi(GetDefault("/etc/default/gpio.client","START_OR_STOP","0"));
    if (wlaczone==0) return 0;
    if (start_or_stop==1 && strcmp(komenda,"AUTO-OFF")==0) return 0;
    if (start_or_stop==2 && strcmp(komenda,"AUTO-ON")==0) return 0;

    komenda = argv[1];
    he = gethostbyname("192.168.10.2");

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("socket");
        exit(1);
    }

    their_addr.sin_family = AF_INET;      /* host byte order */
    their_addr.sin_port = htons(PORT);    /* short, network byte order */
    their_addr.sin_addr = *((struct in_addr *)he->h_addr);
    bzero(&(their_addr.sin_zero), 8);     /* zero the rest of the struct */

    if (connect(sockfd, (struct sockaddr *)&their_addr,sizeof(struct sockaddr)) == -1)
    {
        perror("connect");
        exit(1);
    }

    if (strcmp(komenda,"AUTO-ON")==0) komenda = String("ON");
    if (strcmp(komenda,"AUTO-OFF")==0) komenda = String("OFF");

    if (LOGIC_REVERSE) {
        if (strcmp(komenda,"ON")==0) {
            komenda = String("OFF");
        } else {
            if (strcmp(komenda,"OFF")==0) komenda = String("ON");
        }
    }

    if (send(sockfd,komenda,strlen(komenda),0) == -1)
    {
        perror("send");
        exit (1);
    }
    //printf("After the send function \n");

    if (strcmp(komenda,"STATUS")==0)
    {
        if ((numbytes=recv(sockfd, buf, MAXDATASIZE, 0)) == -1)
        {
            perror("recv");
            exit(1);
        }
        buf[numbytes] = '\0';
        if (LOGIC_REVERSE) {
            if (strcmp(buf,"1")==0) {
                printf("0\n");
            } else {
                printf("1\n");
            }
        } else {
            printf("%s\n",buf);
        }
    }

    close(sockfd);

    return 0;
}

