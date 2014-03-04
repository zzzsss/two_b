//realization of the_project

#include "the_project.h"
void restart(the_car* a);

void the_project::run(int flag)
{
	//0 step
	v_c_flag = flag;
	fresh_cam=1;
	search_all=1;
	//first step -- initial
	project_init();
	//second step -- PerspectiveTransform
	project_getting();
	//third step -- get binary image
	project_binary();
	//fourth step -- get path
	
	project_getpath();


	//fifteh step -- running
	project_running(1);
	//sixth step -- clear
	project_clear();
}

void the_project::project_init()
{
	car_of_pro = new the_car();


	//camera  480*640

	for_cam = cvCreateCameraCapture(1);
	for_video = cvCreateFileCapture("test.avi");
	image_size = cvSize(cvGetCaptureProperty(for_cam,3),cvGetCaptureProperty(for_cam,4));
	wr1 = cvCreateVideoWriter("record_ori.avi",CV_FOURCC('X','V','I','D') ,15,image_size);
	wr2 = cvCreateVideoWriter("record_cha.avi",CV_FOURCC('X','V','I','D') ,15,image_size);

	newpoints[0]=cvPoint2D32f(0,0);
	newpoints[1]=cvPoint2D32f(0,image_size.height);
	newpoints[2]=cvPoint2D32f(image_size.width,image_size.height);
	newpoints[3]=cvPoint2D32f(image_size.width,0);

	red_min=200;
	rg_max=100;
	rb_max=100;
	green_min=200;
	gb_max=100;
	gr_max=100;

}

void the_project::project_getting()
{
	cout << "Ready to go?\n";
	cin.get();
	get_in = cvCreateImage(image_size, IPL_DEPTH_8U, 3);

	if(v_c_flag==0){
	// get a image of path
	///*
		IplImage *new_one;
		cout << "调整照相机位置并获取图像,Sapce to continue:\n";
		cvNamedWindow("Adjust");
		while(1){
			if(cvWaitKey(10)==' '){
				cvCopyImage(new_one,get_in);
				cvDestroyWindow("Adjust");
				cvReleaseCapture(&for_cam);
				break;
			}
		new_one = cvQueryFrame(for_cam);
		cvShowImage("Adjust",new_one);
	}
		cvSaveImage("image_origin.jpg",get_in);
	//*/
	}
	else if(v_c_flag==1){
	//for_test
		get_in = cvQueryFrame(for_video);
	}

	cvNamedWindow("win1");
	cvShowImage("win1",get_in);
	// transform
	get_change = cvCreateImage(image_size, IPL_DEPTH_8U, 3);
	transmat = cvCreateMat(3, 3, CV_32FC1);

	vector<CvPoint2D32f> points;
	void * p_pointer = 
		reinterpret_cast<void *>(&points);

	cout << "choose four points for transform.\n";
	cvSetMouseCallback("win1", mouse,p_pointer);
	cvWaitKey();

	cvDestroyWindow("win1");
	for(int i=0;i<4;i++)
		originpoints[i] = points[i];

	cvGetPerspectiveTransform(originpoints, newpoints, transmat);
	cvWarpPerspective(get_in, get_change, transmat);

	cvNamedWindow("win1");
	cvShowImage("win1",get_in);
	cvNamedWindow("win2");
	cvShowImage("win2",get_change);

	cvSaveImage("image_square.jpg",get_change);

	get_unchange = cvCreateImage(image_size, IPL_DEPTH_8U, 3);
	get_looking = cvCreateImage(image_size, IPL_DEPTH_8U, 3);
	cvCopyImage(get_change,get_unchange);

	cout << "Press any key to continue...\n";
	cvWaitKey();
}

void the_project::project_binary()
{

	get_binary = cvCreateImage(image_size, IPL_DEPTH_8U, 1);

	int blue=100;
	int green=10;
	int red=10;
	cvCreateTrackbar("blue","win2",&blue,0xff);
	cvCreateTrackbar("green","win2",&green,0xff);
	cvCreateTrackbar("red","win2",&red,0xff);

	cvNamedWindow("win3");
	cout << "Press Space to continue...\n";
	while(1){
		char a = cvWaitKey(10);
		if(a==' ')
			break;
		cvInRangeS(get_change,cvScalarAll(0),CV_RGB(red,green,blue),get_binary);
		cvShowImage("win3",get_binary);
	}
	//cvWaitKey();
	

	get_path = cvCreateImage(image_size,8,1);
	cvCopyImage(get_binary,get_path);
}

//get_path
void show_path_b_point(the_node * n,IplImage * p);
void the_project::project_getpath()
{
	p_getpath_fill();
	cvSaveImage("image_binary.jpg",get_binary);

	IplImage * temp = cvCreateImage(image_size,8,1);
	cvCopyImage(get_binary,temp);
	figure_out_p_path(get_binary,&path_tree,temp);

	cout << "Find the path...\n";
	cvNamedWindow("win4");

	path_tree.for_each_down_i(path_tree.head,show_path_point,get_path);
	path_tree.for_each_down_i(path_tree.head,show_path_b_point,get_unchange);
	cvShowImage("win4",get_path);
	cvNamedWindow("win_looking");
	cvNamedWindow("win_find_car");
}
	


void the_project::project_running(int first_time_flag)
{
	
	try{

	if(first_time_flag)
		p_findcar(first_time_flag);

	if(v_c_flag==0){
		cout << "restart camera,press space to continue\n";
		for_cam = cvCreateCameraCapture(1);
		fresh_cam=1;
		while(1){
			if(cvWaitKey(10)==' ')
				break;
			p_fresh();
		}
	}

	p_findcar(0);
	cout << "Start to run the car...\n";
	p_get_ready();	//get car position right

	bool not_end=1;
	//running
	while(not_end){
		if(car_state == On_way){	//在两个节点之中，则向前直到车头到达节点
			p_running_to_next_node();
			car_state = N_before;
		}
		else if(car_state == N_before){	//车头到达节点
			car_state = On_way;
			if(current_zone.ahead->parent==0){	//到达终点
				not_end = 0;
				p_cross_node();
				continue;
			}
			else
				p_turn();
		}
	}

	cout << "The car stopped,any key to quit...\n"; 
	}
	catch(const char *a){
		cout << "Error,"<<a<<",again!";

		cout << "restart camera,(don't press r or s...)\n";
		cvReleaseCapture(&for_cam);
		//for_cam = cvCreateCameraCapture(1);
		//***********************************************************
		get_in = cvCreateImage(image_size, IPL_DEPTH_8U, 3);
		search_all=1;
		//************************************************************
		cout << "restart port...\n";
		car_of_pro->move_s();
		delete car_of_pro;
		car_of_pro = new the_car();
		car_of_pro->move_s();

		fresh_cam=0;
		project_running(0);
	}
}
		
void the_project::project_clear()
{
	cout << "Finished...\n";
	cvDestroyAllWindows();
	//cvReleaseImage(&get_in);
	cvReleaseImage(&get_change);
	cvReleaseImage(&get_binary);
	cvReleaseImage(&get_path);
	cvReleaseVideoWriter(&wr1);
	cvReleaseVideoWriter(&wr2);
	cvReleaseCapture(&for_video);
	cvReleaseCapture(&for_cam);
}