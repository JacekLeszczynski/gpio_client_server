/* USTAWIANIE GPIO I CZYTANIE GPIO */
if (strcmp(s1,"gpio")==0 && (tryb==0 || tryb==1)) {
    if (strcmp(s2,"on")==0) {
        pthread_mutex_lock(&mutex);
        set_GPIO(GPIO_NR_1,1);
        sendmessage("gpio=1",cl.sockno,0);
        sendmessage_status_gpio_all(cl.sockno,0);
        pthread_mutex_unlock(&mutex);
    } else
    if (strcmp(s2,"off")==0) {
        pthread_mutex_lock(&mutex);
        set_GPIO(GPIO_NR_1,0);
        sendmessage("gpio=0",cl.sockno,0);
        sendmessage_status_gpio_all(cl.sockno,0);
        pthread_mutex_unlock(&mutex);
    } else
    if (strcmp(s2,"status")==0) {
        a = get_GPIO(GPIO_NR_1);
        sendmessage(concat("gpio=",itoa(a,10)),cl.sockno,1);
    }
    if (tryb==0) TerminateNow = 1;
} else

if (tryb==0 || tryb==1 || tryb==2) {

    /* USTAWIANIE GPIO_2 I CZYTANIE GPIO_2 */
    if (strcmp(s1,"gpio2")==0) {
        if (strcmp(s2,"on")==0) {
            pthread_mutex_lock(&mutex);
            set_GPIO(GPIO_NR_2,1);
            sendmessage("gpio2=1",cl.sockno,0);
            sendmessage_status_gpio_all(cl.sockno,0);
            pthread_mutex_unlock(&mutex);
        } else
        if (strcmp(s2,"off")==0) {
            pthread_mutex_lock(&mutex);
            set_GPIO(GPIO_NR_2,0);
            sendmessage("gpio2=0",cl.sockno,0);
            sendmessage_status_gpio_all(cl.sockno,0);
            pthread_mutex_unlock(&mutex);
        } else
        if (strcmp(s2,"status")==0) {
            a = get_GPIO(GPIO_NR_2);
            sendmessage(concat("gpio2=",itoa(a,10)),cl.sockno,1);
        }
    } else

    if (strcmp(s1,"gpio3")==0) {
        if (strcmp(s2,"on")==0) {
            pthread_mutex_lock(&mutex);
            set_GPIO(GPIO_NR_3,1);
            sendmessage("gpio3=1",cl.sockno,0);
            sendmessage_status_gpio_all(cl.sockno,0);
            pthread_mutex_unlock(&mutex);
        } else
        if (strcmp(s2,"off")==0) {
            pthread_mutex_lock(&mutex);
            set_GPIO(GPIO_NR_3,0);
            sendmessage("gpio3=0",cl.sockno,0);
            sendmessage_status_gpio_all(cl.sockno,0);
            pthread_mutex_unlock(&mutex);
        } else
        if (strcmp(s2,"status")==0) {
            a = get_GPIO(GPIO_NR_3);
            sendmessage(concat("gpio3=",itoa(a,10)),cl.sockno,1);
        }
    } else

    if (strcmp(s1,"gpio4")==0) {
        if (strcmp(s2,"on")==0) {
            pthread_mutex_lock(&mutex);
            set_GPIO(GPIO_NR_4,1);
            sendmessage("gpio4=1",cl.sockno,0);
            sendmessage_status_gpio_all(cl.sockno,0);
            pthread_mutex_unlock(&mutex);
        } else
        if (strcmp(s2,"off")==0) {
            pthread_mutex_lock(&mutex);
            set_GPIO(GPIO_NR_4,0);
            sendmessage("gpio4=0",cl.sockno,0);
            sendmessage_status_gpio_all(cl.sockno,0);
            pthread_mutex_unlock(&mutex);
        } else
        if (strcmp(s2,"status")==0) {
            a = get_GPIO(GPIO_NR_4);
            sendmessage(concat("gpio4=",itoa(a,10)),cl.sockno,1);
        }
    } else

    if (strcmp(s1,"gpio5")==0) {
        if (strcmp(s2,"on")==0) {
            pthread_mutex_lock(&mutex);
            set_GPIO(GPIO_NR_5,1);
            sendmessage("gpio5=1",cl.sockno,0);
            sendmessage_status_gpio_all(cl.sockno,0);
            pthread_mutex_unlock(&mutex);
        } else
        if (strcmp(s2,"off")==0) {
            pthread_mutex_lock(&mutex);
            set_GPIO(GPIO_NR_5,0);
            sendmessage("gpio5=0",cl.sockno,0);
            sendmessage_status_gpio_all(cl.sockno,0);
            pthread_mutex_unlock(&mutex);
        } else
        if (strcmp(s2,"status")==0) {
            a = get_GPIO(GPIO_NR_5);
            sendmessage(concat("gpio5=",itoa(a,10)),cl.sockno,1);
        }
    } else

    if (strcmp(s1,"gpio6")==0) {
        if (strcmp(s2,"on")==0) {
            pthread_mutex_lock(&mutex);
            set_GPIO(GPIO_NR_6,1);
            sendmessage("gpio6=1",cl.sockno,0);
            sendmessage_status_gpio_all(cl.sockno,0);
            pthread_mutex_unlock(&mutex);
        } else
        if (strcmp(s2,"off")==0) {
            pthread_mutex_lock(&mutex);
            set_GPIO(GPIO_NR_6,0);
            sendmessage("gpio6=0",cl.sockno,0);
            sendmessage_status_gpio_all(cl.sockno,0);
            pthread_mutex_unlock(&mutex);
        } else
        if (strcmp(s2,"status")==0) {
            a = get_GPIO(GPIO_NR_6);
            sendmessage(concat("gpio6=",itoa(a,10)),cl.sockno,1);
        }
    } else

    if (strcmp(s1,"gpio7")==0) {
        if (strcmp(s2,"on")==0) {
            pthread_mutex_lock(&mutex);
            set_GPIO(GPIO_NR_7,1);
            sendmessage("gpio7=1",cl.sockno,0);
            sendmessage_status_gpio_all(cl.sockno,0);
            pthread_mutex_unlock(&mutex);
        } else
        if (strcmp(s2,"off")==0) {
            pthread_mutex_lock(&mutex);
            set_GPIO(GPIO_NR_7,0);
            sendmessage("gpio7=0",cl.sockno,0);
            sendmessage_status_gpio_all(cl.sockno,0);
            pthread_mutex_unlock(&mutex);
        } else
        if (strcmp(s2,"status")==0) {
            a = get_GPIO(GPIO_NR_7);
            sendmessage(concat("gpio7=",itoa(a,10)),cl.sockno,1);
        }
    } else

    if (strcmp(s1,"gpio8")==0) {
        if (strcmp(s2,"on")==0) {
            pthread_mutex_lock(&mutex);
            set_GPIO(GPIO_NR_8,1);
            sendmessage("gpio8=1",cl.sockno,0);
            sendmessage_status_gpio_all(cl.sockno,0);
            pthread_mutex_unlock(&mutex);
        } else
        if (strcmp(s2,"off")==0) {
            pthread_mutex_lock(&mutex);
            set_GPIO(GPIO_NR_8,0);
            sendmessage("gpio8=0",cl.sockno,0);
            sendmessage_status_gpio_all(cl.sockno,0);
            pthread_mutex_unlock(&mutex);
        } else
        if (strcmp(s2,"status")==0) {
            a = get_GPIO(GPIO_NR_8);
            sendmessage(concat("gpio8=",itoa(a,10)),cl.sockno,1);
        }
    }
    if (tryb==0) TerminateNow = 1;

} else

