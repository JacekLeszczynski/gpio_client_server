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
int tabs[CONST_MAX_CLIENTS];  //tryb [0]=tryb (1-gpio, 2-pilot, 3-laptop)
int shutdown_now[CONST_MAX_CLIENTS];  //do zamknięcia!
char *pbufor;

char *GPIO_NR_1,*GPIO_NR_2,*GPIO_NR_3,*GPIO_NR_4,*GPIO_NR_5,*GPIO_NR_6,*GPIO_NR_7,*GPIO_NR_8;
char *PATH_KBD_0,*PATH_KBD_1;
bool REVERSE,AUTO_ON_OFF_BY_LOGIN,AUTO_TIMER = 0;
char *LAPTOP_MAC_ADDRESS;
double LATITUDE,LONGITUDE;

int n = 0, mn = 0, ischat = 0;
int error = 0;
int pilot_adresat = -1;
int gpio_adresat = -1;
int pilot_full = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_t = PTHREAD_MUTEX_INITIALIZER;

bool watek_dziala = 0;
