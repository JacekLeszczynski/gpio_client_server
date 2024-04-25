/* PROCEDURY WYSYŁAJĄCE */
/* wysłanie wiadomości do wybranego użytkownika bez sprawdzania czy istnieje */
void sendmessage(char *msg, int sock_adresat, bool aMutex) //adresat to soket!
{
    if (aMutex) pthread_mutex_lock(&mutex);
    send(sock_adresat,msg,strlen(msg)+1,MSG_NOSIGNAL);
    if (aMutex) pthread_mutex_unlock(&mutex);
}

/* wysłanie wiadomości do innego użytkownika ale tylko wtedy gdy istnieje */
void sendtouser(char *msg, int sock_nadawca, int sock_adresat, bool aMutex) //nadawca i adresat to sokety!
{
    int i,a;

    a = -1;
    if (aMutex) pthread_mutex_lock(&mutex);
    for(i = 0; i < n; i++)
    {
        if(clients[i] == sock_adresat)
        {
            a = i;
            break;
	}
    }
    if (a>-1) send(sock_adresat,msg,strlen(msg)+1,MSG_NOSIGNAL);
    if (aMutex) pthread_mutex_unlock(&mutex);
}

/* wysłanie wiadomości do wszystkich użytkowników, jeśli force_all to także do siebie samego */
void sendtoall(char *msg, int sock_nadawca, bool force_all, bool aMutex) //nadawca to soket
{
    int i;

    if (aMutex) pthread_mutex_lock(&mutex);
    for(i = 0; i < n; i++)
    {
        if (force_all || (clients[i] != sock_nadawca)) send(clients[i],msg,strlen(msg)+1,MSG_NOSIGNAL);
    }
    if (aMutex) pthread_mutex_unlock(&mutex);
}

/* wysłanie wiadomości do wszystkich użytkowników należących do trybu = 1 */
void sendtoall1(char *msg, int sock_nadawca, bool aMutex) //nadawca to soket
{
    int i;

    if (aMutex) pthread_mutex_lock(&mutex);
    for(i = 0; i < n; i++)
    {
        if (clients[i] != sock_nadawca && tabs[i] == 1) send(clients[i],msg,strlen(msg)+1,MSG_NOSIGNAL);
    }
    if (aMutex) pthread_mutex_unlock(&mutex);
}

/* wysłanie wiadomości do wszystkich użytkowników należących do trybu = 1 i 4 */
void sendtoall14(char *msg, int sock_nadawca, bool aMutex) //nadawca to soket
{
    int i;

    if (aMutex) pthread_mutex_lock(&mutex);
    for(i = 0; i < n; i++)
    {
        if (clients[i] != sock_nadawca && (tabs[i] == 1 || tabs[i] == 4)) send(clients[i],msg,strlen(msg)+1,MSG_NOSIGNAL);
    }
    if (aMutex) pthread_mutex_unlock(&mutex);
}

void set_VALUE(char *line,int liczba) {
    char *filename = concat("/var/run/gpio.server.value.",line);
    FILE* plik = fopen(filename, "w");
    if (plik == NULL) return;
    fprintf(plik, "%d", liczba);
    fclose(plik);
}

int get_VALUE(char *line) {
    char *filename = concat("/var/run/gpio.server.value.",line);
    FILE* plik = fopen(filename, "r");
    if (plik == NULL) return 0;
    int liczba;
    fscanf(plik, "%d", &liczba);
    fclose(plik);
    return liczba;
}

int set_GPIO(char *line, int value)
{
    char *s;
    if ((REVERSE && value==0) || (!REVERSE && value==1)) {
        s = String("gpioset --bias=disable gpiochip0 ");
        s = concat(s,line);
        s = concat(s,"=1");
    } else {
        s = String("gpioset --bias=disable gpiochip0 ");
        s = concat(s,line);
        s = concat(s,"=0");
    }
    system(s);
    set_VALUE(line,value);
    return value;
}

int get_GPIO(char *line)
{
    return get_VALUE(line);
}

int IsReverse(int a)
{
    if (REVERSE) {
        if (a==1 ) {
            return 0;
        } else {
            return 1;
        }
    } else {
        return a;
    }
}

bool GetLaptopLogin()
{
    bool b_test = 0;
    for(int i = 0; i < n; i++) {
        if (tabs[i]==3)
        {
            b_test = 1;
            break;
        }
    }
    return b_test;
}

/* wysłanie statusu gpio do zalogowanego użytkownika */
void sendmessage_status_gpio(int sock_adresat, bool aMutex)
{
    int a1,a2,a3,a4,a5,a6,a7,a8,a9;
    char *s;
    a1 = get_GPIO(GPIO_NR_1);
    a2 = get_GPIO(GPIO_NR_2);
    a3 = get_GPIO(GPIO_NR_3);
    a4 = get_GPIO(GPIO_NR_4);
    a5 = get_GPIO(GPIO_NR_5);
    a6 = get_GPIO(GPIO_NR_6);
    a7 = get_GPIO(GPIO_NR_7);
    a8 = get_GPIO(GPIO_NR_8);
    a9 = GetLaptopLogin();
    s = String("gpio_status=");
    s = concat(s,itoa(a1,10));
    s = concat(s,itoa(a2,10));
    s = concat(s,itoa(a3,10));
    s = concat(s,itoa(a4,10));
    s = concat(s,itoa(a5,10));
    s = concat(s,itoa(a6,10));
    s = concat(s,itoa(a7,10));
    s = concat(s,itoa(a8,10));
    s = concat(s,itoa(a9,10));

    if (aMutex) pthread_mutex_lock(&mutex);
    send(sock_adresat,s,strlen(s),MSG_NOSIGNAL);
    if (aMutex) pthread_mutex_unlock(&mutex);
}

/* wysłanie statusu gpio do wszystkich zalogowanych soketów mających ustawiony tryb==4 ale nie do siebie */
void sendmessage_status_gpio_all(int sock_nadawca, bool aMutex)
{
    int i,a1,a2,a3,a4,a5,a6,a7,a8,a9;
    char *s;
    a1 = get_GPIO(GPIO_NR_1);
    a2 = get_GPIO(GPIO_NR_2);
    a3 = get_GPIO(GPIO_NR_3);
    a4 = get_GPIO(GPIO_NR_4);
    a5 = get_GPIO(GPIO_NR_5);
    a6 = get_GPIO(GPIO_NR_6);
    a7 = get_GPIO(GPIO_NR_7);
    a8 = get_GPIO(GPIO_NR_8);
    a9 = GetLaptopLogin();
    s = String("gpio_status=");
    s = concat(s,itoa(a1,10));
    s = concat(s,itoa(a2,10));
    s = concat(s,itoa(a3,10));
    s = concat(s,itoa(a4,10));
    s = concat(s,itoa(a5,10));
    s = concat(s,itoa(a6,10));
    s = concat(s,itoa(a7,10));
    s = concat(s,itoa(a8,10));
    s = concat(s,itoa(a9,10));

    if (aMutex) pthread_mutex_lock(&mutex);
    for(i = 0; i < n; i++)
    {
        if ((tabs[i]==1 || tabs[i]==4) && clients[i]!=sock_nadawca) send(clients[i],s,strlen(s),MSG_NOSIGNAL);
    }
    if (aMutex) pthread_mutex_unlock(&mutex);
}

/* WĄTEK WYSYŁAJĄCY ZAWARTOŚĆ BUFORA PILOTA */
void *sending_keys_pilot(void *arg)
{
    char *s,*msg;
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
    s = StringReplace(s,"4263","key2_down");
    s = StringReplace(s,"29key2_up","");
    s = StringReplace(s,"5612525","");
    s = StringReplace(s,"63","");
    s = StringReplace(s,"294231","");
    if (pilot_adresat == -1)
    {        //gpio_adresat
        if (strcmp(s,"key_power")==0)
        {
            if (gpio_adresat>-1)
            {
                msg = String("pilot=key_power");
                sendtouser(msg,-1,gpio_adresat,1);
            } else {
                int local_a = get_GPIO(GPIO_NR_2);
                if (local_a == 0) {
                    local_a = 1;
                } else {
                    local_a = 0;
                }
                set_GPIO(GPIO_NR_2,local_a);
                sendmessage_status_gpio_all(-1,1);
            }
        } else
        if (strcmp(s,"key_push")==0)
        {
            char *ss = "speaktime";
            system(ss);
        } else
        if (strcmp(s,"key_volume_down")==0)
        {
            //pid_t pid;
            //pid = fork();
            //if (pid==0) execl("/usr/local/bin/speaktime","/usr/local/bin/speaktime",NULL);
            //char *ss = "amixer sset PCM 10%-";
            if (gpio_adresat>-1)
            {
                msg = String("pilot=key_volume_down");
                sendtouser(msg,-1,gpio_adresat,1);
            } else {
                char *ss = "mpc volume -5";
                system(ss);
            }
        } else
        if (strcmp(s,"key_volume_up")==0)
        {
            //pid_t pid;
            //pid = fork();
            //if (pid==0) execl("/usr/local/bin/speaktime","/usr/local/bin/speaktime",NULL);
            //char *ss = "amixer sset PCM 10%+";
            if (gpio_adresat>-1)
            {
                msg = String("pilot=key_volume_up");
                sendtouser(msg,-1,gpio_adresat,1);
            } else {
                char *ss = "mpc volume +5";
                system(ss);
            }
        } else
        if (strcmp(s,"key_play_pause")==0)
        {
            if (gpio_adresat>-1)
            {
                msg = String("pilot=key_play_pause");
                sendtouser(msg,-1,gpio_adresat,1);
            } else {
                char *ss = "mpc play";
                system(ss);
            }
        } else
        if (strcmp(s,"key_back")==0)
        {
            if (gpio_adresat>-1)
            {
                msg = String("pilot=key_back");
                sendtouser(msg,-1,gpio_adresat,1);
            } else {
                char *ss = "mpc stop";
                system(ss);
            }
        } else
        if (strcmp(s,"key_right")==0)
        {
            if (gpio_adresat>-1)
            {
                msg = String("pilot=key_right");
                sendtouser(msg,-1,gpio_adresat,1);
            } else {
                char *ss = "mpc next";
                system(ss);
            }
        } else
        if (strcmp(s,"key_left")==0)
        {
            if (gpio_adresat>-1)
            {
                msg = String("pilot=key_left");
                sendtouser(msg,-1,gpio_adresat,1);
            } else {
                char *ss = "mpc prev";
                system(ss);
            }
        } else
        {
            s = concat("pilot=",s);
            sendtouser(s,-1,gpio_adresat,1);
        }
    } else {
        if (pilot_full == 0 && gpio_adresat!=-1 && strcmp(s,"key2_down")==0)
        {
            s = String("pilot=key2_down");
            sendtouser(s,-1,gpio_adresat,1);
        } else
        if (strcmp(trim(s),"")!=0)
        {
            s = concat("pilot=",s);
            sendtouser(s,-1,pilot_adresat,1);
        }
    }
    watek_dziala = 0;
    pthread_mutex_unlock(&mutex2);
}

