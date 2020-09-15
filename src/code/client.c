#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <netinet/in.h>
int main(int argc,char *argv[])
{
	//第一步：创建一个套接字 
	int client_sock_fd = socket(AF_INET,SOCK_STREAM,0);

	//第二步：建立连接 
	
	struct sockaddr_in server_addr;
	bzero(&server_addr,sizeof(server_addr));

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(atoi(argv[2]));
	server_addr.sin_addr.s_addr = inet_addr(argv[1]);
	connect(client_sock_fd,(struct sockaddr *)&server_addr,sizeof(server_addr));

	char buf[256]={0};
	recv(client_sock_fd,buf,256,0);

	printf("buf = %s\n",buf);

	close(client_sock_fd);
}

