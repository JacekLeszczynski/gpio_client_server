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

