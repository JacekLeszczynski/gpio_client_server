/* TRYB 1 - KLIENT GUI GPIO - DODATKOWE FUNKCJE */
if (tryb==1) {
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

