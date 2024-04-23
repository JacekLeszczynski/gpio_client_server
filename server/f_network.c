/* WĄTEK POŁĄCZENIA */
void *recvmg(void *sock)
{
    struct client_info cl = *((struct client_info *)sock);
    bool TerminateNow = 0, czysc = 0, b_test = 0;
    int len,i,j,a,id,tryb=0,adresat;
    char msg[CONST_MAX_BUFOR],*s,*s1,*s2,*s3,*ss,*log,*pom;
    pthread_t tester_gniazda;

    pthread_mutex_lock(&mutex);
    id = idsock(cl.sockno);
    tryb = tabs[id];
    pthread_mutex_unlock(&mutex);
    int *pieczec = NULL;

    while((len = recv(cl.sockno,&msg,CONST_MAX_BUFOR,0)) > 0)
    {
        if (len<=0) continue;

        /* CZYTANIE DANYCH */
        s = strdup(msg);
        s1 = GetLineToStr(s,1,'=',"");
        s2 = GetLineToStr(s,2,'=',"");
        s3 = GetLineToStr(s,3,'=',"");

        /* JEŚLI DOSTAJĘ EXIT - OPUSZCZAM PĘTLĘ I KOŃCZĘ WĄTEK */
        if (strcmp(s,"exit")==0) {
            TerminateNow = 1;
        } else

        #include "n_login.c"
        #include "n_gpio_monitor.c"
        #include "n_gpio.c"
        #include "n_laptop.c"

        /* STEROWANIE PILOTEM */
        if (strcmp(s1,"pilot")==0) {
            if (strcmp(s2,"active")==0) {
                pthread_mutex_lock(&mutex);
                if (pilot_adresat>-1)
                {
                    s = String("pilot=noactive");
                    sendtouser(s,-1,pilot_adresat,0);
                }
                pilot_adresat = cl.sockno;
                s = String("pilot=active");
                sendtouser(s,-1,pilot_adresat,0);
                pthread_mutex_unlock(&mutex);
            }
        }

        /* JEŚLI ZAŻĄDANO WYJŚCIA - WYCHODZĘ */
        if (TerminateNow)
        {
            sendtouser("server=terminated",-1,cl.sockno,0);
            break;
        };
    }  /* pętla główna recv i pętla wykonywania gotowych zapytań */

    if (AUTO_TIMER) {
        pthread_mutex_lock(&mutex_t);
        free(pieczec);
        pieczec = NULL;
        pthread_mutex_unlock(&mutex_t);
    }
    pthread_mutex_lock(&mutex);
    if (tryb==1 && AUTO_ON_OFF_BY_LOGIN) {
        /* automatycznie wyłącz GPIO w razie potrzeby */
        set_GPIO(GPIO_NR_1,0);
        sendmessage_status_gpio_all(cl.sockno,0);
    }
    if (gpio_adresat == cl.sockno) gpio_adresat = -1;
    if (pilot_adresat == cl.sockno)
    {
        pilot_adresat = -1;
        pilot_full = 0;
        for(i = 0; i < n; i++) {
            if (tabs[i]==2 && clients[i]!=cl.sockno)
            {
                pilot_adresat = clients[i];
                s = String("pilot=active");
                //sendmessage(s,pilot_adresat,0);
                sendtouser(s,-1,pilot_adresat,0);
                break;
            }
        }
    }
    for(i = 0; i < n; i++) {
        if(tabs[i] == 3 && shutdown_now[i] != -1) {
            // wysyłam informację o zamknięciu
            sendtouser("laptop=shutdown_ok",cl.sockno,shutdown_now[i],0);
            sleep(1);
        }
        if(tabs[i] == 3 && clients[i] == cl.sockno) {
            // wysyłam informację o zamknięciu
            sendtoall14("laptop=logout",cl.sockno,0);
        }
	if(clients[i] == cl.sockno) {
	    j = i;
	    while(j < n-1) {
		clients[j] = clients[j+1];
                strcpy(ips[j],ips[j+1]);
                ports[j] = ports[j+1];
                strcpy(keys[j],keys[j+1]);
                tabs[j] = tabs[j+1];
                shutdown_now[j] = shutdown_now[j+1];
		j++;
	    }
            break;
        }
    }
    n--;
    shutdown(cl.sockno,SHUT_RDWR);
    pthread_mutex_unlock(&mutex);
}

