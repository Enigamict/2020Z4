#include <stdio.h>
#include <netinet/in.h>
#include <net/if.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <jansson.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "netlink.h"
#include "config.h"

int main(int argc, char** argv) {
  struct  config cfg;
  int ret = config_parse(&cfg, argv[1]);
  if (ret < 0) {
    fprintf(stderr, "failed on config parser\n");
    return 1;
  }
  char addr_str[256];
  struct message msg;
  json_t *read_json_ob;
  json_error_t error;
  struct sockaddr_in addr;
  struct sockaddr_in client;
  struct ifreq ifr;
  struct timeval tv;
  fd_set fds, readfds;
  int option = 1;
  int sock0;
  int sock;
  int i;
  int n;
  int d;
  char buf[10000];
  char adr_str[256];
  char net_addr[256];
  char msgtype[10];
  sock0 = socket(AF_INET, SOCK_STREAM, 0);
  int fd = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
  setsockopt(sock0, SOL_SOCKET, SO_REUSEADDR, 
      &option, sizeof(option));

  addr.sin_family = AF_INET;
  addr.sin_port = htons(12345);
  addr.sin_addr.s_addr = INADDR_ANY;
  bind(sock0, (struct sockaddr*) &addr, sizeof(addr));

  listen(sock0, 2);
  tv.tv_sec = 10;
  tv.tv_usec = 0;
  FD_ZERO(&readfds);
  FD_SET(sock0, &readfds);
  int count;
  while(count < 2) {
    int len = sizeof(client);
    sock = accept(sock0, (struct sockaddr *)&client, &len);
    printf("%s", inet_ntoa(client.sin_addr));
    size_t ii = 0;
    for (size_t i = 0; i < MAX_NETWORK; i++){
      if (!cfg.networks[i])
	continue;
      msg.networks[ii] = cfg.networks[i]->prefix;
      ii++;
    }

    msg.type = MSG_TYPE_UPDATE;
    msg.path[0] = cfg.router_id;
    msg.nexthop = cfg.neighbors[0]->local_address;

    sendto(sock, &msg, sizeof(msg),
	0, (struct sockaddr *)&addr, sizeof(addr));

    memset(buf, 0, sizeof(buf));
    n = recv(sock, buf, sizeof(buf), 0);
    if (n < 1) {
      printf("okasii");
      return -1;
    }
    struct message *msghdr =  (struct message *)buf;
    for (i = 0; i < 2; i++){
      if (msghdr->type == MSG_TYPE_UPDATE) {
	strcpy(msgtype, "UPDATE");
      }
      if (inet_ntoa(client.sin_addr) == )

      strcpy(adr_str,inet_ntoa(msghdr->nexthop));
      strcpy(net_addr, inet_ntoa(msghdr->networks[i].addr));
      const char *name = cfg.neighbors[0]->ifname;
      uint32_t index = if_nametoindex(name);
      printf("type =  %s, path =[%s], network = {%s/%d} %u \n", 
	  msgtype, adr_str, net_addr, 
	  msghdr->networks[i].length, 
	  index);
      // adddel_route(fd, net_addr,
      //	msghdr->networks[i].length,
      //	adr_str, index, true);


    }
    count++;
  }

  close(fd);

  return 0;

}
