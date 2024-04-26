#include "f_includes.c"
#include "f_defines.c"
#include "f_global.c"
#include "f_functions.c"
#include "komunikacja.c"
#include "f_daemon.c"

#define CONST_TEST 0

/* GŁÓWNA FUNKCJA STARTOWA */
int main(int argc,char *argv[])
{
    struct sockaddr_in my_addr,their_addr;
    int their_sock;
    int option = 1;
    socklen_t their_addr_size;
    int portno;
    pthread_t sendt,recvt,watek0,watek1;
    char msg[CONST_MAX_BUFOR], *BUF, *s, *s1, *s2;
    int len,i;
    struct client_info cl;
    struct hostent *he;
    char ip[INET_ADDRSTRLEN],IP[INET_ADDRSTRLEN];
    char *HOST;
    int PORT;

    if(argc > 2)
    {
	printf("too many arguments");
	exit(1);
    }

    BUF = ConfToBufor("/etc/default/gpio.client");
    HOST = GetConfValue(BUF,"HOST","serwer");
    PORT = atoi(GetConfValue(BUF,"PORT","2122"));

    if (CONST_TEST) fprintf(stderr,"host=%s port=%d\n",HOST,PORT);

    if (!CONST_TEST) daemonize();
    Randomize();

    he = gethostbyname(HOST);

    while(1)
    {
        if ((my_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        {
            perror("socket");
            sleep(1);
            continue;
        }
        their_addr.sin_family = AF_INET;      /* host byte order */
        their_addr.sin_port = htons(PORT);    /* short, network byte order */
        their_addr.sin_addr = *((struct in_addr *)he->h_addr);
        bzero(&(their_addr.sin_zero), 8);     /* zero the rest of the struct */
        if (connect(my_sock, (struct sockaddr *)&their_addr,sizeof(struct sockaddr)) == -1)
        {
            perror("connect");
            sleep(1);
            continue;
        }
        s = String("tryb=gpio");
        sendmessage(s,my_sock,0);
        while(1)
        {
            if (CONST_TEST) fprintf(stderr,"Czekam na ramki...\n");
            if ((len=recv(my_sock, msg, CONST_MAX_BUFOR, 0)) == -1)
            {
                perror("recv");
                exit(1);
            } else
            if (len==0) {
                if (CONST_TEST) fprintf(stderr,"%s\n","LEN = 0");
                break;
            }

            msg[len] = '\0';
            s1 = GetLineToStr(msg,1,'=',"");
            s2 = GetLineToStr(msg,2,'=',"");
            if (CONST_TEST) fprintf(stderr,"[%d]: %s = %s\n",len,s1,s2);

            if (strcmp(s1,"pilot")==0){

                if (CONST_TEST) if (strcmp(s2,"key_mic")==0)
                {
                    BEXIT = 1;
                    break;
                }

                if (strcmp(s2,"key_volume_up")==0) SetVolumeUp(); else
                if (strcmp(s2,"key_volume_down")==0) SetVolumeDown(); else
                if (strcmp(s2,"key_power")==0) ShutdownNow();
            }

        }
        close(my_sock);
        if (BEXIT) break; else
        sleep(1);
    }

    if (CONST_TEST) fprintf(stderr,"Wychodzę.\n");
    return 0;
}
