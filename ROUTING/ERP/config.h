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

  for (int i = 0; i < MAX_NEIGH; i++) {
    if (!cfg->networks[i])
      continue;
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

    		stpcpy(addr_str, json_string_value(
			json_object_get(read_json_ob_ipadress, 
			"address")));
    		printf("%s \n", addr_str);
    	};
    inet_ntop(AF_INET, &cfg->neighbors[i]->address, addr_str, 256);
    printf("test\n");
    printf("neighbor[%d]: %s\n", i, addr_str);
  }

  for (size_t i = 0; i < MAX_NETWORK; i++) {
    if (!cfg->networks[i])
      continue;
    inet_ntop(AF_INET, &cfg->networks[i]->prefix.addr,
              addr_str, 256);
    printf("network[%d]: %s\n", i, addr_str,
           cfg->networks[i]->prefix.length);
  }
}

//static inline void
//config_parse()
