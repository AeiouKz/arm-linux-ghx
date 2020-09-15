#include <sys/types.h>	//open
#include <sys/stat.h>	//open
#include <fcntl.h> //open
#include <unistd.h>	//close
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "lcd_ui.h"
#include "bmp.h"

//编译命令
// arm-linux-gcc test.c -o test -lm -L. -ljpeg -static
// gcc test.c -o test -lm -L. -ljpeg

int main(int argc , char *argv[])
{
	LcdDevice* lcd = create_lcd("/dev/ubuntu_lcd");
	//LcdDevice* lcd = create_lcd("/dev/fb0");
	if(lcd == NULL) exit(1);//为空指针表示没有创建成功

	//触摸屏 打开设备/dev/input/event0
	//int touch_fd = open("/dev/input/event0", O_RDWR);
	int touch_fd = open("/dev/ubuntu_event", O_RDWR);
	if(touch_fd < 0)
	{
		perror("open fail");
		exit(1);
	}
	//clear_lcd(lcd, 0xffffff);//把屏幕设置白色背景
	clear_lcd(lcd, 0x000000);//黑色
	
	bitBmp* bmp = create_bitBmp_file("./1.bmp");
	draw_bmp(lcd,0,0,bmp);
	destroy_bitBmp(bmp);

	destroy_lcd(lcd);
	close(touch_fd);
	
	return 0;
}