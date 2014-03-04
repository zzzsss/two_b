/****************************************************
*寻找路径过程中的全局变量全部封装在命名空间path里，所以调用的时候要加上
*命名空间名。主要需要使用的函数是实现在最后的GetPath函数，使用方法，
*包含头文件后，提供原图片位置（细化后的图像）和目标地址（路径图存贮位置）;
*两个名称都是‘\0’结尾的字符数组（直接写名字就行了）
*调用如下:path::GetPath(lpszSrcPathname,lpszDstName);
*--------------------------------------------------------------------------
*特别注意,这个文件里定义了宏：Path_GETX,Path_GETY，类型Path_Point
*请不要在其它地方使用
****************************************************/
#include "cv.h"
#include "highgui.h"
#ifndef FINDPATH_H_
#define FINDPATH_H_
#include<stdio.h>
#include<iostream>
#define Path_GETX(p) (p/CAP)
#define Path_GETY(p) (p%CAP)

typedef int Path_Point;

namespace path
{
	//======================
	//=用于实现遍历图的数据结构BwImage
	//======================
	struct BwImage
	{
		IplImage * img;
		BwImage(IplImage * i)
		{img = i;}
		~BwImage(){}
		uchar * operator[](int row)
		{
			uchar * data = (uchar *)img->imageData;
			return data+row*img->widthStep;
		}
	};
	//---------------------------------------------
	const int CAP = 10000;
	Path_Point STARTPath_Point = 0;
	Path_Point DSTPath_Point = 0;
	const int FIND[8][2] = {{-1, 0}, {-1, 1}, {0, 1}, {1, 1},{1, 0}, {1, -1}, {0, -1}, {-1, -1}};
	const int SUCCESS = 100;
	const int FAILURE = 200;
	//FUNCTIONS
	bool	detect(BwImage img,int x,int y,int dst_x,int dst_y);
	void	GetStartPath_Point(int iEvent,int x,int y,int flags,int parm);
	void	GetDstPath_Point(int iEvent,int x,int y,int flags,int parm);
	Path_Point	GetPath_Point(IplImage * img,char * windowName,CvMouseCallback CALLBACK);
	void	FindPath(IplImage * src,IplImage * dst,int sta_x,int sta_y,int dst_x,int dst_y);
	void GetPath(char * lpszSrcPathname,char * lpszDstPathname);
	void zero_max(IplImage *p);
};

//plus
void path::zero_max(IplImage *p)
{//单通道
	for(int i=0;i<p->width;i++)
		for(int j=0;j<p->height;j++){
			if((unsigned char(p->imageData[j * p->widthStep + i]))>128)
				p->imageData[j * p->widthStep + i]=255;
		}
	for(int i=0;i<p->width;i++)
		for(int j=0;j<p->height;j++){
			if((unsigned char(p->imageData[j * p->widthStep + i]))>128 && (unsigned char(p->imageData[j * p->widthStep + i]))!=255)
				std::cout << i << j << '\n';
		}
}


bool path::detect(BwImage img,int x,int y,int dst_x,int dst_y)
{
	if(x==dst_x&&y==dst_y)
		return true;
	bool cango = false;
	int a,b;
	for(int i=0;i<8;i++)
	{
		a = x + FIND[i][0];
		b = y + FIND[i][1];
		if(img[b][a]==255)
			{cango=true;break;}
	}
	if(!cango)
		return false;
	for(int i=0;i<8;i++)
	{
		a = x + FIND[i][0];
		b = y + FIND[i][1];
		if(img[b][a]==255)
		{
			img[b][a] =	SUCCESS;
			if(detect(img,a,b,dst_x,dst_y))
				return true;
			else img[b][a] = FAILURE;
		}
	}
	//能够遍历结束的条件是全部分支都失败了，这样的家伙就是false
	return false;
}
/*需要用到的两个鼠标回调函数*/
void path::GetStartPath_Point(int iEvent,int x,int y,int flags,int parm)
{
	if(iEvent==CV_EVENT_LBUTTONDOWN)
	{
		STARTPath_Point = x*CAP+y;
	}
}
void path::GetDstPath_Point(int iEvent,int x,int y,int flags,int parm)
{
	if(iEvent==CV_EVENT_LBUTTONDOWN)
	{
		DSTPath_Point = x*CAP+y;
	}
}
/*使用方法是：打开一个窗口，点击图像寻找目标点，目标点在头文件中已定义，按任意键表示确定*/
/*可能会很复杂：当没有确定值时，相应点是0，否则是有值的，当有值时，复制当前值，清空位置
然后根据需要是否保留这个值，不保留，则下次更新它。*/
Path_Point path::GetPath_Point(IplImage * img,char * windowName,CvMouseCallback the_CALLBACK)
{
	BwImage BwImg(img);
	Path_Point result;
	int x,y;
	Path_Point & cur = the_CALLBACK==(CvMouseCallback)GetStartPath_Point?

STARTPath_Point:DSTPath_Point;
	cvNamedWindow(windowName);
	cvSetMouseCallback(windowName,the_CALLBACK);
	while(true)
	{
		if(cur)
		{/*水平扫描找白点*/
			x = Path_GETX(cur);y = Path_GETY(cur);
			for(int i=x;i<img->width;i++)
			{
				if(BwImg[y][i]==255)
				{
					x = i;break;
				}
			}
			cur = 0;
			result = x*CAP+y;
			printf("当前获取的点是:(%i,%i),按下任意键表示确认\n",x,y);
		}
		cvShowImage(windowName,img);
		if(cvWaitKey(1)!=-1)
			break;
	}
	cvDestroyWindow(windowName);
	return result;
}

/*最后的查找函数*/
void path::FindPath(IplImage * src,IplImage * dst,int sta_x,int sta_y,int dst_x,int dst_y)
{
	//填充图像
	cvCopy(src,dst);
	BwImage bw_dst(dst);
	//递归深探路径
	detect(bw_dst,sta_x,sta_y,dst_x,dst_y);
}
/*主执行过程*/
/*第一个参数是源文件名的空结尾字符数组，第二个是目标文件的*/
void path::GetPath(char * lpszSrcPathname,char * lpszDstPathname)
{
	char * orname = "原始图像";
	char * dsname = "处理后图像";
	IplImage * src = cvLoadImage(lpszSrcPathname,0);

	//plus
	zero_max(src);
	
	IplImage * dst = cvCreateImage(cvGetSize(src),8,1);
	Path_Point start = GetPath_Point(src,"获取起点",(CvMouseCallback)GetStartPath_Point);
	Path_Point end = GetPath_Point(src,"获取终点",(CvMouseCallback)GetDstPath_Point);
	FindPath(src,dst,Path_GETX(start),Path_GETY(start),Path_GETX(end),Path_GETY(end));

	/*处理结果*/
	BwImage BwDst(dst);
	for(int i=0;i<dst->height;i++)
	{
		for(int j=0;j<dst->widthStep;j++)
		{
			if(BwDst[i][j]==SUCCESS)
			{
				BwDst[i][j] = 255;
				continue;
			}
			BwDst[i][j] = 0;
		}
	}
	
	cvNamedWindow("win7");
	cvShowImage("win7",dst);
	cvWaitKey();

	/*保存结果*/
	cvSaveImage(lpszDstPathname,dst);
	//cvDestroyAllWindows();
}

#endif
