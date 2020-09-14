#include <sys/types.h>	//open
#include <sys/stat.h>	//open
#include <fcntl.h> //open
#include <unistd.h>	//close
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "lcd_ui.h"
//#include "bmp.h"
#include "jpeg.h"
#include "my_name.h"
#include "my_number.h"
#include "my_word.h"

//编译命令
// arm-linux-gcc test.c -o test -lm -L. -ljpeg -static
// gcc test.c -o test -lm -L. -ljpeg

int main(int argc , char **argv)
{
	LcdDevice* lcd = create_lcd("/dev/ubuntu_lcd");
	//LcdDevice* lcd = create_lcd("/dev/fb0");
	if(lcd == NULL) exit(1);//为空指针表示没有创建成功

	//触摸屏 打开设备/dev/input/event0
	//int fd = open("/dev/input/event0", O_RDWR);
	int fd = open("/dev/ubuntu_event", O_RDWR);
	if(fd < 0)
	{
		perror("open fail");
		exit(1);
	}
	//clear_lcd(lcd, 0xffffff);//把屏幕设置白色背景
	clear_lcd(lcd, 0x000000);//黑色
	
	int j;
	for(j=0;j<7;j++)
		draw_word(lcd,200,200+j*56,56,56*56/8,hello[j],0x00FF00F0);
	for(j=0;j<3;j++)
		draw_word(lcd,50,200+j*56,56,56*56/8,my_name[j],0xff0000);
	sleep(2);
	draw_bmp(lcd,800,480,"./hello.bmp");
	draw_button(lcd);
	
	//测试点
	//jpeg_show(lcd,"./1.jpg");
	//draw_bmp(lcd,800,480,"./1.bmp");
	//draw_bmp(lcd,800,480,"./2.bmp");
	//draw_bmp(lcd,800,480,"./3.bmp");
	//draw_block(lcd,100,0,100,150,0x00ff00);
	//draw_block(lcd,100,700,100,150,0x00ff00);
	//draw_line(lcd,100,0,100,'0',0x00ff00);
	//draw_linex(lcd,200,0,100,0x00ff00);
	//draw_lineY(lcd,200,0,100,0x00ff00);
	//draw_bmp(lcd,800,480,"./2.bmp");
	//draw_box(lcd,100,400,300,100,0x00ff00);
	//draw_circle(lcd,240,100,0xff0000);
	//创建bmp图片数据
	//bitBmp *bmp = create_bitBmp_file("./2.bmp");
	//bitBmp *bmp = create_bitBmp_defaut(480,800);
	//draw_bmp(lcd, 480,800, bmp);
	//destroy_bitBmp(bmp);

	int index = 0;
	char* jpg_name[] = {"./1.jpg","./2.jpg","./3.jpg"};
	char* bmp_name[] = {"./1.bmp","./2.bmp","./3.bmp"};
	
	int x,y;
	while(1)
	{
		get_xy(fd , &x, &y);
		if(x>0 && x<100 && y>100 && y<250)
		{
			if(index >0)
			{		
				index--;
			}
		}else if(x>700 && x<800 && y>100 && y<250)	
		{
			if(index < 5)
			{	
				index++;
			}
		}else if(x>750 && x<800 && y>0 && y<50)
		{
			jpeg_show(lcd,"./bye.jpg");
			sleep(1);
			break;
		}
		if(index>=0 && index <=2)
			jpeg_show(lcd,jpg_name[index]);
		else if(index>=3 && index<=5)
			draw_bmp(lcd,800,480,bmp_name[index-3]);	
		draw_button(lcd);
		draw_circle(lcd,25,750,25,0x0000ff);
	}
	
	destroy_lcd(lcd);
	close(fd);
	
	return 0;
}