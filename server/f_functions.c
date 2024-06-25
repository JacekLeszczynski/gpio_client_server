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

int StatusDayNow() {
    MYSQL_RES *res;
    MYSQL_ROW row;
    int a = 0;

    if (mysql_query(db,"select StatusDayNow()")) {
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

char *GetImieninyNow() {
    MYSQL_RES *res;
    MYSQL_ROW row;
    char a[255];

    if (mysql_query(db,"select GetImieniny(now())")) {
        return "";
    }
    res = mysql_store_result(db);
    if ((row = mysql_fetch_row(res)) != NULL) {
        const char *wynik = row[0];
        strcpy(a,wynik);
    } else {
        mysql_free_result(res);
        return "";
    }
    mysql_free_result(res);
    return strdup(a);
}

char DzienRoboczyToChar() {
    int r = DzienRoboczy();
    if (r==0) {
        return '0';
    } else {
        if (r==1) {
            return '1';
        } else {
            return '-';
        }
    }
}

char *DzienRoboczyToStr() {
    int r = DzienRoboczy();
    if (r==0) {
        return "0";
    } else {
        if (r==1) {
            return "1";
        } else {
            return "-";
        }
    }
}

void extract_date_and_time(const char *datetime, char *date, char *time) {
    // Sprawdzenie czy input ma odpowiednią długość
    if (strlen(datetime) < 19) {
        fprintf(stderr, "Invalid input format\n");
        return;
    }

    // Skopiowanie części "yyyy-mm-dd" do zmiennej wyjściowej 'date'
    strncpy(date, datetime, 10);
    date[10] = '\0';  // Null-terminate the string

    // Skopiowanie części "hh:mm" do zmiennej wyjściowej 'time'
    strncpy(time, datetime + 11, 5);
    time[5] = '\0';  // Null-terminate the string
}

void split_times(const char *input, char *t1, char *t2) {
    // Znalezienie pozycji separatora '-'
    const char *separator = strchr(input, '-');
    if (separator == NULL) {
        fprintf(stderr, "Invalid input format\n");
        return;
    }

    // Skopiowanie pierwszej części (przed '-')
    size_t length1 = separator - input;
    strncpy(t1, input, length1);
    t1[length1] = '\0';  // Null-terminate the first string

    // Skopiowanie drugiej części (po '-')
    const char *second_part = separator + 1;
    strncpy(t2, second_part, strlen(second_part));
    t2[strlen(second_part)] = '\0';  // Null-terminate the second string
}

char *GetHoursSunDay(double longitude, double latitude) {
    MYSQL_RES *res;
    MYSQL_ROW row;
    char s[40],query[256];

    snprintf(query, sizeof(query), "select sun_times(%f,%f,now())",longitude,latitude);
    if (mysql_query(db,query)) {
        return strdup(mysql_error(db));
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

char *GetCalendarAll() {
  char *s,dzien_roboczy,dzien,*tmp,t1[6],t2[6],date[11],time[6],*imieniny;

  int a = StatusDayNow();
  if (a==-1) {
    dzien_roboczy = '-';
  } else {
    if (a==0) {
      dzien_roboczy = '0';
    } else {
      if (a==1) {
        dzien_roboczy = '1';
      } else {
        if (a==2) {
          dzien_roboczy = '2';
        } else {
          dzien_roboczy = '3';
        }
      }
    }
  }

  //tmp = GetHoursSunDay(50.0614300,19.9365800);
  tmp = GetHoursSunDay(LATITUDE,LONGITUDE);

  split_times(tmp,t1,t2);
  extract_date_and_time(LocalTime(),date,time);
  if (strcmp(t1,time)<=0 && strcmp(time,t2)<=0) {
    dzien = '1';
  } else {
    dzien = '0';
  }

  imieniny = GetImieninyNow();

  s = String(date);
  s = concat_str_char(s,';');
  s = concat(s,time);
  s = concat_str_char(s,';');
  s = concat(s,t1);
  s = concat_str_char(s,';');
  s = concat(s,t2);
  s = concat_str_char(s,';');
  s = concat_str_char(s,dzien_roboczy);
  s = concat_str_char(s,';');
  s = concat_str_char(s,dzien);
  s = concat_str_char(s,';');
  s = concat(s,imieniny);
  return s;
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

