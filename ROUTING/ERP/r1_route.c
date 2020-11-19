#include <stdio.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <jansson.h>
#include <string.h>
#include <arpa/inet.h>
#define MAX_HOP 32
#define MAX_NETWORK 256
#define MSG_TYPE_UNSPEC 0
#define MSG_TYPE_UPDATE 1
#define MSG_TYPE_WITHDRAW 2
struct prefix {
  struct in_addr addr;
  uint32_t length;
};

struct message {
  uint32_t type; // MSG_TYPE_XX
  struct in_addr path[MAX_HOP];
  struct prefix networks[MAX_NETWORK];
};

struct neighbor {
  struct in_addr address;
};
int main() {
  char addr_str[256];
  struct message msg;
  json_t *read_json_ob;
  json_error_t error;
  read_json_ob = json_load_file("./setting.json", 0, &error);
  if (read_json_ob == NULL ) {
    printf("json configが読み取れません\n");
    return -1;
  }
  inet_pton(AF_INET, json_string_value(json_object_get(
            read_json_ob, "router-id")), 
            &msg.path[0]);

  json_t *json_net;
  json_net = json_object_get(
             read_json_ob, "networks");
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
    msg.networks->length = atoi(p + 1);
  }
  struct sockaddr_in addr;
  struct sockaddr_in client;
  int option = 1;
  int sock0;
  int sock;
  sock0 = socket(AF_INET, SOCK_STREAM, 0);
  setsockopt(sock0, SOL_SOCKET, SO_REUSEADDR, 
             &option, sizeof(option));
  
  addr.sin_family = AF_INET;
  addr.sin_port = htons(12345);
  addr.sin_addr.s_addr = INADDR_ANY;
  bind(sock0, (struct sockaddr*) &addr, sizeof(addr));

  listen(sock0, 1);

  int len = sizeof(client);
  sock = accept(sock0, (struct sockaddr *)&client, &len);

  msg.type = MSG_TYPE_UPDATE;

  sendto(sock, &msg, sizeof(msg),
             0, (struct sockaddr *)&addr, sizeof(addr));

  return 0;
}

