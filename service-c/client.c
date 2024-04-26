#include "f_includes.c"
#include "f_defines.c"
#include "f_global.c"
#include "f_functions.c"
#include "komunikacja.c"
#include "f_network.c"
#include "f_daemon.c"

#define CONST_TEST 0

void test()
{
  char *s,*s1;
  int i,ii,len;

  s = String("Ala,Bela,Celina");

  ii = GetLineCount(s,',');
  printf("Zaczynam czytanie danych '%s' o ilości elementów = %i:\n",s1,ii);
  for (i=1;i<=ii;i++) {
      s1 = GetLineToStr(s,i,',',"");
      printf("%i: %s\n",i,s1);
  }
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
    char *HOST;
    int PORT;

    if (CONST_TEST) {
        test();
        return 0;
    }

    if(argc > 2)
    {
	printf("too many arguments");
	exit(1);
    }

    BUF = ConfToBufor("/etc/default/gpio.client");
    HOST = GetConfValue(BUF,"HOST","serwer");
    PORT = atoi(GetConfValue(BUF,"PORT","2122"));

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
        tabs[n] = 0;
        shutdown_now[n] = -1;
	n++;
	pthread_create(&recvt,NULL,recvmg,&cl);
	pthread_mutex_unlock(&mutex);
    }
    return 0;
}
