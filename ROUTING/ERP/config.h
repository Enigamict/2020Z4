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

static inline void
config_parse(struct config* cfg, char* json)
{
  char addr_str[256];
  json_t* read_json_ob;
  json_error_t error;
  read_json_ob = json_load_file(json, 0, &error);
  if ( read_json_ob == NULL )
  {
        printf("NULL\n");
  }
  printf("read done\n");
  inet_aton(json_string_value(json_object_get(read_json_ob, "router-id")), 
            &cfg->router_id);
  inet_ntop(AF_INET, &cfg->router_id, addr_str ,256);
  printf("router-id: %s\n", addr_str);
  json_t* read_json_ob_neighbors;
  read_json_ob_neighbors = json_object_get(
	  read_json_ob, "neighbors");
  if (read_json_ob_neighbors == NULL) {
	  printf("NULL \n");
  }
  
  int index;
  json_t *read_json_ob_ipadress;
  json_array_foreach(read_json_ob_neighbors, 
	 index, read_json_ob_ipadress) {
	 inet_aton(json_string_value(json_object_get(
		   read_json_ob_ipadress, "address")),
		   &cfg->neighbors[index]->address);
    inet_ntop(AF_INET, &cfg->neighbors[index]->address, addr_str, 256);
    printf("network[%d]: %s\n", index, addr_str);
  }
}

