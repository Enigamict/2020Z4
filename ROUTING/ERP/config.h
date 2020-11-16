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
  if ( read_json_ob == NULL )
  {
        printf("NULL\n");
	return 0;
  }
  printf("read done\n");
  int pton = inet_pton(AF_INET, json_string_value(json_object_get
                      (read_json_ob, "router-id")), 
                       &cfg->router_id);
  printf("%d \n", pton);
  inet_ntop(AF_INET, &cfg->router_id, addr_str ,256);
  printf("router-id: %s\n", addr_str);
  /* ここからneighbors  */
  for (size_t i = 0; i < MAX_NEIGH; i++) {
    if (!cfg->networks[i])
      continue;
    json_t *json_nei;
    json_nei = json_object_get(
	    read_json_ob, "neighbors");
    if (json_nei == NULL) {
	  printf("NULL \n");
	  return 0;  
    }
    
    int index;
    json_t *json_neiaddr;
    json_array_foreach(json_nei, index, json_neiaddr) {
	    int pton = inet_pton(AF_INET, json_string_value
		       (json_object_get(json_neiaddr, "address")), 
			&cfg->neighbors[13]->address);
			if (pton < 0) {
			  printf("neighbors addr error");
			}
	    inet_ntop(AF_INET, &cfg->neighbors[13]->address, addr_str, 256);
	    printf("neighbor[%d]: %s\n", i, addr_str);

    }
}

}
