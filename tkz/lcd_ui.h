
#ifndef _LCD_UI_H
#define _LCD_UI_H

#include <sys/types.h>	//open
#include <sys/stat.h>	//open
#include <fcntl.h>	//open
#include <sys/mman.h> //内存映射
#include <unistd.h>	//close,read,wrute,lseek
#include <stdbool.h>	//bool类型
#include <linux/input.h>	//触摸，鼠标，按键
#include <stdio.h>	//
#include <stdlib.h>	//
#include <math.h> //sqrt,

#include "bmp.h" //bmp图片处理，创建bmp图片数据

//设计一个lcd结构体
typedef struct LcdDevice{
	int fd;				//打开的显示屏设备
	unsigned int *mmp;	//映射的内存指针
	int lw, lh;			//宽，高
	int pertype;		//一个像素点占用的字节数
}LcdDevice;

//初始化lcd设备
LcdDevice *create_lcd(const char *devname)
{
	//lcd结构体类型
	LcdDevice *lcd = malloc(sizeof(LcdDevice));
	//打开设备
	lcd->fd = open(devname, O_RDWR);
	if(lcd->fd  < 0)
	{
		perror("open fail");
		free(lcd);
		return NULL;
	}
	//映射空间 NULL，默认让系统自动分配 映射到内存空间的文件内容的长度 
	//具有可读可写权限 映射共享这块空间对于别的进程来说是可见的
	lcd->mmp = (unsigned int *)mmap(NULL, 800*480*4, PROT_READ|PROT_WRITE,
			MAP_SHARED, lcd->fd, 0);//文件描述符和偏移量
	if(lcd->mmp ==  (void *)-1)
	{
		perror("mmap fail");
		close(lcd->fd);
		free(lcd);
		return NULL;
	}
	//初始lcd参数：宽， 高， 深度
	lcd->lw = 800;
	lcd->lh = 480;
	lcd->pertype = 4;
	return lcd;
}

//销毁lcd设备
bool destroy_lcd(LcdDevice *lcd)
{
	if(lcd == NULL) return false;
	//释放映射
	munmap(lcd->mmp, 800*480*4);
	//关闭设备
	close(lcd->fd);
	free(lcd);
	return true;
}
/*
//显示bmp图片,x,y为起始坐标对应h,w  x为行 Y为列
void draw_bmp(LcdDevice* lcd, int x, int y, bitBmp *bmp)
{
	unsigned int *p = lcd->mmp+y*lcd->lw+x;//xiugai
	unsigned int *pic = bmp->startBmp;
	for(int i=0; i<bmp->bh; i++)//一行一行开始
	{
		memcpy(p, pic, bmp->bw*bmp->pertype);
		p+=lcd->lw;
		pic+=bmp->bw;	
	}
}
*/
//清屏
void clear_lcd(LcdDevice *lcd, unsigned int color)
{
	unsigned int *p = lcd->mmp;
	for(int i=0; i<480*800; i++)
	{
		p[i] = color;
	}
}

//画点 x行 y列
void draw_point(LcdDevice *lcd,int x,int y,unsigned int color)
{
	unsigned int *p = lcd->mmp;
	*(p+x*lcd->lw+y) = color; //画点
}


// 画块 起始坐标即 x0行,y0列 宽，高
void draw_block(LcdDevice *lcd,int x0,int y0,int width,int high,unsigned int color){
	//如何画这一块 首先起点坐标确定 即  i j的初始值知道
	int i,j;
	for(i=x0;i< x0+high ;i++){
		for(j=y0;j < y0+width;j++){
			draw_point(lcd,i,j,color);
		}
	}
}
//画线 起始坐标 行 列 长度 横竖标志"0"|"1"
void draw_line(LcdDevice *lcd,int x,int y,int L,char f,unsigned int color){
	int i,j;
	if(f == '0'){		
		for(j=y;j<(y+L);j++)
			draw_point(lcd,x,j,color);
	}
	else if(f == '1'){
		for(i=x;i<(x+L);i++)
			draw_point(lcd,i,y,color);
	}
}
//画横线 起始坐标 行 列 长度
void draw_lineX(LcdDevice *lcd,int x,int y,int L,unsigned int color){
	int j;	
	for(j=y;j<(y+L);j++)
		draw_point(lcd,x,j,color);
}
//画竖线 起始坐标 行 列 长度
void draw_lineY(LcdDevice *lcd,int x,int y,int L,unsigned int color){
	int i;		
	for(i=x;i<(x+L);i++)
		draw_point(lcd,i,y,color);
}

//画框 起始坐标 x行 y列 宽 高 
void draw_box(LcdDevice *lcd,int x,int y,int w,int h,unsigned int color){
	draw_lineX(lcd,x,y,w,color);
	draw_lineX(lcd,x+h,y,w,color);
	
	draw_lineY(lcd,x,y,h,color);
	draw_lineY(lcd,x,y+w,h,color);
}

//画圆 圆心 x，y 半径 r
void draw_circle(LcdDevice *lcd,int x,int y,int r,unsigned int color){
	int i,j;
	int d; //圆上的点到圆心的距离
	for(i=0;i<480;i++) 			// 第0行到 第479行 x
	{
		for(j=0;j<800;j++)		//列 y
		{	
			d = (int)sqrt((i-x)*(i-x)+(j-y)*(j-y));				
			if((d >= r) && (d <= r))
				draw_point(lcd,i,j,color); 
		}
	}
}
void draw_button(LcdDevice *lcd){
	draw_box(lcd,100,0,100,150,0x00ff00);
	draw_box(lcd,100,700,100,150,0x00ff00);
	draw_circle(lcd,240,50,50,0x00ff00);
	draw_circle(lcd,240,750,50,0x00ff00);
	
}

//显示文字
void draw_word(LcdDevice *lcd,int x0,int y0,int width,int size, char word_name[],unsigned color){
	//第一步：算出每一行是几个字节  怎么算？
	int flag = width/8; 

	//第二步： 遍历每一个字节 即显示数组中的每一个字节 
	int n = 0;
	for(n = 0;n<size;n++)
	{
		//第三步：遍历每一个字节中的每一位  一共是8位 你要考虑从哪个开始遍历
		//注意 只能从最低位开始遍历
		int i = 0;
		for(i=0;i<8;i++)
		{
			/*
			第四步： 画点 
			lcd_draw_ponit(行,列,color); //颜色就是显示文字的颜色 
			行使用的 第 n个 字节 / 每一行的字节数确定
			列使用 第n个字节 %每一行的字节数 *8 +(7-i)  i表示 第i位 
			*/
			if(word_name[n]>>i & 0x01)//如果为真则表示 该位为1 
			{
				draw_point(lcd,x0+n/flag,y0+n%flag*8+(7-i),color);
			}
			else 
			{
				//第i位为0 则表示这个点不显示
			}
		}
	}
}


//修改处
int draw_bmp(LcdDevice* lcd,int width,int high,const char *filename)
{
	//第一步：打开图片文件 
	int pic_fd = open(filename,O_RDWR);
	if(pic_fd < 0)
	{
		perror("open pic error");
		return -1;
	}
	
	//第二步：读取图片上的内容 
	unsigned char buf[800*480*3] = {0};//注意 为什么需要用无符号表示
	//显示图片的时候 将54个字节的文件头偏移走！
	//前54个字节为文件头 表示文件的格式 长 宽 大小 以及色深等信息 需要跳过 
	int ret = lseek(pic_fd,54,SEEK_SET);	
	if(ret < 0)
		{
			perror("lseek pic error");
			return -1;
		}
	ret = read(pic_fd,buf,800*480*3);
	if(ret < 0)
	{
		perror("read pic error");
		return -1;
	}
	//显示
	int i,j;
	unsigned int a,r,g,b,color;
	unsigned char *p = buf;
	for(i=(480-high)/2;i<480;i++)
	{
		for(j=(800-width)/2;j<800;j++)
		{
			a = 0x00;
			b = *p++;  // 0 3 6 9 ...
			g = *p++;  // 1 4 7 10 ...
			r = *p++;  // 2 5 8 11 ....
			
			color = a<<24|r<<16|g<<8|b;
			draw_point(lcd,479-i,j,color);

		}
	}
	return 0;
}

//获取坐标 x列 y行
void get_xy(int fd, int *x, int *y){
	*x=-1;
	*y=-1;
	//2.读取数据
	struct input_event data;
	while(1)
	{
		int size = read(fd, &data, sizeof(data));
		if(size < 0)
		{
			perror("read fail");
			exit(1);
		}
		//坐标测试
		//printf("type=%hd code=%hd value=%d\n", data.type, data.code, data.value);
		if(data.type == 3 && data.code ==0)//x坐标
		{
			*x = data.value;
		}else if(data.type == 3 && data.code ==1)//y坐标
		{	
			*y = data.value;
			if(*x >= 0)
			{
				break;
			}
		}
	}
	printf("(x,y)=(%d,%d);\n",*x,*y);
	
	//把1024*600的坐标换算800*480坐标
	//*x=(*x)*800/1024;
	//*y=(*y)*480/600;
}

#endif 