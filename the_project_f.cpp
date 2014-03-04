//realization of the_project_functions

#include "the_project.h"

CvRect get_block(CvPoint a,IplImage *b,int r=10);
void set_block(CvPoint a,IplImage *b,CvScalar c,int r,int flag);

void mouse_get_pixel(int mouseevent, int x, int y, int flags, void* param)
{
	IplImage * p = (IplImage *)param;
	if (mouseevent == CV_EVENT_LBUTTONDOWN) //按下左键
	{
		cout << unsigned(unsigned char(p->imageData[y * p->widthStep + x*3]))<< '\n'
			<< unsigned(unsigned char(p->imageData[y * p->widthStep + x*3+1])) << '\n'
			<<unsigned(unsigned char(p->imageData[y * p->widthStep + x*3+2])) << '\n';
	}
}void mouse_init_carpos(int mouseevent, int x, int y, int flags, void* param)
{
	CvPoint * temp = static_cast<CvPoint *>(param);
	if (mouseevent == CV_EVENT_LBUTTONDOWN){ //按下左键
		temp[0] = cvPoint(x,y);
		cout << "Choose head_point-" << x << ',' << y << '\n';
	}
	if (mouseevent == CV_EVENT_RBUTTONDOWN){ //按下右键
		temp[1] = cvPoint(x,y);
		cout << "Choose tail_point-" << x << ',' << y << '\n';
	}
}

void the_project::p_init_carpos(const char *a)
{
	CvPoint t[2];
	cvSetMouseCallback(a, mouse_init_carpos,(void *)t);
	cout << "选择小车前后初始位置，左键为前点，右键为后点...\n";
	cvWaitKey();
	car_pos[0]=t[0];
	car_pos[1]=cvPoint((t[0].x+t[1].x)/2,(t[0].y+t[1].y)/2);
	car_pos[2]=t[1];
}

void the_project::p_fresh()
{
	char a=cvWaitKey(10);
	if(a=='r'){
		cout << "Re_figure_out the car position...\n";
		p_get_ready();
	}
	else if(a=='s'){
		throw "Stop by force!!...\n";
	}

	///*
	if(v_c_flag==0 && fresh_cam){
		get_in = cvQueryFrame(for_cam);
		cvWarpPerspective(get_in, get_change, transmat);
	}
	else{
		get_in = cvQueryFrame(for_video);
		cvWarpPerspective(get_in, get_change, transmat);
	}
	//*/
	

	cvShowImage("win2",get_change);

}

int aoiGravityCenter(IplImage *src, CvPoint &center);

void the_project::p_findcar(int flag)//if flag=1,it is the first time to find a car otherwise flag=0
{
	if(flag==0)
		p_fresh();
	//red=pf=pos[0],green=pb=pos[2]
	static IplImage * temp_red = cvCreateImage(image_size, IPL_DEPTH_8U, 1);
	static IplImage * temp_green = cvCreateImage(image_size, IPL_DEPTH_8U, 1);

	cvInRangeS(get_change,CV_RGB(red_min,0,0),CV_RGB(0xff,rg_max,rb_max),temp_red);
	cvInRangeS(get_change,CV_RGB(0,green_min,0),CV_RGB(gr_max,0xff,gb_max),temp_green);

	cvShowImage("win_find_car_r",temp_red);
	cvShowImage("win_find_car_g",temp_green);
	if(flag==1){

		cvNamedWindow("win_p");
		cvShowImage("win_p",get_change);

		cvSetMouseCallback("win1", mouse_get_pixel,(void *)get_change);

		cout << "Adjust the color,press Space to finish...\n";

		cvNamedWindow("red");
		cvNamedWindow("green");
		cvCreateTrackbar("red_min","red",&red_min,0xff);
		cvCreateTrackbar("r_blue_max","red",&rb_max,0xff);
		cvCreateTrackbar("r_green_max","red",&rg_max,0xff);
		cvCreateTrackbar("green_min","green",&green_min,0xff);
		cvCreateTrackbar("g_red_max","green",&gr_max,0xff);
		cvCreateTrackbar("g_green_max","green",&gb_max,0xff);
		while(1){
			cvShowImage("red",temp_red);
			cvShowImage("green",temp_green);
			char a = cvWaitKey(10);
			if(a==' '){
				cvDestroyWindow("red");
				cvDestroyWindow("green");

				

				cvDestroyWindow("win_p");
				cout << "Adjust finished...\n";
				break;
			}
			cvInRangeS(get_change,CV_RGB(red_min,0,0),CV_RGB(0xff,rg_max,rb_max),temp_red);
			cvInRangeS(get_change,CV_RGB(0,green_min,0),CV_RGB(gr_max,0xff,gb_max),temp_green);
		}
	}

	
	static CvConnectedComp c_f,c_b;

	CvBox2D box1,box2;
	if(search_all==0){
		cvCamShift(temp_red,get_block(car_pos[0],temp_red,50),cvTermCriteria(2,10,0.5),&c_f,&box1);
		cvCamShift(temp_green,get_block(car_pos[2],temp_green,50),cvTermCriteria(2,10,0.5),&c_b,&box2);
	}
	else{
		cvCamShift(temp_red,cvRect(0,0,640,480),cvTermCriteria(2,5,1),&c_f,&box1);
		cvCamShift(temp_green,cvRect(0,0,640,480),cvTermCriteria(2,5,1),&c_b,&box2);
		search_all=0;
	}
	car_pos[0] = cvPoint(c_f.rect.x+c_f.rect.width/2,c_f.rect.y+c_f.rect.height/2);
	car_pos[2] = cvPoint(c_b.rect.x+c_b.rect.width/2,c_b.rect.y+c_b.rect.height/2);
	car_pos[1]=cvPoint((car_pos[0].x+car_pos[2].x)/2,(car_pos[0].y+car_pos[2].y)/2);
	
	if(flag==0 && current_zone.ahead!=0){
		cvCopyImage(get_unchange,get_looking);
		set_block(car_pos[0],get_looking,cvScalar(10,10,250),7,3);
		set_block(car_pos[2],get_looking,cvScalar(250,10,10),7,3);
		cvShowImage("win_looking",get_looking);
		double the_angle=p_get_onroad_angle();
		cout << "forward Point:" << current_zone.ahead->flag << ";car position" 
			<< car_pos[1].x << "," << car_pos[1].y << ";the_angle:"<<the_angle<<".\n ";
	}

	cvWriteFrame(wr1,get_in);
	cvWriteFrame(wr2,get_change);
}

void the_project::p_getpath_fill()
{
	cout << "Start to fill or repair,press any key to finish...\n";
	cvSetMouseCallback("win3", maze_mouse2,get_binary);
	cvWaitKey();
}