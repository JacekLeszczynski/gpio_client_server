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

int my_sock, is_port, dodatkowe_watki = 0;
int clients[CONST_MAX_CLIENTS];
char ips[CONST_MAX_CLIENTS][INET_ADDRSTRLEN];
int ports[CONST_MAX_CLIENTS];
char keys[CONST_MAX_CLIENTS][30]; //identyfikacja klientów
bool tabs[CONST_MAX_CLIENTS];  //tryb [0]=tryb (1-gpio, 2-pilot)
char *pbufor;

char *GPIO_NR,*PATH_KBD_0,*PATH_KBD_1;
bool REVERSE;

int n = 0, mn = 0, ischat = 0;
int error = 0;
int pilot_adresat = -1;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;

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

bool watek_dziala = 0;

/* WĄTEK WYSYŁAJĄCY ZAWARTOŚĆ BUFORA PILOTA */
void *sending_keys_pilot(void *arg)
{
    char *s;
    msleep(50);
    pthread_mutex_lock(&mutex2);
    s = String(pbufor);
    pbufor = String("");
/*
42,63,
29,a
56,125,25,
63,
29,42,31,
*/
    s = StringReplace(s,"4263","key_down");
    s = StringReplace(s,"29key_up","");
    s = StringReplace(s,"5612525","");
    s = StringReplace(s,"63","");
    s = StringReplace(s,"294231","");
    if (pilot_adresat == -1)
    {
        if (strcmp(s,"key_up")==0)
        {
            //pid_t pid;
            //pid = fork();
            //if (pid==0) execl("/usr/local/bin/speaktime","/usr/local/bin/speaktime",NULL);
            char *ss = "speaktime";
            system(ss);
        } else
        if (strcmp(s,"key_left")==0)
        {
            //pid_t pid;
            //pid = fork();
            //if (pid==0) execl("/usr/local/bin/speaktime","/usr/local/bin/speaktime",NULL);
            //char *ss = "amixer sset PCM 10%-";
            char *ss = "mpc volume -5";
            system(ss);
        } else
        if (strcmp(s,"key_right")==0)
        {
            //pid_t pid;
            //pid = fork();
            //if (pid==0) execl("/usr/local/bin/speaktime","/usr/local/bin/speaktime",NULL);
            //char *ss = "amixer sset PCM 10%+";
            char *ss = "mpc volume +5";
            system(ss);
        }
        //sendtoall(upcase(s),-1,1,1);
    } else {
        if (strcmp(trim(s),"")!=0)
        {
            s = concat("pilot=",s);
            sendtouser(s,-1,pilot_adresat,1);
        }
    }
    watek_dziala = 0;
    pthread_mutex_unlock(&mutex2);
}

/* WĄTEK CZYTAJĄCY NACIŚNIĘCIE KLAWISZY PILOTA */
void *procedure_w0(void *arg)
{
    pthread_t watek;
    const char *dev = PATH_KBD_0;
    int fd;
    char *klucz;
    fd = open(dev, O_RDONLY); // Open the buffer
    if (fd != -1) {
        struct input_event ev;
        ssize_t n;

        pthread_mutex_lock(&mutex);
        dodatkowe_watki++;
        pthread_mutex_unlock(&mutex);
        while (1) {
            n = read(fd, &ev, sizeof ev); // Read from the buffer
            if (ev.type == EV_KEY && ev.value == 1)
            {
                pthread_mutex_lock(&mutex2);
                if (ev.code==48)
                {
                    klucz = String("key_up");
                    pbufor = concat(pbufor,klucz);
                } else
                if (ev.code==104)
                {
                    klucz = String("key_left");
                    pbufor = concat(pbufor,klucz);
                } else
                if (ev.code==109)
                {
                    klucz = String("key_right");
                    pbufor = concat(pbufor,klucz);
                } else
                if (ev.code==1)
                {
                    klucz = String("key_down");
                    pbufor = concat(pbufor,klucz);
                } else
                {
                    klucz = itoa(ev.code,10);
                    pbufor = concat(pbufor,klucz);
                }
                if (watek_dziala == 0)
                {
                    watek_dziala = 1;
                    pthread_create(&watek,NULL,sending_keys_pilot,NULL);
                }
                pthread_mutex_unlock(&mutex2);
            }
        }
        pthread_mutex_lock(&mutex);
        dodatkowe_watki--;
        pthread_mutex_unlock(&mutex);
        close(fd);
        fflush(stdout);
    }
}

void *procedure_w1(void *arg)
{
    pthread_t watek;
    const char *dev = PATH_KBD_1;
    int fd;
    char *klucz;
    fd = open(dev, O_RDONLY); // Open the buffer
    if (fd != -1) {
        struct input_event ev;
        ssize_t n;

        pthread_mutex_lock(&mutex);
        dodatkowe_watki++;
        pthread_mutex_unlock(&mutex);
        while (1) {
            n = read(fd, &ev, sizeof ev); // Read from the buffer
            if (ev.type == EV_KEY && ev.value == 1)
            {
                pthread_mutex_lock(&mutex2);
                if (ev.code==11)
                {
                    klucz = String("key_power");
                    pbufor = concat(pbufor,klucz);
                } else
                if (ev.code==2)
                {
                    klucz = String("key_play_pause");
                    pbufor = concat(pbufor,klucz);
                } else
                if (ev.code==3)
                {
                    klucz = String("key_up");
                    pbufor = concat(pbufor,klucz);
                } else
                if (ev.code==4)
                {
                    klucz = String("key_left");
                    pbufor = concat(pbufor,klucz);
                } else
                if (ev.code==5)
                {
                    klucz = String("key_right");
                    pbufor = concat(pbufor,klucz);
                } else
                if (ev.code==6)
                {
                    klucz = String("key_down");
                    pbufor = concat(pbufor,klucz);
                } else
                if (ev.code==7)
                {
                    klucz = String("key_push");
                    pbufor = concat(pbufor,klucz);
                } else
                if (ev.code==8)
                {
                    klucz = String("key_back");
                    pbufor = concat(pbufor,klucz);
                } else
                if (ev.code==9)
                {
                    klucz = String("key_enter");
                    pbufor = concat(pbufor,klucz);
                } else
                if (ev.code==10)
                {
                    klucz = String("key_menu");
                    pbufor = concat(pbufor,klucz);
                } else
                if (ev.code==30)
                {
                    klucz = String("key_page_up");
                    pbufor = concat(pbufor,klucz);
                } else
                if (ev.code==48)
                {
                    klucz = String("key_page_down");
                    pbufor = concat(pbufor,klucz);
                } else
                if (ev.code==46)
                {
                    klucz = String("key_mic");
                    pbufor = concat(pbufor,klucz);
                } else
                if (ev.code==32)
                {
                    klucz = String("key_volume_up");
                    pbufor = concat(pbufor,klucz);
                } else
                if (ev.code==18)
                {
                    klucz = String("key_volume_down");
                    pbufor = concat(pbufor,klucz);
                } else
                if (ev.code==33)
                {
                    klucz = String("key_delete");
                    pbufor = concat(pbufor,klucz);
                } else
                if (ev.code==34)
                {
                    klucz = String("key_mute");
                    pbufor = concat(pbufor,klucz);
                } else
                {
                    klucz = itoa(ev.code,10);
                    pbufor = concat(pbufor,klucz);
                }
                if (watek_dziala == 0)
                {
                    watek_dziala = 1;
                    pthread_create(&watek,NULL,sending_keys_pilot,NULL);
                }
                pthread_mutex_unlock(&mutex2);
            }
        }
        pthread_mutex_lock(&mutex);
        dodatkowe_watki--;
        pthread_mutex_unlock(&mutex);
        close(fd);
        fflush(stdout);
    }
}

/* WĄTEK POŁĄCZENIA */
void *recvmg(void *sock)
{
    struct client_info cl = *((struct client_info *)sock);
    bool TerminateNow = 0, czysc = 0;
    int len,i,j,a,id;
    char msg[CONST_MAX_BUFOR],*s,*s1,*s2,*s3;

    id = idsock(cl.sockno);

    while((len = recv(cl.sockno,&msg,CONST_MAX_BUFOR,0)) > 0)
    {
        if (len<=0) continue;
        s = strdup(msg);
        s1 = GetLineToStr(s,1,'=',"");
        s2 = GetLineToStr(s,2,'=',"");

        //s3 = concat("Zmienna s1=",s1);
        //sendmessage(s3,cl.sockno,1);
        //s3 = concat("Zmienna s2=",s2);
        //sendmessage(s3,cl.sockno,1);

        if (strcmp(s1,"gpio")==0 && tabs[id]==1) {
            if (strcmp(s2,"on")==0) {
                export_GPIO();
                direction_GPIO();
                if (REVERSE) set_GPIO(0); else set_GPIO(1);
                unexport_GPIO();
                if (REVERSE) sendmessage("gpio=1",cl.sockno,1); else sendmessage("gpio=0",cl.sockno,1);
            } else
            if (strcmp(s2,"off")==0) {
                export_GPIO();
                direction_GPIO();
                if (REVERSE) set_GPIO(1); else set_GPIO(0);
                unexport_GPIO();
                if (REVERSE) sendmessage("gpio=0",cl.sockno,1); else sendmessage("gpio=1",cl.sockno,1);
            } else
            if (strcmp(s2,"status")==0) {
                export_GPIO();
                a = get_GPIO();
                unexport_GPIO();
                if (a==1) {
                    if (REVERSE) sendmessage("gpio=0",cl.sockno,1); else sendmessage("gpio=1",cl.sockno,1);
                } else {
                    if (REVERSE) sendmessage("gpio=1",cl.sockno,1); else sendmessage("gpio=0",cl.sockno,1);
                }
            }
        } else
        if (strcmp(s1,"tv")==0 && tabs[id]==1) {
            if (strcmp(s2,"on")==0) {
                system("systemctl start tvheadend");
                sendmessage("STATUS_TV_ON",cl.sockno,1);
            } else
            if (strcmp(s2,"off")==0) {
                system("systemctl stop tvheadend");
                sendmessage("tv=off",cl.sockno,1);
            }
        } else
        if (strcmp(s1,"tryb")==0) {
            if (strcmp(s2,"gpio")==0) {
                pthread_mutex_lock(&mutex);
                tabs[id] = 1;
                pthread_mutex_unlock(&mutex);
            } else
            if (strcmp(s2,"pilot")==0) {
                pthread_mutex_lock(&mutex);
                tabs[id] = 2;
                if (pilot_adresat==-1)
                {
                    pilot_adresat = cl.sockno;
                    s = String("pilot=active");
                    sendtouser(s,-1,pilot_adresat,0);
                } else {
                    s = String("tryb=pilot");
                    sendtouser(s,-1,cl.sockno,0);
                }
                pthread_mutex_unlock(&mutex);
            }
        } else
        if (strcmp(s1,"pilot")==0) {
            if (strcmp(s2,"active")==0) {
                pthread_mutex_lock(&mutex);
                if (pilot_adresat>-1)
                {
                    s = String("pilot=noactive");
                    sendtouser(s,-1,pilot_adresat,0);
                }
                pilot_adresat = cl.sockno;
                s = String("pilot=active");
                sendtouser(s,-1,pilot_adresat,0);
                pthread_mutex_unlock(&mutex);
            }
        }

        //sendmessage(s,cl.sockno,1);

        if (TerminateNow) break;
    }  /* pętla główna recv i pętla wykonywania gotowych zapytań */

    pthread_mutex_lock(&mutex);
    if (pilot_adresat == cl.sockno)
    {
        pilot_adresat = -1;
        for(i = 0; i < n; i++) {
            if (tabs[i]==2 && clients[i]!=cl.sockno)
            {
                pilot_adresat = clients[i];
                s = String("pilot=active");
                //sendmessage(s,pilot_adresat,0);
                sendtouser(s,-1,pilot_adresat,0);
                break;
            }
        }
    }
    for(i = 0; i < n; i++) {
	if(clients[i] == cl.sockno)
        {
	    j = i;
	    while(j < n-1)
            {
		clients[j] = clients[j+1];
                strcpy(ips[j],ips[j+1]);
                ports[j] = ports[j+1];
                strcpy(keys[j],keys[j+1]);
                tabs[j] = tabs[j+1];
		j++;
	    }
            break;
        }
    }
    n--;
    shutdown(cl.sockno,SHUT_RDWR);
    pthread_mutex_unlock(&mutex);
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
    pthread_t sendt,recvt,watek0,watek1;
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
    bool SCAN_KEYBOARD_0 = SCAN_KEYBOARD;
    bool SCAN_KEYBOARD_1 = SCAN_KEYBOARD;
    if (SCAN_KEYBOARD){
        PATH_KBD_0 = GetConfValue(BUF,"PATH_DEV_KEYBOARD_0","");
        PATH_KBD_1 = GetConfValue(BUF,"PATH_DEV_KEYBOARD_1","");
        if (strcmp(PATH_KBD_0,"")==0) SCAN_KEYBOARD_0 = 0;
        if (strcmp(PATH_KBD_1,"")==0) SCAN_KEYBOARD_1 = 0;
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

    pbufor = String("");

    if (SCAN_KEYBOARD_0) {
        pthread_create(&watek0,NULL,procedure_w0,NULL);
    }
    if (SCAN_KEYBOARD_1) {
        pthread_create(&watek1,NULL,procedure_w1,NULL);
    }
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
        strcpy(keys[n],"");
        tabs[n] = 1; //domyslnie GPIO!
	n++;
	pthread_create(&recvt,NULL,recvmg,&cl);
	pthread_mutex_unlock(&mutex);
    }
    return 0;
}
