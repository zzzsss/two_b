#include"the_project.h"

//image of black_white
void set_point(CvPoint a,IplImage *b,CvScalar c,int flag)
{
	if(flag==0)	//��ͨ��
		b->imageData[a.y * b->widthStep + a.x] = c.val[0];
	else if(flag==3){	//bgr
		b->imageData[a.y * b->widthStep + a.x*3] = c.val[0];
		b->imageData[a.y * b->widthStep + a.x*3+1] = c.val[1];
		b->imageData[a.y * b->widthStep + a.x*3+2] = c.val[2];
	}
}

void set_block(CvPoint a,IplImage *b,CvScalar c,int r,int flag)
{
	int x1,y1,x2,y2;
	x1 = (a.x-r<0) ? 0 : (a.x-r);
	y1 = (a.y-r<0) ? 0 : (a.y-r);
	x2 = (a.x+r > b->width) ? b->width : (a.x+r);
	y2 = (a.y+r > b->height) ? b->height : (a.y+r);

	for(int i=x1;i<=x2;i++)
		for(int j=y1;j<y2;j++)
			set_point(cvPoint(i,j),b,c,flag);
}

CvRect get_block(CvPoint a,IplImage *b,int r)
{
	int x1,y1,x2,y2;
	x1 = (a.x-r<0) ? 0 : (a.x-r);
	y1 = (a.y-r<0) ? 0 : (a.y-r);
	x2 = (a.x+r > b->width) ? b->width : (a.x+r);
	y2 = (a.y+r > b->height) ? b->height : (a.y+r);
	return cvRect(x1,y1,x2-x1+1,y2-y1+1);
}


void mouse(int mouseevent, int x, int y, int flags, void* param)
{
	if (mouseevent == CV_EVENT_LBUTTONDOWN) //�������
	{
		vector<CvPoint2D32f> * temp = 
			reinterpret_cast<vector<CvPoint2D32f> *>(param);
		temp->push_back(cvPoint2D32f(x,y));
		cout << "trans-point-"<<x <<","<<y<<'\n';
	}
}

//maze_project
void maze_mouse1(int mouseevent, int x, int y, int flags, void* param)
{
	if (mouseevent == CV_EVENT_LBUTTONDOWN) //�������
	{
		vector<CvPoint> * temp = 
			reinterpret_cast<vector<CvPoint> *>(param);
		temp->push_back(cvPoint(x,y));
		cout << "select-point-"<<x <<","<<y<<'\n';
	}
}

void maze_mouse2(int mouseevent, int x, int y, int flags, void* param)
{
	IplImage * temp = reinterpret_cast<IplImage *>(param);
	if (mouseevent == CV_EVENT_LBUTTONDOWN) //�������
	{
		set_block(cvPoint(x,y),temp,cvScalarAll(0),7,0);
		cvShowImage("win3",temp);
	}
	else if ((mouseevent == CV_EVENT_MOUSEMOVE) && (flags & CV_EVENT_FLAG_LBUTTON)) //����϶�
	{
		set_block(cvPoint(x,y),temp,cvScalarAll(0),7,0);
		cvShowImage("win3",temp);
	}
	else if (mouseevent == CV_EVENT_RBUTTONDOWN) //�������
	{
		set_block(cvPoint(x,y),temp,cvScalarAll(0xff),7,0);
		cvShowImage("win3",temp);
	}
	else if ((mouseevent == CV_EVENT_MOUSEMOVE) && (flags & CV_EVENT_FLAG_RBUTTON)) //����϶�
	{
		set_block(cvPoint(x,y),temp,cvScalarAll(0xff),7,0);
		cvShowImage("win3",temp);
	}
}

//get_path_mouse
void getpath_mouse1(int mouseevent, int x, int y, int flags, void* param)
{
	if (mouseevent == CV_EVENT_LBUTTONDOWN) //�������
	{
		CvPoint * temp = 
			reinterpret_cast<CvPoint *>(param);
		*temp = cvPoint(x,y);
		cout << "select-begin-point-"<<x <<","<<y<<'\n';
	}
}