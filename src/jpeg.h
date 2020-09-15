
#ifndef _JPEG_H
#define _JPEG_H

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

#include "jpeglib.h"
#include "lcd_ui.h"

int jpeg_show(LcdDevice *lcd,const unsigned char *filename)
{
	/*
	��.�����䲢��ʼ��һ��jpeg��ѹ����
	*/
	struct jpeg_decompress_struct dinfo; //������һ��jpeg�Ľ�ѹ����
	
	struct jpeg_error_mgr jerr; //����һ���������
	dinfo.err = jpeg_std_error(&jerr);
		
	jpeg_create_decompress(&dinfo); //��ʼ�������ѹ����

	/*
	2. ָ��Ҫ��ѹ����ͼ���ļ�
	
	*/
	FILE *infile;
	infile = fopen(filename, "r");
	if (infile == NULL)
	{
		perror("fopen error");
		return -1;
	}

	jpeg_stdio_src(&dinfo, infile); //Ϊ��ѹ����dinfoָ��Ҫ��ѹ���ļ�

	/*
	3. ����jpeg_read_header()��ȡͼ����Ϣ

	*/
	jpeg_read_header(&dinfo, TRUE);

	/*
	4. ����jpeg��ѹ������dinfo��һЩ�������ɲ���Ĭ�ϲ���
	*/


	/*
	5.����jpeg_start_decompress()������ѹ����
	jpeg_start_decompress(&dinfo); 
	����jpeg_start_decompress(&dinfo);����֮��JPEG��ѹ����dinfo�������⼸����Ա����
	����Ƚ����ã�
	output_width:  ͼ���������
	output_height����ͼ�������߶�
	output_component����ÿ�����صķ�������Ҳ���ֽ�����3/4�ֽ�
		�ڵ���jpeg_start_decompress(&dinfo); ֮��������ҪΪ��ѹ���ɨ�����ϵ��������ص����洢�ռ䣬
	output_width * output_components(һ�е��ֽ�����output_height��)

	*/

	jpeg_start_decompress(&dinfo); 


	unsigned char *buffer = malloc(dinfo.output_width * dinfo.output_components);

	/*
	��.����ȡһ�л��߶���ɨ�������ݲ�����ͨ���Ĵ����������ģ�
	
	//output_scanline��ʾɨ���������
	//output_height��ʾͼ���������
	while (dinfo.output_scanline < dinfo.output_height)
	{
		jpeg_read_scanlines(&dinfo, &buffer, 1);

		//deal with scanlines . RGB/ARGB
	}
	*/

	//output_scanline��ʾɨ���������
	//output_height��ʾͼ���������
	while (dinfo.output_scanline < dinfo.output_height)
	{
		jpeg_read_scanlines(&dinfo, &buffer, 1); //dinfo.output_scanline + 1

		//deal with scanlines . RGB/ARGB
		int x; //һ�е����ص�����

		char *p = buffer;
		for (x = 0; x < dinfo.output_width; x++)
		{
			unsigned char r, g, b, a = 0;
			int color;
			if (dinfo.output_components == 3)
			{
				r = *p++;
				g = *p++;
				b = *p++;
			} else if (dinfo.output_components == 4)
			{
				a = *p++;
				r = *p++;
				g = *p++;
				b = *p++;
			}
			color = (a << 24) | (r << 16) | (g << 8) |(b) ;


			draw_point(lcd,dinfo.output_scanline - 1 ,x,  color); 
				
		}
	
	}

	/*
	7. ���á�jpeg_finish_decompress()��ɽ�ѹ����
	*/
	jpeg_finish_decompress(&dinfo);



	/*
	��.����jpeg_destroy_decompress()�ͷ�jpeg��ѹ����dinfo
	jpeg_destroy_decompress(&dinfo);
	
	
	*/
	jpeg_destroy_decompress(&dinfo);
	free(buffer);
	fclose(infile);
	
	return 0;
}


#endif
