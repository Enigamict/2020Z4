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

// 実行時  -ljansson が必要 janssonがインストールされてない場合 sudo apt-get install libjansson-dev

int main(int argc, char** argv)
{
	if (argc < 2) {
		printf("use argc");
		return 1;
	}
	struct sockaddr_in server;
	int sock;
	int n;
	int val = 1;
	char buf[32];
	json_t* read_json_ob;
    	json_error_t error;
	char ipadress[100];
	 /* ソケットの作成 */
	sock = socket(AF_INET, SOCK_STREAM, 0);

	read_json_ob = json_load_file(argv[1], 0, &error);
    	if ( read_json_ob == NULL )
    	{
        	printf("NULL\n");
        	return 1;
   	 }
	printf("read done\n");
	json_t* read_json_ob_neighbors;
    	read_json_ob_neighbors = json_object_get(
		read_json_ob, "neighbors");
    	if (read_json_ob_neighbors == NULL) {
        	printf("NULL \n");
        	return 0;;
    	}

    	int index;
    	json_t *read_json_ob_ipadress;
    	json_array_foreach(read_json_ob_neighbors, 
		index, read_json_ob_ipadress) {

    		stpcpy(ipadress, json_string_value(
			json_object_get(read_json_ob_ipadress, 
			"address")));
    		printf("%s \n", ipadress);
    	};
	
	printf("parse done \n");
	 /* 接続先指定用構造体の準備 */
	server.sin_family = AF_INET;
	server.sin_port = htons(12345);
	server.sin_addr.s_addr = inet_addr(ipadress);
	

	 /* サーバに接続 */
	connect(sock, (struct sockaddr *)&server, sizeof(server));
	memset(buf, 0, sizeof(buf));
	n = recv(sock, buf, sizeof(buf), 0);

	//int len = 0;
	//len = sizeof(buf);
	//send(sock, buf, len, 0);
	//printf("%d, %s\n", n, buf);

	 /* socketの終了 */
	close(sock);

	return 0;
}
