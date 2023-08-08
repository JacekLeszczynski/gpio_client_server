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
        /*log = concat(s,"(id=");
        log = concat(log,IntToSys(id,10));
        log = concat(log,", tryb=");
        log = concat(log,IntToSys(tryb,10));
        log = concat_str_char(log,')');
        log_message("/tmp/server-gpio.log",log);*/
        s1 = GetLineToStr(s,1,'=',"");
        s2 = GetLineToStr(s,2,'=',"");
        s3 = GetLineToStr(s,3,'=',"");

        /* JEŚLI DOSTAJĘ EXIT - OPUSZCZAM PĘTLĘ I KOŃCZĘ WĄTEK */
        if (strcmp(s,"exit")==0) {
            TerminateNow = 1;
        } else

        /* ... NIE WIEM CO TU BYŁO ROBIONE ... */
        if (strcmp(s1,"gpio")==0 && tryb==1 && strcmp(s2,"refresh")==0) {
            pthread_mutex_lock(&mutex_t);
            timer_count = 0;
            pthread_mutex_unlock(&mutex_t);
        } else

        /* USTAWIANIE GPIO I CZYTANIE GPIO */
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
            if (tryb==0) TerminateNow = 1;
        } else

        /* TRYB 1 - KLIENT GUI GPIO - DODATKOWE FUNKCJE */
        if (tryb==1) {
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
                if (strcmp(s2,"status")==0) {
                    b_test = 0;
                    for(i = 0; i < n; i++) {
                        if (tabs[i]==3)
                        {
                            b_test = 1;
                            break;
                        }
                    }
                    if (b_test) {
                        sendtoall1("laptop=login",-1,0);
                    } else {
                        sendtoall1("laptop=logout",-1,0);
                    }
                } else
                if (strcmp(s2,"start")==0) {
                    ss = String("wakeonlan ");
                    ss = concat(ss,LAPTOP_MAC_ADDRESS);
                    system(ss);
                    sendmessage("laptop=starting",cl.sockno,1);
                } else
                if (strcmp(s2,"restart")==0) {
                    for(i = 0; i < n; i++) {
                        if(tabs[i] == 3)
                        {
                            ss = String("laptop=restart=");
                            ss = concat(ss,IntToSys(cl.sockno,10));
                            sendmessage(ss,clients[i],1);
                        }
                    }
                } else
                if (strcmp(s2,"shutdown")==0) {
                    for(i = 0; i < n; i++) {
                        if(tabs[i] == 3)
                        {
                            ss = String("laptop=shutdown=");
                            ss = concat(ss,IntToSys(cl.sockno,10));
                            sendmessage(ss,clients[i],1);
                        }
                    }
                }
            }
        } else

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
                sendtoall1("laptop=login",cl.sockno,0);
                pthread_mutex_unlock(&mutex);
            }
        } else

        /* ODPOWIEDZI OD LAPTOPA LUB DRUGIEGO KOMPUTERA */
        if (tryb==3) {
            adresat = atoi(s3);
            if (strcmp(s2,"shutdowning")==0) {
                shutdown_now[i] = adresat;
            } else
            if (strcmp(s2,"not_shutdown")==0) {
                shutdown_now[id] = -1;
            }
            ss = String(s1);
            ss = concat_str_char(ss,'=');
            ss = concat(ss,s2);
            sendtouser(ss,-1,adresat,1);
            sleep(1);
        } else

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
        if(tabs[i] == 3 && shutdown_now[i] != -1) {
            // wysyłam informację o zamknięciu
            sendtouser("laptop=shutdown_ok",cl.sockno,shutdown_now[i],0);
            sleep(1);
        }
        if(tabs[i] == 3 && clients[i] == cl.sockno) {
            // wysyłam informację o zamknięciu
            sendtoall1("laptop=logout",cl.sockno,0);
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

