#include <stdio.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <jansson.h>
#include <string.h>

// 実行時  -ljansson が必要 janssonがインストールされてない場合 sudo apt-get install libjansson-dev

int main(int argc, char** argv)
{
	if (argc < 2) {
		printf("use argc");
		return 1;
	}
	struct sockaddr_in addr;
	struct sockaddr_in client;
        int option = 1;
        int sock0;
        int sock;
	json_t* read_json_ob;
	json_error_t error;
	char ipadress[100];
        sock0 = socket(AF_INET, SOCK_STREAM, 0);
        setsockopt(sock0, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
 	
	read_json_ob = json_load_file(argv[1], 0, &error);
    	if ( read_json_ob == NULL )
    	{
        	printf("NULL\n");
        	return 1;
    	}

	strcpy(ipadress, json_string_value(json_object_get(read_json_ob, "adress")));
        addr.sin_family = AF_INET;
        addr.sin_port = htons(12345);
        addr.sin_addr.s_addr = INADDR_ANY;
        bind(sock0, (struct sockaddr*) &addr, sizeof(addr));
        listen(sock0, 1);
	int len = sizeof(client);
        sock = accept(sock0, (struct sockaddr *)&client, &len);

	/* ここでkeepaliveを設定 */
        setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, &option, sizeof(option));  
        /* ここからkeepAliveのオプション */
	option = 3;
        setsockopt(sock, IPPROTO_TCP, TCP_KEEPIDLE, &option, sizeof(option));
 
        option = 1;
        setsockopt(sock, IPPROTO_TCP, TCP_KEEPINTVL, &option, sizeof(option));
        
	option = 3;
        setsockopt(sock, IPPROTO_TCP, TCP_KEEPCNT, &option, sizeof(option));

 	recv(sock, NULL, 0, 0);
        return 0;
}
