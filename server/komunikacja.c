/* PROCEDURY WYSYŁAJĄCE */
/* wysłanie wiadomości do wybranego użytkownika bez sprawdzania czy istnieje */
void sendmessage(char *msg, int sock_adresat, bool aMutex) //adresat to soket!
{
    int i,a,lx,lx2,l1,ll;
    char *tmp,*ss,*x,*hex;
    unsigned char *x1,*x2,*x3,*x4,*x5;

    if (aMutex) pthread_mutex_lock(&mutex);
    send(sock_adresat,msg,strlen(msg),MSG_NOSIGNAL);
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
    if (a>-1) send(sock_adresat,msg,strlen(msg),MSG_NOSIGNAL);
    if (aMutex) pthread_mutex_unlock(&mutex);
}

/* wysłanie wiadomości do wszystkich użytkowników, jeśli force_all to także do siebie samego */
void sendtoall(char *msg, int sock_nadawca, bool force_all, bool aMutex) //nadawca to soket
{
    int i;

    if (aMutex) pthread_mutex_lock(&mutex);
    for(i = 0; i < n; i++)
    {
        if (force_all || (clients[i] != sock_nadawca)) send(clients[i],msg,strlen(msg),MSG_NOSIGNAL);
    }
    if (aMutex) pthread_mutex_unlock(&mutex);
}

int set_GPIO(int value)
{
    char *ss;
    ss = String("gpioset gpiochip0 ");
    ss = concat(ss,GPIO_NR);
    ss = concat(ss,"=");
    ss = concat(ss,itoa(value,10));
    system(ss);
    GPIO_VALUE = value;
}

int get_GPIO()
{
    return GPIO_VALUE;

    FILE *f;
    char *ss,w[100];
    int a;
    ss = String("gpioget gpiochip0 ");
    ss = concat(ss,GPIO_NR);
    f = popen(ss,"r");
    if (f == NULL) return -1;
    fgets(w,sizeof(w),f);
    a = atoi(w);
    return a;
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
    s = StringReplace(s,"4263","key_down");
    s = StringReplace(s,"29key_up","");
    s = StringReplace(s,"5612525","");
    s = StringReplace(s,"63","");
    s = StringReplace(s,"294231","");
    if (pilot_adresat == -1)
    {        //gpio_adresat
        if (strcmp(s,"key_power")==0 && gpio_adresat>-1)
        {
            msg = String("pilot=key_power");
            sendtouser(msg,-1,gpio_adresat,1);
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
        }
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

