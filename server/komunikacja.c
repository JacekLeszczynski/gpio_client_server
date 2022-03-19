/* PROCEDURY WYSYŁAJĄCE */
void sendmessage(char *msg, int sock_adresat, bool aMutex) //adresat to soket!
{
    int i,a,lx,lx2,l1,ll;
    char *tmp,*ss,*x,*hex;
    unsigned char *x1,*x2,*x3,*x4,*x5;

    if (aMutex) pthread_mutex_lock(&mutex);
    send(sock_adresat,msg,strlen(msg),MSG_NOSIGNAL);
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

