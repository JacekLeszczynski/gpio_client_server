/* GPIO MONITOR */

if (tryb==4) {
    int zmienna = -1;

    //przeczytanie wszystkich stanów GPIO
    if (strcmp(s1,"status")==0) {
        sendmessage_status_gpio(cl.sockno,1);
    } else
    if (strcmp(s1,"getworkday")==0) {
        bool db_connected = DbConnect();
        if (db_connected) {
            zmienna = DzienRoboczy();
            DbDisconnect();
        } else { zmienna = -1; }
        if (zmienna==1)
        {
            sendmessage("workday=1",cl.sockno,1);
        } else {
            if (zmienna==0) {
                sendmessage("workday=0",cl.sockno,1);
            } else {
                sendmessage("workday=-1",cl.sockno,1);
            }
        }
    } else
    if (strcmp(s1,"statusday")==0) {
        bool db_connected = DbConnect();
        if (db_connected) {
            zmienna = StatusDayNow();
            DbDisconnect();
        } else { zmienna = -1; }
        if (zmienna==1) {
            //sobota
            sendmessage("statusday=1",cl.sockno,1);
        } else {
            if (zmienna==2) {
                //niedziela lub święto
                sendmessage("statusday=2",cl.sockno,1);
            } else {
                if (zmienna==3) {
                    //urlop
                    sendmessage("statusday=3",cl.sockno,1);
                } else {
                    if (zmienna==0) {
                        //dzień roboczy
                        sendmessage("statusday=0",cl.sockno,1);
                    } else {
                        //coś nie wyszło
                        sendmessage("statusday=-1",cl.sockno,1);
                    }
                }
            }
        }
    } else
    if (strcmp(s1,"getcalendar")==0) {
        bool db_connected = DbConnect();
        if (db_connected) {
            ss = GetCalendarAll();
            DbDisconnect();
        } else { ss = String("E"); }
        ss = concat("calendar=",ss);
        sendmessage(ss,cl.sockno,1);
    } else

    if (strcmp(s1,"laptop")==0) {
        if (strcmp(s2,"status")==0) {
            if (GetLaptopLogin()) {
                sendtoall1("laptop=login",-1,0);
            } else {
                sendtoall1("laptop=logout",-1,0);
            }
        } else
        if (strcmp(s2,"start")==0) {
            sendmessage("laptop=starting",cl.sockno,1);
            ss = String("wakeonlan ");
            ss = concat(ss,LAPTOP_MAC_ADDRESS);
            system(ss);
            sleep(1);
            system(ss);
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
                if(tabs[i] == 3) {
                    ss = String("laptop=shutdown=");
                    ss = concat(ss,IntToSys(cl.sockno,10));
                    sendmessage(ss,clients[i],1);
                }
            }
        }
    } else

    //ustawienie GPIO
    if (strcmp(s1,"set")==0) {
        pthread_mutex_lock(&mutex);
        if (strcmp(s2,"1")==0) {
            if (strcmp(s3,"1")==0) { set_GPIO(GPIO_NR_1,1); s="gpio=1"; } else { set_GPIO(GPIO_NR_1,0); s="gpio=0"; }
        } else
        if (strcmp(s2,"2")==0) {
            if (strcmp(s3,"1")==0) { set_GPIO(GPIO_NR_2,1); s="gpio2=1"; } else { set_GPIO(GPIO_NR_2,0); s="gpio2=0"; }
        } else
        if (strcmp(s2,"3")==0) {
            if (strcmp(s3,"1")==0) { set_GPIO(GPIO_NR_3,1); s="gpio3=1"; } else { set_GPIO(GPIO_NR_3,0); s="gpio3=0"; }
        } else
        if (strcmp(s2,"4")==0) {
            if (strcmp(s3,"1")==0) { set_GPIO(GPIO_NR_4,1); s="gpio4=1"; } else { set_GPIO(GPIO_NR_4,0); s="gpio4=0"; }
        } else
        if (strcmp(s2,"5")==0) {
            if (strcmp(s3,"1")==0) { set_GPIO(GPIO_NR_5,1); s="gpio5=1"; } else { set_GPIO(GPIO_NR_5,0); s="gpio5=0"; }
        } else
        if (strcmp(s2,"6")==0) {
            if (strcmp(s3,"1")==0) { set_GPIO(GPIO_NR_6,1); s="gpio6=1"; } else { set_GPIO(GPIO_NR_6,0); s="gpio6=0"; }
        } else
        if (strcmp(s2,"7")==0) {
            if (strcmp(s3,"1")==0) { set_GPIO(GPIO_NR_7,1); s="gpio7=1"; } else { set_GPIO(GPIO_NR_7,0); s="gpio7=0"; }
        } else
        if (strcmp(s2,"8")==0) {
            if (strcmp(s3,"1")==0) { set_GPIO(GPIO_NR_8,1); s="gpio8=1"; } else { set_GPIO(GPIO_NR_8,0); s="gpio8=0"; }
        }
        sendmessage(s,cl.sockno,0);
        sendmessage_status_gpio_all(cl.sockno,0);
        pthread_mutex_unlock(&mutex);
    }

} else

