#define CONST_TEST 0

#include "f_includes.c"
#include "f_defines.c"
#include "f_global.c"
#include "f_functions.c"
#include "komunikacja.c"
#include "f_pilot.c"
#include "f_network.c"
#include "f_daemon.c"

void test()
{
  char *s,dzien_roboczy,dzien,*tmp,t1[6],t2[6],date[11],time[6];
  char *s2;

  DbConnect();
  dzien_roboczy = DzienRoboczyToChar();
  //50.0614300,19.9365800
  tmp = GetHoursSunDay(50.0614300,19.9365800);
  s2 = GetCalendarAll();
  DbDisconnect();

  split_times(tmp,t1,t2);
  extract_date_and_time(LocalTime(),date,time);
  printf("Dziś jest = %s\n",date);
  printf("Jest godzina = %s\n",time);
  printf("Wschód słońca = %s\n",t1);
  printf("Zachód słońca = %s\n",t2);
  printf("Dzień roboczy = %c\n",dzien_roboczy);
  if (strcmp(t1,time)<=0 && strcmp(time,t2)<=0) {
    dzien = '1';
    printf("Jest dzień.\n");
  } else {
    dzien = '0';
    printf("Jest noc.\n");
  }

  s = String(date);
  s = concat_str_char(s,',');
  s = concat(s,time);
  s = concat_str_char(s,',');
  s = concat(s,t1);
  s = concat_str_char(s,',');
  s = concat(s,t2);
  s = concat_str_char(s,',');
  s = concat_str_char(s,dzien_roboczy);
  s = concat_str_char(s,',');
  s = concat_str_char(s,dzien);
  printf("s1 = %s\n",s);
  printf("s2 = %s\n",s2);
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

    if (CONST_TEST) {
        test();
        return 0;
    }

    if(argc > 2)
    {
	printf("too many arguments");
	exit(1);
    }

    BUF = ConfToBufor("/etc/default/gpio.server");
    portno = atoi(GetConfValue(BUF,"PORT","2122"));
    GPIO_NR_1 = GetConfValue(BUF,"GPIO_NUMBER_1","23");
    GPIO_NR_2 = GetConfValue(BUF,"GPIO_NUMBER_2","24");
    GPIO_NR_3 = GetConfValue(BUF,"GPIO_NUMBER_3","25");
    GPIO_NR_4 = GetConfValue(BUF,"GPIO_NUMBER_4","26");
    GPIO_NR_5 = GetConfValue(BUF,"GPIO_NUMBER_5","27");
    GPIO_NR_6 = GetConfValue(BUF,"GPIO_NUMBER_6","17");
    GPIO_NR_7 = GetConfValue(BUF,"GPIO_NUMBER_7","22");
    GPIO_NR_8 = GetConfValue(BUF,"GPIO_NUMBER_8","16");
    REVERSE = atoi(GetConfValue(BUF,"REVERSE","0"));
    AUTO_ON_OFF_BY_LOGIN = atoi(GetConfValue(BUF,"AUTO_ON_OFF","0"));
    AUTO_TIMER = atoi(GetConfValue(BUF,"TIMER","0"));
    bool SCAN_KEYBOARD = atoi(GetConfValue(BUF,"SCAN_KEYBOARD","0"));
    bool SCAN_KEYBOARD_0 = SCAN_KEYBOARD;
    bool SCAN_KEYBOARD_1 = SCAN_KEYBOARD;
    if (SCAN_KEYBOARD){
        PATH_KBD_0 = GetConfValue(BUF,"PATH_DEV_KEYBOARD_0","");
        PATH_KBD_1 = GetConfValue(BUF,"PATH_DEV_KEYBOARD_1","");
        if (strcmp(PATH_KBD_0,"")==0) SCAN_KEYBOARD_0 = 0;
        if (strcmp(PATH_KBD_1,"")==0) SCAN_KEYBOARD_1 = 0;
    }
    LAPTOP_MAC_ADDRESS = GetConfValue(BUF,"LAPTOP_MAC_ADDRESS","");
    LATITUDE = atof(GetConfValue(BUF,"LATITUDE","0.0"));
    LONGITUDE = atof(GetConfValue(BUF,"LONGITUDE","0.0"));
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
        tabs[n] = 0;
        shutdown_now[n] = -1;
	n++;
	pthread_create(&recvt,NULL,recvmg,&cl);
	pthread_mutex_unlock(&mutex);
    }
    return 0;
}
