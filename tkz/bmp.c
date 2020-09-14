#include "bmp.h"

bitBmp* create_bitBmp_file(const char *filename)
{
	FILE *file = fopen(filename, "r");
	if(file == NULL)
	{
		perror("open fail");
		return NULL;
	}
	//读取图片头
	BitMapFileHeader fileHeader;
	fread(&fileHeader, 1, sizeof(fileHeader), file); //读取图片头
	char *tmp = (char *)&fileHeader.bfType; //bmp图片标志位
	if(tmp[0] != 'B' || tmp[1] != 'M') //判断是否是位图
	{
		return NULL;
	}
	
	//创建一个位图数据（在内存中）
	bitBmp* bmp = malloc(sizeof(bitBmp)); //分配内存空间
	bmp->bw = fileHeader.biWidth;           //
	bmp->bh = fileHeader.biHeight;          //
	bmp->pertype = 4; //一个像素有四个字节
	//申请空间存储图像数据
	bmp->startBmp = malloc(bmp->bw*bmp->bh*bmp->pertype); //比如480*800*4
	
	//从文件中读取数据
	unsigned int size = bmp->bw*bmp->bh*fileHeader.biBitCount/8;
	char buffer[size]; //保存图片数据
	fread(buffer, size, 1, file);
	
	unsigned int rowsize = bmp->bw*fileHeader.biBitCount/8;
	//测试点
	printf("%d-%d-%d\n",bmp->bw,bmp->bh,fileHeader.biBitCount/8);
	
	if(fileHeader.biBitCount/8 == bmp->pertype)
	{
		for(int i=0; i< bmp->bh; i++)
		{
			memcpy(bmp->startBmp+i*bmp->bw, buffer+(bmp->bh-1-i)*rowsize, rowsize);
		}
	}else if(fileHeader.biBitCount/8 == 3)
	{
		for(int i=0; i<bmp->bh; i++)
		{
			for(int j=0; j<bmp->bw; j++)
			{
				memcpy(bmp->startBmp+i*bmp->bw+j,  buffer+(bmp->bh-1-i)*rowsize+j*3, 3);
			}
		}
	}
	return bmp;
}

bitBmp* create_bitBmp_defaut(int w, int h)
{
	//创建一个位图数据
	bitBmp* bmp = malloc(sizeof(bitBmp));
	bmp->bw = w;           //
	bmp->bh = h;          //
	bmp->pertype = 4;
	//申请空间存储图像数据
	bmp->startBmp = malloc(bmp->bw*bmp->bh*bmp->pertype);
	memset(bmp->startBmp,0,bmp->bw*bmp->bh*bmp->pertype );
	return bmp;
}
/*
bitBmp* zoom_bitBmp(bitBmp* bmp, int wscale, int hscale)
{
	bitBmp* tmp = (bitBmp*)malloc(sizeof(bitBmp));
	tmp->bw = wscale;
	tmp->bh = hscale;
	tmp->pertype = 4;
	tmp->startBmp = (unsigned int *)malloc(tmp->bw*tmp->bh*tmp->pertype);
	
	int wsc = bmp->bw/(double)wscale*100;
	int hsc = bmp->bh/(double)hscale*100;
	printf("%d:%d\n", wsc, hsc);
	
	for(int j=0; j<tmp->bh; j++)
	{
		for(int i=0; i<tmp->bw; i++)
		{
			double number = (i*wsc)/100+bmp->bw*((j*hsc)/100);
			tmp->startBmp[i+j*tmp->bw] =  bmp->startBmp[(int)number];
		}
	}
	destroy_bitBmp(bmp);
	return tmp;
}*/
bool destroy_bitBmp(bitBmp* bmp)
{
	if(bmp == NULL) return false;
	if(bmp->startBmp != NULL) free(bmp->startBmp);
	free(bmp);
	return true;
}

