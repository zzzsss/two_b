#include "the_tree.h"
#include "the_FindPath.h"
#include <iostream>
#include <vector>
using std::vector;
using std::cout;

int search_extend = 5;


void cvThin (IplImage* src, IplImage* dst, int iterations=1);
void getpath_mouse1(int mouseevent, int x, int y, int flags, void* param);
CvRect get_block(CvPoint a,IplImage *b,int r=10);

int has_white(IplImage *p,CvPoint c,int extend){
	CvRect r=get_block(c,p,extend);
	for(int i=r.x;i<r.x+r.width;i++)
		for(int j=r.y;j<r.y+r.height;j++)
			if(unsigned char(p->imageData[j * p->widthStep + i]) > (0xff-10))
				return 1;
	return 0;
}

bool has_plenty_white(IplImage *p,CvPoint a,CvPoint b,int extend,int numbers)
{
	int k=1;
	CvPoint temp;
	for(int i=1;i<numbers;i++){
		temp.x = i/double(numbers) * a.x + (numbers-i)/double(numbers) * b.x;
		temp.y = i/double(numbers) * a.y + (numbers-i)/double(numbers) * b.y;
		k = k * has_white(p,temp,extend);
	}
	return k;
}

bool has_no_wall(IplImage *p,CvPoint a,CvPoint b,int extend,int numbers)
{
	CvPoint temp;
	for(int i=1;i<numbers;i++){
		temp.x = i/double(numbers) * a.x + (numbers-i)/double(numbers) * b.x;
		temp.y = i/double(numbers) * a.y + (numbers-i)/double(numbers) * b.y;
		if(has_white(p,temp,extend))
			return 0;
	}
	return 1;
}
//keys:
//1.cvGoodFeaturesToTrack的数量和最小距离
//2.合并时的角度
void figure_out_p_path(IplImage * p,the_tree * t,IplImage *help)
{
	IplImage * p_copy_1 = cvCreateImage(cvSize(p->width,p->height),8,1);
	cvCopyImage(p,p_copy_1);
	cvInRangeS(p_copy_1,CV_RGB(0,0,0),CV_RGB(128,128,128),p);
	cvReleaseImage(&p_copy_1);
	//cvSaveImage("image_before.jpg",p);

	for(int j=0;j<100;++j){  //细化
		cvThin(p,p);
	}

	cvSaveImage("image_fat.jpg",p);
	cvShowImage("win3",p);
	cout << "细化路径，any key to continue...\n";
	cvWaitKey();

	path::GetPath("image_fat.jpg","image_thin.jpg");

	p = cvLoadImage("image_thin.jpg",0);


	IplImage * p_copy = cvCreateImage(cvSize(p->width,p->height),8,1);
	cvCopyImage(p,p_copy);
	
	IplImage * temp1=cvCreateImage(cvSize(p->width,p->height),IPL_DEPTH_32F,1);
	IplImage * temp2=cvCreateImage(cvSize(p->width,p->height),IPL_DEPTH_32F,1);

	int corner_num = 40;
	int min_distance=20;
	CvPoint2D32f* corners = new CvPoint2D32f[corner_num];
	cvGoodFeaturesToTrack(p,temp1,temp2,corners,&corner_num,0.01,min_distance);

	vector<CvPoint> point_v;

	for(int i=0;i<corner_num;i++){
		cvCircle(p,cvPoint(corners[i].x,corners[i].y),10,cvScalarAll(128));
		point_v.push_back(cvPoint(corners[i].x,corners[i].y));
	}

	//trackerbar to adjust
	cout << "Find corners,Space to continue...\n";
	cvCreateTrackbar("corner_num","win3",&corner_num,50);
	cvCreateTrackbar("min_distance","win3",&min_distance,100);
	while(1){
		cvShowImage("win3", p);
		cout << "Enter to get another,Space to quit...\n";
		char a=cvWaitKey();
		if(a==' ')
			break;

		cvCopyImage(p_copy,p);
		delete []corners;
		int corner_num_temp = corner_num;
		corners = new CvPoint2D32f[corner_num];
		cvGoodFeaturesToTrack(p,temp1,temp2,corners,&corner_num_temp,0.01,min_distance);

		point_v.clear();
		for(int i=0;i<corner_num_temp;i++){
			cvCircle(p,cvPoint(corners[i].x,corners[i].y),10,cvScalarAll(128));
			point_v.push_back(cvPoint(corners[i].x,corners[i].y));
		}
	}
	
	//定义起点
	CvPoint begin;
	CvPoint end;
	cout << "左键单击，定义起点(win3)...\n";
	cvSetMouseCallback("win3",getpath_mouse1,&begin);
	cvWaitKey();
	cout << "左键单击，定义终点(win3)...\n";
	cvSetMouseCallback("win3",getpath_mouse1,&end);
	cvWaitKey();

	//corners顺序
	vector<CvPoint> point_order;
	while(!point_v.empty()){
		vector<CvPoint>::iterator max=point_v.end();
		for(vector<CvPoint>::iterator n=point_v.begin();n!=point_v.end();n++)
			if(has_no_wall(help,begin,*n,search_extend,1000))
				if(max == point_v.end() || distance(begin,*n)<=distance(begin,*max))
						max = n;
		point_order.push_back(*max);
		begin = *max;
		point_v.erase(max);
	}
	
	//合并corners
	//at least two points in point_order
	vector<CvPoint> point_few;
	CvPoint temp;
	CvPoint v0 = point_order.back();
	point_order.pop_back();
	CvPoint v1 = point_order.back();
	point_order.pop_back();
	point_few.push_back(v0);
	point_few.push_back(v1);
	while(!point_order.empty()){
		temp=point_order.back();
		point_order.pop_back();
		if(abs(get_vector_angle(v1,v0,temp,v1)) < 10){
			point_few.pop_back();
			point_few.push_back(temp);
			v1 = temp;
		}
		else{
			v0=v1;
			v1=temp;
			point_few.push_back(temp);
		}
	}

	vector<CvPoint> point_finish;
	int the_few_num = point_few.size();
	for(int i=0;i<the_few_num;i++){
		point_finish.push_back(point_few.back());
		point_few.pop_back();
	}

	//再次进一步合并
	for(vector<CvPoint>::iterator m=point_finish.begin();;){
		if(m==point_finish.end())
			break;
		m++;

		vector<CvPoint>::iterator temp = m;
		m--;
		if(temp==point_finish.end())
			break;
		if(is_near_point(*m,*temp,30)){
			m->x = (m->x+temp->x)/2;
			m->y = (m->y+temp->y)/2;
			temp=point_finish.erase(temp);
		}
		else
			m++;
	}
	
	//存入树中
	t->insert(end,-1);
	for(vector<CvPoint>::iterator m=point_finish.end()-1;m!=point_finish.begin();m--){
		t->insert(*m,-1);
	}
	t->insert(point_finish.front(),-1);
}

void show_path_point(the_node * n,IplImage * p)
{
	cvCircle(p,n->position,5,cvScalarAll(0xff));
	cvShowImage("win4",p);
}

void show_path_b_point(the_node * n,IplImage * p)
{
	cvCircle(p,n->position,5,cvScalarAll(0));
	//cvShowImage("win4",p);
}

