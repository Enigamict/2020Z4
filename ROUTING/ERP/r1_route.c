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

static void parse(const void* ptr, size_t len)
{
	const uint8_t *ptr0 = (const uint8_t*)ptr;
	for (;;) {
		const struct {
			struct nlmsghdr n;
			struct rtmsg r;
			char buf[4096];
		} *msg = (const void *)ptr0;

		if (msg->n.nlmsg_type == NLMSG_ERROR ||
		    msg->n.nlmsg_type == NLMSG_DONE ||
		    msg->n.nlmsg_type == 0) {
			printf("DONE\n");
			break;
		}

		ptr0 = (ptr0 + msg->n.nlmsg_len);
	}
	return;
}
static void adddel_route(int fd, const char *dststr, int plen, const char *nexthopstr, uint32_t oif, bool is_add)
{
	struct {
		struct nlmsghdr n;
		struct rtmsg r;
		char buf[4096];
	} req = {
		.n.nlmsg_len = NLMSG_LENGTH(sizeof(struct rtmsg)),
		.n.nlmsg_flags = NLM_F_REQUEST | NLM_F_CREATE | NLM_F_ACK | NLM_F_REPLACE,
		.n.nlmsg_type = is_add ? RTM_NEWROUTE : RTM_DELROUTE,
		.r.rtm_family = AF_INET,
		.r.rtm_dst_len = plen,
		.r.rtm_src_len = 0,
		.r.rtm_tos = 0,
		.r.rtm_table = RT_TABLE_MAIN,
		.r.rtm_protocol = RTPROT_STATIC,
		.r.rtm_scope = RT_SCOPE_UNIVERSE, // FOR_VIA_ROUTE
		.r.rtm_type = RTN_UNICAST,
		.r.rtm_flags = 0,
	};

	/* set RTA_DST */
	struct in_addr prefix;
	inet_pton(AF_INET, dststr, &prefix);
	addattr_l(&req.n, sizeof(req), RTA_DST, &prefix, sizeof(struct in_addr));

	/* set RTA_GATEWAY */
	struct in_addr gw;
	inet_pton(AF_INET, nexthopstr, &gw);
	addattr_l(&req.n, sizeof(req),
		  RTA_GATEWAY, &gw,
		  sizeof(struct in_addr));

	/* set RTA_OIF */
	uint32_t oif_idx = oif;
	addattr32(&req.n, sizeof(req), RTA_OIF, oif_idx);

	/* set RTA_TABLE */
	uint32_t table = 254;
	addattr32(&req.n, sizeof(req), RTA_TABLE, table);

	/* submit request */
	char buf[10000];
	struct nlmsghdr *answer = (struct nlmsghdr*)buf;
	int ret = nl_talk(fd, &req.n, answer, sizeof(buf));
	if (ret < 0)
		exit(1);

	parse(answer, sizeof(buf));
}
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
  int option = 1;
  int sock0;
  int sock;
  int i;
  int n;
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

  listen(sock0, 1);

  int len = sizeof(client);
  sock = accept(sock0, (struct sockaddr *)&client, &len);
  
  for (size_t i = 0; i < MAX_NETWORK; i++){
    if (!cfg.networks[i])
      continue;
    msg.networks[i] = cfg.networks[i]->prefix;
  }
  msg.nexthop = cfg.neighbors[0]->address;
  // for (size_t i = 0; i < MAX_; i++){
  //  msg.path[i] = cfg.router_id;

  msg.type = MSG_TYPE_UPDATE;

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

	  strcpy(adr_str,inet_ntoa(msghdr->path[0]));
	  strcpy(net_addr, inet_ntoa(msghdr->networks[i].addr));
          uint32_t index = if_nametoindex("R1_net0");
	  printf("type =  %s, path =[%s], network = {%s/%d} \n", 
			  msgtype, adr_str, net_addr, msghdr->networks[i].length);

	  adddel_route(fd, net_addr,
                       msghdr->networks[i].length,
                       adr_str, index, true);

  }
  
  close(fd);

  return 0;

}
