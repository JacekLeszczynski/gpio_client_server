#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

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

