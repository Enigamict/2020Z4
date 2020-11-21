#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <jansson.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "netlink.h"
#include "config.h"

// 実行時  -ljansson が必要 janssonがインストールされてない場合 sudo apt-get install libjansson-dev


int main(int argc, char** argv)
{
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
  struct sockaddr_in server;
  int sock;
  int sock0;
  int i;
  int n;
  char buf[10000];
  char adr_str[256];
  char net_addr[256];
  char msgtype[10];
   /* ソケットの作成 */
  sock = socket(AF_INET, SOCK_STREAM, 0);
  int fd = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
   /* 接続先指定用構造体の準備 */
  server.sin_family = AF_INET;
  server.sin_port = htons(12345);
  server.sin_addr.s_addr = inet_addr("10.255.1.1");


   /* サーバに接続 */
  connect(sock, (struct sockaddr *)&server, sizeof(server));
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

    strcpy(adr_str,inet_ntoa(msghdr->path[0]));
    strcpy(net_addr, inet_ntoa(msghdr->networks[i].addr));
    printf("type =  %s, path =[%s], network = {%s/%d} \n", 
	   msgtype, adr_str, net_addr, msghdr->networks[i].length);
    
    adddel_route(fd, net_addr, msghdr->networks[i].length, adr_str, 17, true);	
  }
  
  close(fd);
  msg.type = MSG_TYPE_UPDATE;

  for (size_t i = 0; i < MAX_NETWORK; i++){
    if (!cfg.networks[i])
      continue;
    msg.networks[i] = cfg.networks[i]->prefix;
  }
  
  for (size_t i = 0; i < MAX_NEIGH; i++){
    if (!cfg.networks[i])
      continue;
   //msg.path[i] = cfg.networks[i]->prefix;
  }


  sendto(sock, &msg, sizeof(msg),
             0, (struct sockaddr *)&addr, sizeof(addr));
  
   /* socketの終了 */
  close(sock);

  return 0;
}
