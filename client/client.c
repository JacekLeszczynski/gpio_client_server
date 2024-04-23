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


#define MAXDATASIZE 100      /* max number of bytes we can get at once */

char *BUF;

int test()
{
    int wlaczone,start_or_stop;
    BUF = ConfToBufor("/etc/default/gpio.client");
    wlaczone = strtobool(GetConfValue(BUF,"AUTO_ON_OFF","yes"));
    start_or_stop = atoi(GetConfValue(BUF,"START_OR_STOP","0"));
    printf("%d\n",wlaczone);
    printf("%d\n",start_or_stop);
    return 1;
}

int main(int argc, char *argv[])
{
    char *host;
    int port;
    int sockfd, numbytes;
    char buf[MAXDATASIZE];
    struct hostent *he;
    struct sockaddr_in their_addr; /* connector's address information */
    char *str_init,*komenda;
    char wlaczone,start_or_stop;
    char *tmp;

    //if (test()==1) return 0;

    if (argc != 2)
    {
        fprintf(stderr,"usage: client ON|OFF|STATUS|ON2-8|OFF2-8|STATUS2-8\n");
        exit(1);
    }
    komenda = argv[1];

    BUF = ConfToBufor("/etc/default/gpio.client");
    host = GetConfValue(BUF,"HOST","127.0.0.1");
    port = atoi(GetConfValue(BUF,"PORT","2021"));
    wlaczone = strtobool(GetConfValue(BUF,"AUTO_ON_OFF","yes"));
    start_or_stop = atoi(GetConfValue(BUF,"START_OR_STOP","0"));
    if (wlaczone==0 && (strcmp(komenda,"AUTO-ON")==0 || strcmp(komenda,"AUTO-OFF")==0)) return 0;
    if (start_or_stop==1 && strcmp(komenda,"AUTO-OFF")==0) return 0;
    if (start_or_stop==2 && strcmp(komenda,"AUTO-ON")==0) return 0;

    he = gethostbyname(host);

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("socket");
        exit(1);
    }

    their_addr.sin_family = AF_INET;      /* host byte order */
    their_addr.sin_port = htons(port);    /* short, network byte order */
    their_addr.sin_addr = *((struct in_addr *)he->h_addr);
    bzero(&(their_addr.sin_zero), 8);     /* zero the rest of the struct */

    if (connect(sockfd, (struct sockaddr *)&their_addr,sizeof(struct sockaddr)) == -1)
    {
        perror("connect");
        exit(1);
    }

    //str_init = String("tryb=gpio");
    //if (send(sockfd,str_init,strlen(str_init)+1,0) == -1)
    //{
    //    perror("send");
    //    exit (1);
    //}
    //usleep(500);

    komenda = upcase(komenda);

    if (strcmp(komenda,"AUTO-ON")==0) komenda = String("gpio=on");
    else if (strcmp(komenda,"AUTO-OFF")==0) komenda = String("gpio=off");
    else if (strcmp(komenda,"STATUS")==0) komenda = String("gpio=status");
    else if (strcmp(komenda,"STATUS2")==0) komenda = String("gpio2=status");
    else if (strcmp(komenda,"STATUS3")==0) komenda = String("gpio3=status");
    else if (strcmp(komenda,"STATUS4")==0) komenda = String("gpio4=status");
    else if (strcmp(komenda,"STATUS5")==0) komenda = String("gpio5=status");
    else if (strcmp(komenda,"STATUS6")==0) komenda = String("gpio6=status");
    else if (strcmp(komenda,"STATUS7")==0) komenda = String("gpio7=status");
    else if (strcmp(komenda,"STATUS8")==0) komenda = String("gpio8=status");
    else if (strcmp(komenda,"ON")==0) komenda = String("gpio=on");
    else if (strcmp(komenda,"OFF")==0) komenda = String("gpio=off");
    else if (strcmp(komenda,"ON2")==0) komenda = String("gpio2=on");
    else if (strcmp(komenda,"OFF2")==0) komenda = String("gpio2=off");
    else if (strcmp(komenda,"ON3")==0) komenda = String("gpio3=on");
    else if (strcmp(komenda,"OFF3")==0) komenda = String("gpio3=off");
    else if (strcmp(komenda,"ON4")==0) komenda = String("gpio4=on");
    else if (strcmp(komenda,"OFF4")==0) komenda = String("gpio4=off");
    else if (strcmp(komenda,"ON5")==0) komenda = String("gpio5=on");
    else if (strcmp(komenda,"OFF5")==0) komenda = String("gpio5=off");
    else if (strcmp(komenda,"ON6")==0) komenda = String("gpio6=on");
    else if (strcmp(komenda,"OFF6")==0) komenda = String("gpio6=off");
    else if (strcmp(komenda,"ON7")==0) komenda = String("gpio7=on");
    else if (strcmp(komenda,"OFF7")==0) komenda = String("gpio7=off");
    else if (strcmp(komenda,"ON8")==0) komenda = String("gpio8=on");
    else if (strcmp(komenda,"OFF8")==0) komenda = String("gpio8=off");

    //printf("Komenda do wysłania: %s\n",komenda);

    if (send(sockfd,komenda,strlen(komenda),0) == -1)
    {
        perror("send");
        exit (1);
    }

    if (strcmp(komenda,"gpio=status")==0
     || strcmp(komenda,"gpio2=status")==0
     || strcmp(komenda,"gpio3=status")==0
     || strcmp(komenda,"gpio4=status")==0
     || strcmp(komenda,"gpio5=status")==0
     || strcmp(komenda,"gpio6=status")==0
     || strcmp(komenda,"gpio7=status")==0
     || strcmp(komenda,"gpio8=status")==0)
    {
        if ((numbytes=recv(sockfd, buf, MAXDATASIZE, 0)) == -1)
        {
            perror("recv");
            exit(1);
        }
        buf[numbytes] = '\0';
        printf("%s\n",buf);
    }

    close(sockfd);

    return 0;
}

