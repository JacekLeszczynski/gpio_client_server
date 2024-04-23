/*
    TRYBY:
    1 - CLIENT GPIO
    2 - OBSŁUGA PILOTA
    3 - LAPTOP
    4 - MONITOR GPIO
*/

/* OBSŁUGA LOGOWANIA DO SIECI - NOWE POŁĄCZENIA */
if (strcmp(s1,"tryb")==0) {

    if (strcmp(s2,"gpio")==0) {
        pthread_mutex_lock(&mutex);
        id = idsock(cl.sockno);
        tryb = 1;
        tabs[id] = tryb;
        if (gpio_adresat==-1) gpio_adresat = cl.sockno;
        pthread_mutex_unlock(&mutex);
        if (AUTO_ON_OFF_BY_LOGIN) {
            /* automatyczne odpalenie GPIO */
            set_GPIO(GPIO_NR_1,1);
            pthread_mutex_lock(&mutex);
            sendmessage("gpio=1",cl.sockno,0);
            sendmessage_status_gpio_all(cl.sockno,0);
            pthread_mutex_unlock(&mutex);
        }
    } else

    if (strcmp(s2,"pilot")==0) {
        pthread_mutex_lock(&mutex);
        id = idsock(cl.sockno);
        tryb = 2;
        tabs[id] = tryb;
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
    } else

    if (strcmp(s2,"laptop")==0) {
        pthread_mutex_lock(&mutex);
        id = idsock(cl.sockno);
        tryb = 3;
        tabs[id] = tryb;
        s = String("laptop=active");
        sendtouser(s,-1,cl.sockno,0);
        sendtoall14("laptop=login",cl.sockno,0);
        pthread_mutex_unlock(&mutex);
    } else

    if (strcmp(s2,"pilot_full_0")==0) {
        pilot_full = 0;
    } else

    if (strcmp(s2,"pilot_full_1")==0) {
        pilot_full = 1;
    } else

    if (strcmp(s2,"gpio_monitor")==0) {
        pthread_mutex_lock(&mutex);
        id = idsock(cl.sockno);
        tryb = 4;
        tabs[id] = tryb;
        s = String("monitor_gpio=active");
        sendtouser(s,-1,cl.sockno,0);
        pthread_mutex_unlock(&mutex);
    }

} else

