# arm-linux-ghx
粤嵌 arm-linux-c/c++ 开发

//编译命令
交叉编译：
// arm-linux-gcc test.c -o test -lm -L. -ljpeg -static
虚拟机上虚拟屏幕测试：
// gcc test.c -o test -lm -L. -ljpeg	

包含3个bmp图片文件和3个jpeg图片文件
	int index = 0;
	char* jpg_name[] = {"./1.jpg","./2.jpg","./3.jpg"};
	char* bmp_name[] = {"./1.bmp","./2.bmp","./3.bmp"};
  通过改变 index 的值，来调用 
  jpeg_show(lcd,dfsjpg_name[index]);
  或
  draw_bmp（lcd，800，480，bmp_name[index]);
	来显示图片





