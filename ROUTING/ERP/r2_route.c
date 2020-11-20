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
#define MAX_NETWORK 2
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
  char addr_str[256];
  struct message msg;
  json_t *read_json_ob;
  json_error_t error;
  read_json_ob = json_load_file("./setting.json", 0, &error);
  if (read_json_ob == NULL ) {
    printf("json configが読み取れません\n");
    return -1;
  }

  json_t *json_nei;
  json_nei = json_object_get(
             read_json_ob, "r2_neighbors");
  if (json_nei == NULL) {
    printf("neighborsの値を取得することができません \n");
    return -1;  
  }

  int index;
  json_t *json_neiaddr;
  json_array_foreach(json_nei, index, json_neiaddr) {
           inet_pton(AF_INET, json_string_value(
	             json_object_get(
	             json_neiaddr, "address")), 
                     &msg.path[index]);
  }


  json_t *json_net;
  json_net = json_object_get(
             read_json_ob, "r2_networks");
  if (json_net == NULL) {
    printf("networkの値を取得することができません \n");
    return -1;
  }

  int index_pre;
  char *p;
  int lenp;
  char prex_addr_len[256];
  char prex_addr[256];
  json_t *json_pre;

  json_array_foreach(json_net, index_pre, json_pre) {
    strcpy(prex_addr_len, json_string_value(
           json_object_get(json_pre, "prefix")));

    /* ここの時点で文字列はCIDR表記となっているため
       表記の部分から後のアドレスを取り出す */
    p = strrchr(prex_addr_len, '/');

    /* アドレスから表記の部分まで  */
    lenp = p - &prex_addr_len[0];

    /* prex_addrに元の文字列から表記を取り出したもの文字列を取り出す  */
    strncpy(prex_addr, prex_addr_len, lenp);

    /* 終端文字入れ  */
    prex_addr[lenp] = '\0';

    inet_pton(AF_INET, prex_addr, &msg.networks[index_pre].addr);
    msg.networks[index_pre].length = atoi(p + 1);
  }
  struct sockaddr_in addr;
  struct sockaddr_in client;
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
  for (i = 0; i < MAX_NETWORK; i++){
    if (msghdr->type == MSG_TYPE_UPDATE) {
      strcpy(msgtype, "UPDATE");
    }

    strcpy(adr_str,inet_ntoa(msghdr->path[0]));
    strcpy(net_addr, inet_ntoa(msghdr->networks[i].addr));
    printf("type =  %s, path =[%s], network = {%s/%d} \n", 
	   msgtype, adr_str, net_addr, msghdr->networks[i].length);
  }
  int len = sizeof(client);
  sock0 = accept(sock, (struct sockaddr *)&client, &len);

  msg.type = MSG_TYPE_UPDATE;

  sendto(sock, &msg, sizeof(msg),
             0, (struct sockaddr *)&addr, sizeof(addr));
  
   /* socketの終了 */
  close(sock);

  return 0;
}
