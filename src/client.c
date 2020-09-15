#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <netinet/in.h>

#include "lcd_ui.h"

#define PORT 5432
#define MAX_QUE_CONN_NM 5
#define BUFFER_SIZE 1024

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

	//打开屏幕
	LcdDevice* lcd = create_lcd("/dev/ubuntu_lcd");
	//LcdDevice* lcd = create_lcd("/dev/fb0");
	if(lcd == NULL) exit(1);//为空指针表示没有创建成功
	clear_lcd(lcd, 0x000000);//黑色
	
	
	char buf[1024]={0};
	char pic[800*480*3]={0};
	recv(client_sock_fd,pic,BUFFER_SIZE,0);

	printf("buf = %s\n",pic);


	destroy_lcd(lcd);

	close(client_sock_fd);
}

