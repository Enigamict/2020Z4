#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <jansson.h>
#include <string.h>
#define MAX_NEIGH 32
#define MAX_NETWORK 256

struct config {
  struct in_addr router_id;
  struct neighbor *neighbors[MAX_NEIGH];
  struct network *networks[MAX_NETWORK];
};

struct neighbor {
  struct in_addr address;
};

struct prefix {
  struct in_addr addr;
  uint32_t length;
};

struct network {
  struct prefix prefix;
};

static inline int
config_parse(struct config* cfg, char* json)
{
  char addr_str[256];
  json_t *read_json_ob;
  json_error_t error;
  read_json_ob = json_load_file(json, 0, &error);
  if (read_json_ob == NULL ) {
    printf("json configが読み取れません\n");
    return -1;
  }
  inet_pton(AF_INET, json_string_value(json_object_get(
            read_json_ob, "router-id")), 
            &cfg->router_id);
  inet_ntop(AF_INET, &cfg->router_id, addr_str ,256);
  printf("router-id: %s\n", addr_str);
  
  /* ここからneighbors  */

  json_t *json_nei;
  json_nei = json_object_get(
             read_json_ob, "neighbors");
  if (json_nei == NULL) {
    printf("neighborsの値を取得することができません \n");
    return -1;  
  }

  struct neighbor *ns[MAX_NEIGH];
  int index;
  char nei_addr[256];
  
  json_t *json_neiaddr;
  json_array_foreach(json_nei, index, json_neiaddr) {
    strcpy(nei_addr ,json_string_value(
	   json_object_get(
	   json_neiaddr, "address")));
    ns[index] = (struct neighbor *)malloc(sizeof(struct neighbor));
    inet_pton(AF_INET, nei_addr, &ns[index]->address);
    inet_ntop(AF_INET, &ns[index]->address, addr_str, 256);
    printf("neighbor[%d]: %s\n", index, addr_str);
  }

 /* network */
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
  struct prefix *nk[MAX_NETWORK];
  json_t *json_pre;

  json_array_foreach(json_net, index_pre, json_pre) {
    strcpy(prex_addr_len, json_string_value(
           json_object_get(json_pre, "prefix")));
    nk[index_pre] = (struct prefix *)malloc(sizeof(struct prefix));

    /* ここの時点で文字列はCIDR表記となっているため
       表記の部分から後のアドレスを取り出す */ 
    p = strrchr(prex_addr_len, '/');

    /* アドレスから表記の部分まで  */
    lenp = p - &prex_addr_len[0];

    /* prex_addrに元の文字列から表記を取り出したもの文字列を取り出す  */
    strncpy(prex_addr, prex_addr_len, lenp);

    /* 終端文字入れ  */
    prex_addr[lenp] = '\0';

    inet_pton(AF_INET, prex_addr, &nk[index_pre]->addr);
    inet_ntop(AF_INET,  &nk[index_pre]->addr, addr_str, 256);
    nk[index_pre]->length = atoi(p + 1);
    printf("network[%d]: %s/%d\n", index_pre, addr_str,
            nk[index_pre]->length);
  }
  
}
