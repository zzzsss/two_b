/****************************************************
*Ѱ��·�������е�ȫ�ֱ���ȫ����װ�������ռ�path����Ե��õ�ʱ��Ҫ����
*�����ռ�������Ҫ��Ҫʹ�õĺ�����ʵ��������GetPath������ʹ�÷�����
*����ͷ�ļ����ṩԭͼƬλ�ã�ϸ�����ͼ�񣩺�Ŀ���ַ��·��ͼ����λ�ã�;
*�������ƶ��ǡ�\0����β���ַ����飨ֱ��д���־����ˣ�
*��������:path::GetPath(lpszSrcPathname,lpszDstName);
*--------------------------------------------------------------------------
*�ر�ע��,����ļ��ﶨ���˺꣺Path_GETX,Path_GETY������Path_Point
*�벻Ҫ�������ط�ʹ��
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
	//=����ʵ�ֱ���ͼ�����ݽṹBwImage
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
{//��ͨ��
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
	//�ܹ�����������������ȫ����֧��ʧ���ˣ������ļһ����false
	return false;
}
/*��Ҫ�õ����������ص�����*/
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
/*ʹ�÷����ǣ���һ�����ڣ����ͼ��Ѱ��Ŀ��㣬Ŀ�����ͷ�ļ����Ѷ��壬���������ʾȷ��*/
/*���ܻ�ܸ��ӣ���û��ȷ��ֵʱ����Ӧ����0����������ֵ�ģ�����ֵʱ�����Ƶ�ǰֵ�����λ��
Ȼ�������Ҫ�Ƿ������ֵ�������������´θ�������*/
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
		{/*ˮƽɨ���Ұ׵�*/
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
			printf("��ǰ��ȡ�ĵ���:(%i,%i),�����������ʾȷ��\n",x,y);
		}
		cvShowImage(windowName,img);
		if(cvWaitKey(1)!=-1)
			break;
	}
	cvDestroyWindow(windowName);
	return result;
}

/*���Ĳ��Һ���*/
void path::FindPath(IplImage * src,IplImage * dst,int sta_x,int sta_y,int dst_x,int dst_y)
{
	//���ͼ��
	cvCopy(src,dst);
	BwImage bw_dst(dst);
	//�ݹ���̽·��
	detect(bw_dst,sta_x,sta_y,dst_x,dst_y);
}
/*��ִ�й���*/
/*��һ��������Դ�ļ����Ŀս�β�ַ����飬�ڶ�����Ŀ���ļ���*/
void path::GetPath(char * lpszSrcPathname,char * lpszDstPathname)
{
	char * orname = "ԭʼͼ��";
	char * dsname = "�����ͼ��";
	IplImage * src = cvLoadImage(lpszSrcPathname,0);

	//plus
	zero_max(src);
	
	IplImage * dst = cvCreateImage(cvGetSize(src),8,1);
	Path_Point start = GetPath_Point(src,"��ȡ���",(CvMouseCallback)GetStartPath_Point);
	Path_Point end = GetPath_Point(src,"��ȡ�յ�",(CvMouseCallback)GetDstPath_Point);
	FindPath(src,dst,Path_GETX(start),Path_GETY(start),Path_GETX(end),Path_GETY(end));

	/*������*/
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

	/*������*/
	cvSaveImage(lpszDstPathname,dst);
	//cvDestroyAllWindows();
}

#endif
