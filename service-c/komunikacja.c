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

void SetVolumeUp(){
    char *s;
    s = String("amixer sset Master Playback 5%+");
    //s = String("wpctl set-volume @DEFAULT_AUDIO_SINK@ 5%+");
    system(s);
}

void SetVolumeDown(){
    char *s;
    s = String("amixer sset Master Playback 5%-");
    //s = String("wpctl set-volume @DEFAULT_AUDIO_SINK@ 5%-");
    system(s);
}

void ShutdownNow(){
    char *s;
    s = String("shutdown -P now");
    system(s);
}

