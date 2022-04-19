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
#include <linux/input.h>
//#include <iostream>
//#include <ncurses.h>

//#define RUNNING_DIR	"/disk/dbases"
#define LOCK_FILE	"/var/run/server.gpio.pid"
//#define LOG_FILE	"/disk/log/studiopi.gpio.log"
//#define GPIO_NR "492"

#define CONST_MAX_CLIENTS 100
#define CONST_MAX_BUFOR 20000

struct client_info {
    int sockno;
    char ip[INET_ADDRSTRLEN];
    int port;
};

int my_sock, is_port;
int clients[CONST_MAX_CLIENTS];
char ips[CONST_MAX_CLIENTS][INET_ADDRSTRLEN];
int ports[CONST_MAX_CLIENTS];
char *GPIO_NR,*PATH_KBD;
bool REVERSE;

int n = 0, mn = 0, ischat = 0;
int error = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int idsock(int soket)
{
    int i, a = -1;
    for(i=0; i<n; i++)
    {
        if (clients[i]==soket)
        {
            a=i;
            break;
        }
    }
    return a;
}

#include "komunikacja.c"

/* WĄTEK CZYTAJĄCY NACIŚNIĘCIE KLAWISZY PILOTA */
void *recvkb(void *arg)
{
    const char *dev = PATH_KBD;
    int fd;
    char *msg;
    fd = open(dev, O_RDONLY); // Open the buffer
    if (fd != -1) {
        struct input_event ev;
        ssize_t n;

        while (1) {
            n = read(fd, &ev, sizeof ev); // Read from the buffer
            if (ev.type == EV_KEY && ev.value == 1)
            {
                msg = concat("KEY=",itoa(ev.code,10));
                sendtoall(msg,-1,1,1);
            }
        }
        close(fd);
        fflush(stdout);
    }
}

/* WĄTEK POŁĄCZENIA */
void *recvmg(void *sock)
{
    struct client_info cl = *((struct client_info *)sock);
    bool TerminateNow = 0, czysc = 0;
    int len,i,j,a;
    char msg[CONST_MAX_BUFOR],*s;

    while((len = recv(cl.sockno,&msg,CONST_MAX_BUFOR,0)) > 0)
    {
        if (len<=0) continue;
        s = strdup(msg);

        if (strcmp(s,"ON")==0) {
            export_GPIO();
            direction_GPIO();
            if (REVERSE) set_GPIO(0); else set_GPIO(1);
            unexport_GPIO();
            if (REVERSE) sendmessage("1",cl.sockno,1); else sendmessage("0",cl.sockno,1);
        } else
        if (strcmp(s,"OFF")==0) {
            export_GPIO();
            direction_GPIO();
            if (REVERSE) set_GPIO(1); else set_GPIO(0);
            unexport_GPIO();
            if (REVERSE) sendmessage("0",cl.sockno,1); else sendmessage("1",cl.sockno,1);
        } else
        if (strcmp(s,"STATUS")==0) {
            export_GPIO();
            a = get_GPIO();
            unexport_GPIO();
            if (a==1) {
                if (REVERSE) sendmessage("0",cl.sockno,1); else sendmessage("1",cl.sockno,1);
            } else {
                if (REVERSE) sendmessage("1",cl.sockno,1); else sendmessage("0",cl.sockno,1);
            }
        } else
        if (strcmp(s,"TV_ON")==0) {
            system("systemctl start tvheadend");
            sendmessage("STATUS_TV_ON",cl.sockno,1);
        } else
        if (strcmp(s,"TV_OFF")==0) {
            system("systemctl stop tvheadend");
            sendmessage("STATUS_TV_OFF",cl.sockno,1);
        }

        //sendmessage(s,cl.sockno,1);

        if (TerminateNow) break;
    }  /* pętla główna recv i pętla wykonywania gotowych zapytań */

    pthread_mutex_lock(&mutex);
    for(i = 0; i < n; i++) {
	if(clients[i] == cl.sockno)
        {
	    j = i;
	    while(j < n-1)
            {
		clients[j] = clients[j+1];
                strcpy(ips[j],ips[j+1]);
                ports[j] = ports[j+1];
		j++;
	    }
	}
    }
    n--;
    pthread_mutex_unlock(&mutex);
    shutdown(cl.sockno,SHUT_RDWR);
}

void signal_handler(int sig)
{
    char *ss;
    switch(sig)
    {
        case SIGHUP:
	    //log_message(LOG_FILE,"hangup signal catched");
	    break;
        case SIGINT:
        case SIGTERM:
            //shutdown(my_sock,2);
            //close(my_sock);
            //log_message(LOG_FILE,"terminate signal catched");
            exit(0);
            break;
    }
}

void daemonize()
{
    int i,lfp;
    char str[10];

    if(getppid()==1) return; /* already a daemon */
    i=fork();
    if (i<0) exit(1); /* fork error */
    if (i>0) exit(0); /* parent exits */
    /* child (daemon) continues */
    setsid(); /* obtain a new process group */
    for (i=getdtablesize();i>=0;--i) close(i); /* close all descriptors */
    i=open("/dev/null",O_RDWR); dup(i); dup(i); /* handle standart I/O */
    umask(027); /* set newly created file permissions */
    //chdir(RUNNING_DIR); /* change running directory */
    lfp=open(LOCK_FILE,O_RDWR|O_CREAT,0640);
    if (lfp<0) exit(1); /* can not open */
    if (lockf(lfp,F_TLOCK,0)<0) exit(0); /* can not lock */
    /* first instance continues */
    sprintf(str,"%d\n",getpid());
    write(lfp,str,strlen(str)); /* record pid to lockfile */
    signal(SIGCHLD,SIG_IGN); /* ignore child */
    signal(SIGTSTP,SIG_IGN); /* ignore tty signals */
    signal(SIGTTOU,SIG_IGN);
    signal(SIGTTIN,SIG_IGN);
    signal(SIGHUP,signal_handler); /* catch hangup signal */
    signal(SIGTERM,signal_handler); /* catch kill signal */
}

/* GŁÓWNA FUNKCJA STARTOWA */
int main(int argc,char *argv[])
{
    struct sockaddr_in my_addr,their_addr;
    int their_sock;
    int option = 1;
    socklen_t their_addr_size;
    int portno;
    pthread_t sendt,recvt,recvt2,udpvt;
    char msg[CONST_MAX_BUFOR], *BUF;
    int len,i;
    struct client_info cl;
    char ip[INET_ADDRSTRLEN],IP[INET_ADDRSTRLEN];
    int PORT;

    if(argc > 2)
    {
	printf("too many arguments");
	exit(1);
    }

    BUF = ConfToBufor("/etc/default/gpio.server");
    portno = atoi(GetConfValue(BUF,"PORT","2122"));
    GPIO_NR = GetConfValue(BUF,"GPIO_NUMBER","492");
    REVERSE = atoi(GetConfValue(BUF,"REVERSE","0"));
    bool SCAN_KEYBOARD = atoi(GetConfValue(BUF,"SCAN_KEYBOARD","0"));
    if (SCAN_KEYBOARD){
        PATH_KBD = GetConfValue(BUF,"PATH_DEV_KEYBOARD","");
        if (strcmp(PATH_KBD,"")==0) SCAN_KEYBOARD = 0;
    }
    daemonize();
    Randomize();

    my_sock = socket(AF_INET,SOCK_STREAM,0);
    setsockopt(my_sock, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
    memset(my_addr.sin_zero,'\0',sizeof(my_addr.sin_zero));
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(portno);
    my_addr.sin_addr.s_addr = 0; //inet_addr("0.0.0.0");
    their_addr_size = sizeof(their_addr);

    if(bind(my_sock,(struct sockaddr *)&my_addr,sizeof(my_addr)) != 0)
    {
        perror("binding unsuccessful");
        //log_message(LOG_FILE,"Zajęty port!");
        //mysql_close(db);
        exit(1);
    };

    is_port = portno;

    if(listen(my_sock,5) != 0)
    {
	perror("listening unsuccessful");
        //log_message(LOG_FILE,"Problem z listiningiem.");
	exit(1);
    }

    if (SCAN_KEYBOARD) pthread_create(&recvt2,NULL,recvkb,NULL);
    while(1)
    {
        if((their_sock = accept(my_sock,(struct sockaddr *)&their_addr,&their_addr_size)) < 0)
        {
            perror("accept unsuccessful");
            exit(1);
        }
	//perror("User is logging...");
        pthread_mutex_lock(&mutex);
        inet_ntop(AF_INET, (struct sockaddr *)&their_addr, ip, INET_ADDRSTRLEN);
        cl.sockno = their_sock;

        strcpy(IP,inet_ntoa(their_addr.sin_addr));
        PORT = (int) ntohs(their_addr.sin_port);

	clients[n] = their_sock;
        strcpy(ips[n],IP);
        ports[n] = PORT;
	n++;
	pthread_create(&recvt,NULL,recvmg,&cl);
	pthread_mutex_unlock(&mutex);
    }
    return 0;
}
