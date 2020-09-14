#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#include <string.h>
#include <jpeglib.h>



int lcd_fd;
int pic_fd;
int *plcd = NULL;
char* bmp_name[] = {"x.bmp","f.bmp","t.bmp","ShouYunMusb.jpg"};
struct point{
	int x;
	int y;
};




int get_tts(struct point *p)
{
	//第一步：打开屏幕 
	int ts_fd = open("/dev/input/event0",O_RDWR);
	if(ts_fd < 0)
	{
		perror("open ts_lcd error");
		return -1;
	}
	
	//第二步：读取触摸屏操作
	
	p->x = p->y = -1; //即此时传递过来一个参数  p是一个结构体的指针 即 一个结构体的首地址 
					  //这个地址中有两个成员分别是 x 和 y 刚开始没有获取到操作坐标的时候应该为 -1 -1 
					  // 为什么不是 0 0  在屏幕上 0 0 是一个存在的点 就是最左上角的点 
	struct input_event data;
	memset(&data,0,sizeof(data)); //将这个存储单元中的所有内容全部清为 0 
	
	while(1)
	{
		int ret = read(ts_fd,&data,sizeof(data));
		if(ret < sizeof(data))
		{
			printf("read data failed\n");
			continue;
		}
		if(data.type == EV_ABS)
		{
			if(data.code == ABS_X)
			{
				//此时读取到的是 X轴坐标 这个值在value中  但是你需要赋值给 x 
				if(p->x < 0 && p->y < 0)
					p->x = data.value;
			}
			else if(data.code == ABS_Y)
			{
				///此时读取到的是 Y轴坐标 这个值在value中  但是你需要赋值给 y 
				if(p->x >=0 && p->y< 0)
				{
					p->y = data.value;
					break;
				}
			}
		
		}
	}
	printf("x = %d\n,y = %d\n",p->x,p->y);
}




int lcd_init(void)
{
	//第一：打开屏幕 
	lcd_fd = open("/dev/fb0",O_RDWR);
	if(lcd_fd < 0)
	{
		perror("open lcd error");
		return -1;
	}
	
	//第二： 映射屏幕 
	
	plcd = (int*)mmap(NULL,800*480*4,PROT_READ|PROT_WRITE,MAP_SHARED,lcd_fd,0);
	if(plcd == NULL)
	{
		perror("mmap lcd error");
		close(lcd_fd);
		return -1;
	}
	return 0;
}


int lcd_uninit(void)
{
	//第一解除映射 
	int ret = munmap(plcd,800*480*4);
	if(ret < 0)
	{
		perror("munmap error");
		return -1;
	}
	ret = close(lcd_fd);
	if(ret < 0)
	{
		perror("close lcd error");
		return -1;
	}
	return 0;
}




void lcd_draw_point(int x,int y,int color)
{
	*(plcd+x*800+y) = color; //画点
}


void LCD_DrawPoint(unsigned long x, unsigned long y, int color)
{
	int * p = (int *)plcd;

	*(p + y*800 + x) = color;
}


int show_bmp(char *pathname,int x,int y,int width,int high)
{
	pic_fd = open(pathname,O_RDWR);
	if(pic_fd < 0)
	{
		perror("open pic errir");
		return -1;                //打开图片
	}
	
	
	unsigned char pic_buf[480*800*3] = {0};
		int lret = lseek(pic_fd,54,SEEK_SET);
		if(lret < 0)
		{
			perror("lseek error");
			return -1;
		}
		int ret = read(pic_fd,pic_buf,width*high*3);
		if(ret < 0)
		{
			perror("read pic error");
			return -1;         //读取图片
		}
	
	unsigned int a,r,g,b,color;
	int i,j; 
	unsigned char *p = pic_buf; 
	for(i = x;i<width+x;i++)
	{
		for(j = y;j<high+y;j++)
		{
			a = 0x00;
			b = *p++; 
			g = *p++;
			r = *p++;
			color = a<<24 | r<<16 | g<<8 | b;
			lcd_draw_point(479-i,j,color); //将图片投到屏幕 
		}
	}
	
}



void lcd_draw_block(int x0,int y0,int width,int high,int color)
{
	//如何画这一块 首先起点坐标确定 即  i j的初始值知道 
	
	int i,j;
	for(i=x0;i< x0+high ;i++)
	{
		for(j=y0;j< y0+width;j++)
		{
			lcd_draw_point(i,j,color);
		}
	}
}


int show_jpg(char *ptr)
{
	struct jpeg_decompress_struct dinfo; 
	struct jpeg_error_mgr jerr;
	dinfo.err = jpeg_std_error(&jerr);
	jpeg_create_decompress(&dinfo); //初始化这个解压对象
	
	FILE *infile;
	infile = fopen(ptr, "r");
	if (infile == NULL)
	{
		perror("fopen error");
		return -1;
	}
	
	jpeg_stdio_src(&dinfo, infile); //为解压对象dinfo指定要解压的文件
	
	//调用jpeg_read_header()获取图像信息 宽和高之类的
	jpeg_read_header(&dinfo, TRUE);
	
	jpeg_start_decompress(&dinfo); 
	printf("width = %d output_components=%d \n",dinfo.output_width,dinfo.output_components);
	unsigned char *buffer = (unsigned char *)malloc(800*3);//(dinfo.output_width * dinfo.output_components);
	memset(buffer,0,sizeof(buffer));
	
	while (dinfo.output_scanline < dinfo.output_height)
	{
		jpeg_read_scanlines(&dinfo, &buffer, 1); //dinfo.output_scanline + 1

		//deal with scanlines . RGB/ARGB
		int x; //一行的像素点数量

		unsigned char *p = buffer;
		for (x = 0; x < dinfo.output_width; x++)
		{
			unsigned char r, g, b, a = 0;
			int color;
			
			r = *p++;
			g = *p++;
			b = *p++;
			
			color = (a << 24) | (r << 16) | (g << 8) |(b) ;


			LCD_DrawPoint(x, dinfo.output_scanline - 1 ,  color); 
			
			
		}
	}
	jpeg_finish_decompress(&dinfo);
	jpeg_destroy_decompress(&dinfo);
	free(buffer);
	fclose(infile);
	
	return 0;
}


int main()
{
	int i = 0;
	int j = 0;
	int temp;
	int panduan=0;
	char temp_arr[20]={0};
	struct point start;
	lcd_init();
	lcd_draw_block(0,0,800,480,0x00FFFFFF);
	
	double num = 0.6;
	double nnum = -num;
	while(1)
	{
		get_tts(&start);
		if((start.y < nnum*start.x && start.y>nnum*start.x+480) || (start.y < num*start.x && start.y>nnum*start.x+480))
		{
			i++;
		}
		if((start.y > num*start.x && start.y<nnum*start.x+480) || (start.y > num*start.x && start.y<nnum*start.x+480))
		{
			i--;
		}
		if(i<0)
		i = 2;
		if(i>3)
		i=0;
		//temp_arr[20] = bmp_name[i];
		strcpy(temp_arr,bmp_name[i]);
		temp = strlen(bmp_name[i])-3;
		printf("%s\n",temp_arr);
		printf("temp = %d\n",temp);
		
		
		if(strncmp(&temp_arr[temp],"jpg",3) == 0)
			panduan = 1;
		else
			panduan = 0;
		
		if(panduan)
		{
			show_jpg(bmp_name[i]);
		}
		else{
			printf("panduan = %d\n",panduan);
			show_bmp(bmp_name[i],0,0,480,800);
		}
		memset(temp_arr,0,20);
		start.x=-1;
		start.y=-1;
			
	}


	
	close(pic_fd);
	
	//第七步：解除初始化 
	lcd_uninit();
	return 0;
}

























































	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	