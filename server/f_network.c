void *procedure_test(void *arg)
{
    int *pieczec, wyjscie = 0;
    pieczec = arg;
    while(1)
    {
        pthread_mutex_lock(&mutex_t);
        if (pieczec == NULL)
        {
            wyjscie = 1;
        } else {
            if (timer_count<60)
            {
                timer_count++;
            } else {
                if (REVERSE) set_GPIO(0); else set_GPIO(1);
            }
        }
        pthread_mutex_unlock(&mutex_t);
        if (wyjscie) break;
        sleep(5);
    }
}

/* WĄTEK POŁĄCZENIA */
void *recvmg(void *sock)
{
    struct client_info cl = *((struct client_info *)sock);
    bool TerminateNow = 0, czysc = 0;
    int len,i,j,a,id,tryb=0;
    char msg[CONST_MAX_BUFOR],*s,*s1,*s2,*s3;
    pthread_t tester_gniazda;

    pthread_mutex_lock(&mutex);
    id = idsock(cl.sockno);
    tryb = tabs[id];
    pthread_mutex_unlock(&mutex);
    int *pieczec = NULL;

    while((len = recv(cl.sockno,&msg,CONST_MAX_BUFOR,0)) > 0)
    {
        if (len<=0) continue;
        s = strdup(msg);
        s1 = GetLineToStr(s,1,'=',"");
        s2 = GetLineToStr(s,2,'=',"");

        //s3 = concat("Zmienna s1=",s1);
        //sendmessage(s3,cl.sockno,1);
        //s3 = concat("Zmienna s2=",s2);
        //sendmessage(s3,cl.sockno,1);

        if (strcmp(s,"exit")==0) {
            TerminateNow = 1;
        } else
        if (strcmp(s1,"gpio")==0 && tryb==1 && strcmp(s2,"refresh")==0) {
            pthread_mutex_lock(&mutex_t);
            timer_count = 0;
            pthread_mutex_unlock(&mutex_t);
        } else
        if (strcmp(s1,"gpio")==0 && (tryb==0 || tryb==1)) {
            if (strcmp(s2,"on")==0) {
                if (REVERSE) set_GPIO(0); else set_GPIO(1);
                if (REVERSE) sendmessage("gpio=1",cl.sockno,1); else sendmessage("gpio=0",cl.sockno,1);
            } else
            if (strcmp(s2,"off")==0) {
                if (REVERSE) set_GPIO(1); else set_GPIO(0);
                if (REVERSE) sendmessage("gpio=0",cl.sockno,1); else sendmessage("gpio=1",cl.sockno,1);
            } else
            if (strcmp(s2,"status")==0) {
                a = get_GPIO();
                if (a>1) {
                    sendmessage(concat("gpio=",itoa(a,10)),cl.sockno,1);
                } else {
                    if (a==1) {
                        if (REVERSE) sendmessage("gpio=0",cl.sockno,1); else sendmessage("gpio=1",cl.sockno,1);
                    } else {
                        if (REVERSE) sendmessage("gpio=1",cl.sockno,1); else sendmessage("gpio=0",cl.sockno,1);
                    }
                }
            }
            if (tryb=0) TerminateNow = 1;
        } else
        if (tryb==1) {
            /* wywołania z gui gpio wykonujące różne dodatkowe funkcje */
            if (strcmp(s1,"tv")==0) {
                if (strcmp(s2,"on")==0) {
                    system("systemctl start tvheadend");
                    sendmessage("STATUS_TV_ON",cl.sockno,1);
                } else
                if (strcmp(s2,"off")==0) {
                    system("systemctl stop tvheadend");
                    sendmessage("tv=off",cl.sockno,1);
                }
            } else
            if (strcmp(s1,"laptop")==0) {
                if (strcmp(s2,"start")==0) {
                    for(i = 0; i < n; i++) {
                        if(tabs[i] == 3)
                        {
                            char *ss = concat("wakeonlan ",LAPTOP_MAC_ADDRESS);
                            system(ss);
                        }
                    }
                    sendmessage("laptop=start",cl.sockno,1);
                } else
                if (strcmp(s2,"restart")==0) {
                    for(i = 0; i < n; i++) {
                        if(tabs[i] == 3)
                        {
                            sendmessage("restart",clients[i],1);
                        }
                    }
                    sendmessage("laptop=restart",cl.sockno,1);
                } else
                if (strcmp(s2,"shutdown")==0) {
                    for(i = 0; i < n; i++) {
                        if(tabs[i] == 3)
                        {
                            sendmessage("shutdown",clients[i],1);
                        }
                    }
                    sendmessage("laptop=shutdown",cl.sockno,1);
                }
            }
        } else
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
                    if (REVERSE) set_GPIO(0); else set_GPIO(1);
                    if (REVERSE) sendmessage("gpio=1",cl.sockno,1); else sendmessage("gpio=0",cl.sockno,1);
                }
                if (AUTO_TIMER) {
                    pthread_mutex_lock(&mutex_t);
                    pieczec = malloc(sizeof(int));
                    *pieczec = cl.sockno;
                    pthread_create(&tester_gniazda,NULL,procedure_test,&pieczec);
                    pthread_mutex_unlock(&mutex_t);
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
                pthread_mutex_unlock(&mutex);
            }
        } else
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

        //sendmessage(s,cl.sockno,1);

        if (TerminateNow) break;
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
        if (REVERSE) set_GPIO(1); else set_GPIO(0);
        //if (REVERSE) sendmessage("gpio=0",cl.sockno,1); else sendmessage("gpio=1",cl.sockno,1);
    }
    if (gpio_adresat == cl.sockno) gpio_adresat = -1;
    if (pilot_adresat == cl.sockno)
    {
        pilot_adresat = -1;
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
	if(clients[i] == cl.sockno)
        {
	    j = i;
	    while(j < n-1)
            {
		clients[j] = clients[j+1];
                strcpy(ips[j],ips[j+1]);
                ports[j] = ports[j+1];
                strcpy(keys[j],keys[j+1]);
                tabs[j] = tabs[j+1];
		j++;
	    }
            break;
        }
    }
    n--;
    shutdown(cl.sockno,SHUT_RDWR);
    pthread_mutex_unlock(&mutex);
}

