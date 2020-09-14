#define ROW 5
#define COL 5                          //ROW、COL分别为数组行、列元素的个数
#include<stdio.h>
#include<stdlib.h>
void init(int arr[][COL], int row, int col)
{
	int i = 0;
	int j = 0;                         //遍历数组时的循环变量
	for (i = 0; i < ROW; i++)
	{
		printf("请输入%d*%d数组第%d行元素，单个输入:>", ROW, COL, i + 1);
		for (j = 0; j < COL; j++)
		{
			scanf("%d", &arr[i][j]);
		}
	}
}
void print(int arr[][COL], int row, int col)
{
	int i = 0;
	int j = 0;  	//遍历数组时的循环变量
	printf("输入的数组如下：\n");
	for (i = 0; i < ROW; i++)
	{
		for (j = 0; j < COL; j++)
		{
			printf("%-4d", arr[i][j]);
		}
		printf("\n");                  //打印一行换行
	}
}

int main()
{
	int arr[ROW][COL] = { 0 }; 
	init(arr, ROW, COL);                 //初始化数组
	print(arr, ROW, COL);                //打印数组
	saddle(arr, ROW, COL);               //找鞍点
	system("pause");
	return 0;
}