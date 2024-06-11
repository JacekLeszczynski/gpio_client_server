#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

MYSQL *db;

bool DbConnect() {
    db = mysql_init(NULL);
    if (CONST_TEST) {
        if (!mysql_real_connect(db,DB_HOST_TEST,DB_USER_TEST,DB_PASS_TEST,DB_DATABASE,0,NULL,0)) return 0; else return 1;
    } else {
        db = mysql_init(NULL);
        if (!mysql_real_connect(db,DB_HOST,DB_USER,DB_PASS,DB_DATABASE,0,NULL,0)) return 0; else return 1;
    }
}

void DbDisconnect() {
    mysql_close(db);
}

int DzienRoboczy() {
    MYSQL_RES *res;
    MYSQL_ROW row;
    int a = 0;

    if (mysql_query(db,"select IsWorkDayNow()")) {
        return -1;
    }
    res = mysql_store_result(db);
    if ((row = mysql_fetch_row(res)) != NULL) {
        const int wynik = atoi(row[0]);
        a = wynik;
    } else {
        mysql_free_result(res);
        return -1;
    }
    mysql_free_result(res);
    return a;
}

char *GetHoursSunDay(double longitude, double latitude) {
    MYSQL_RES *res;
    MYSQL_ROW row;
    char s[40];

    if (mysql_query(db,"call CalculateSunriseSunset(longitude,latitude,CURDATE(),@time1,@time2); select @time1,@time2;")) {
    //if (mysql_query(db,"call CalculateSunriseSunset(longitude,latitude,CURRENT_DATE(),@time1,@time2); select @time1,@time2;")) {
        return "err";
    }
    res = mysql_store_result(db);
    if ((row = mysql_fetch_row(res)) != NULL) {
        const char *wynik = row[0];
        strcpy(s,wynik);
        //s = wynik;
    } else {
        mysql_free_result(res);
        return "";
    }
    mysql_free_result(res);
    return strdup(s);
}

void wake_on_lan(const char *mac_addr) {
  int sockfd;
  char buf[64];

  // Open a socket for UDP
  sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd < 0) {
    perror("socket");
    exit(1);
  }

  // Bind the socket to any address and port
  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(9);
  addr.sin_addr.s_addr = INADDR_ANY;
  if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
    perror("bind");
    exit(1);
  }

  // Build the wake-on-lan packet
  sprintf(buf, "%02X:%02X:%02X:%02X:%02X:%02X",
          mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);

  // Send the packet
  if (sendto(sockfd, buf, strlen(buf), 0, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
    perror("sendto");
    exit(1);
  }

  close(sockfd);
}

int idsock(int soket)
{
    int i, a = -1;
    for(i=0; i<n; i++)
    {
        if (clients[i]==soket)
        {
            a=i;
            break;
        }
    }
    return a;
}

void log_message(char *filename, char *message)
{
    FILE *logfile;
    logfile=fopen(filename,"a");
    if(!logfile) return;
    fprintf(logfile,"%s %s\n",LocalTime(),message);
    fclose(logfile);
}

