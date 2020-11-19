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
#define MAX_HOP 32
#define MAX_NETWORK 256
#define MSG_TYPE_UNSPEC 0
#define MSG_TYPE_UPDATE 1
#define MSG_TYPE_WITHDRAW 2

// 実行時  -ljansson が必要 janssonがインストールされてない場合 sudo apt-get install libjansson-dev

struct prefix {
    struct in_addr addr;
    uint32_t length;
};
 
struct message {
  uint32_t type; // MSG_TYPE_XX
  struct in_addr path[MAX_HOP];
  struct prefix networks[MAX_NETWORK];
};
int main()
{
  struct message msg;
  struct sockaddr_in server;
  int sock;
  int n;
  int val = 1;
  char buf[10000];
  char adr_str[256];
  char net_addr[256];
  char msgtype[10];
   /* ソケットの作成 */
  sock = socket(AF_INET, SOCK_STREAM, 0);

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
  //strcpy(adr_str,inet_ntoa(msghdr->networks[0]->));
  if (msghdr->type == MSG_TYPE_UPDATE) {
    strcpy(msgtype, "UPDATE");
  }

  strcpy(adr_str,inet_ntoa(msghdr->path[0]));
  strcpy(net_addr, inet_ntoa(msghdr->networks[0].addr));
  printf("type =  %s, path =[%s], network = {%s/%d} ", 
         msgtype, adr_str, net_addr, msghdr->networks[0].length);

  
   /* socketの終了 */
  close(sock);

  return 0;
}
