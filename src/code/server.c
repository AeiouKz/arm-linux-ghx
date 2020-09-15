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
	//第一步：建立套接字 
	int server_sock_fd = socket(AF_INET,SOCK_STREAM,0);
	if(server_sock_fd < 0)
	{
		perror("socket error");
		return 0;
	}
	//第二步：绑定地址 
	struct sockaddr_in server_addr;
	bzero(&server_addr,sizeof(server_addr));  //置0

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(atoi(argv[2]));
	server_addr.sin_addr.s_addr = inet_addr(argv[1]);

	int ret = bind(server_sock_fd,(struct sockaddr*)&server_addr,sizeof(server_addr));
	if(ret <0)
	{
		perror("bind error");
		close(server_sock_fd);
		return -1;
	}
	//第三步：设置服务器的监听

	ret = listen(server_sock_fd,10);
	if(ret <0)
	{
		perror("listen error");
		close(server_sock_fd);
		return -1;
	}
	//第四步：接受链接 
	while(1)
	{
		struct sockaddr_in client_addr;
		bzero(&client_addr,sizeof(client_addr));
		int addr_len = sizeof(client_addr);
		int client_sock_fd = accept(server_sock_fd,(struct sockaddr*)&client_addr,&addr_len);
		if(client_sock_fd < 0)
		{
			perror("accept error");
			close(server_sock_fd);
			return -1;
		}
		//第五步：收发数据 
		printf("client_port = %d\n",ntohs(client_addr.sin_port));

		char client_addr_str[16]={0};

		inet_ntop(AF_INET,&client_addr.sin_addr.s_addr,client_addr_str,INET_ADDRSTRLEN);
		printf("client_addr_str = %s\n",client_addr_str);
		char *str = "connect server success!\n";

		ret = send(client_sock_fd,str,strlen(str),0);
		if(ret < 0)
		{
			perror("send error");
			close(server_sock_fd);
			close(client_sock_fd);
			return -1;
		}
		close(client_sock_fd);
	}
	//第六步：关闭 
	close(server_sock_fd);
	
}
