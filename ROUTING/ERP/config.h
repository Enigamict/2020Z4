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
  if (read_json_ob == NULL )
  {
        printf("json configが読み取れません\n");
	return -1;
  }
  inet_pton(AF_INET, json_string_value(json_object_get
           (read_json_ob, "router-id")), 
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

  int index;
  json_t *json_neiaddr;
  json_array_foreach(json_nei, index, json_neiaddr) {
                     inet_pton(AF_INET, json_string_value
		     (json_object_get(json_neiaddr, "address")), 
		     &cfg->neighbors[13]->address);

  inet_ntop(AF_INET, &cfg->neighbors[13]->address, addr_str, 256);
  printf("neighbor[%d]: %s\n", index, addr_str);
 }
}


