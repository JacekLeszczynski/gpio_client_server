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

int export_GPIO()
{
    FILE *fd;
    fd = fopen("/sys/class/gpio/export", "w");
    if (!fd)
    {
        fprintf (stderr, "Blad otwarcia pliku\n");
        return -1;
    } else {
        fprintf (fd,"%s",GPIO_NR);
    }
    fclose (fd);
    return 0;
}

int unexport_GPIO()
{
    FILE *fd;
    fd = fopen("/sys/class/gpio/unexport", "w");
    if (!fd)
    {
        fprintf (stderr, "Blad otwarcia pliku\n");
        return -1;
    } else {
        fprintf (fd,"%s",GPIO_NR);
    }
    fclose (fd);
    return 0;
}

int direction_GPIO()
{
    FILE *fd;
    char path [50] ="";
    strcat(path, "/sys/class/gpio/gpio");
    strcat(path, GPIO_NR);
    strcat(path, "/direction");
    fd = fopen(path, "w");
    if (!fd)
    {
        fprintf (stderr, "Blad otwarcia pliku\n");
        return -1;
    } else {
        fprintf (fd,"%s","out");
    }
    fclose (fd);
    return 0;
}

int direction_GPIO_OR()
{
    FILE *fd;
    char path [50] ="";
    strcat(path, "/sys/class/gpio/gpio");
    strcat(path, GPIO_NR);
    strcat(path, "/direction");
    fd = fopen(path, "w");
    if (!fd)
    {
        fprintf (stderr, "Blad otwarcia pliku\n");
        return -1;
    } else {
        fprintf (fd,"%s","in");
    }
    fclose (fd);
    return 0;
}

int set_GPIO(int value)
{
    FILE *fd;
    char path [50] ="";
    strcat(path, "/sys/class/gpio/gpio");
    strcat(path, GPIO_NR);
    strcat(path, "/value");
    fd = fopen(path, "w");
    if (!fd)
    {
        fprintf (stderr, "Blad otwarcia pliku\n");
        return -1;
    } else {
        fprintf (fd,"%d",value);
    }
    fclose (fd);
    return 0;
}

int get_GPIO()
{
    FILE *fd;
    char path [50] ="";
    char bufor [4];
    int temp;
    strcat(path, "/sys/class/gpio/gpio");
    strcat(path, GPIO_NR);
    strcat(path, "/value");
    fd = fopen(path, "r");
    if (!fd)
    {
        fprintf (stderr, "Blad otwarcia pliku\n");
        return -1;
    } else {
        if (!fgets(bufor,3,fd))
        {
            fprintf (stderr, "Blad odczytu danych\n");
            return -1;
        } else {
            temp = atoi(bufor);
        }
    }
    fclose (fd);
    return temp;
}

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

