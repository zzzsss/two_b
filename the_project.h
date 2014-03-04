#ifndef THE_PROJECT_H
#define THE_PROJECT_H

#include "the_tree.h"
#include "the_car.h"
#include <iostream>
#include <cstdlib>
#include <vector>

using std::cout;
using std::cin;
using std::vector;

void maze_mouse1(int mouseevent, int x, int y, int flags, void* param);
void maze_mouse2(int mouseevent, int x, int y, int flags, void* param);

void mouse(int mouseevent, int x, int y, int flags, void* param);
double get_angle(double x1,double y1,double x2,double y2);
void figure_out_p_path(IplImage * p,the_tree * t,IplImage *);
void figure_out_m_path(IplImage * p,the_tree * t);
void show_path_point(the_node * n,IplImage * p);

class the_project{
	friend class the_s_car;
protected:
	//flags
	int v_c_flag;//1 for video,0 for camera
	bool fresh_cam;
	bool search_all;

	//functions
	void p_fresh();
	void p_findcar(int flag);
	//tree
	the_tree path_tree;
	//image
	IplImage * get_in;
	IplImage * get_change;
	IplImage * get_binary;
	IplImage * get_path;

	IplImage * get_unchange;	//first get_change
	IplImage * get_looking;

	CvCapture * for_video;
	CvCapture * for_cam;
	CvVideoWriter * wr1;
	CvVideoWriter * wr2;
	CvSize image_size;

	void p_getpath_fill();
	//persp
	CvPoint2D32f originpoints[4]; //保存四个点的原始坐标,anti_clock
	CvPoint2D32f newpoints[4];
	CvMat* transmat;
	//car
	CvPoint car_pos[3];
	int red_min;
	int rg_max;
	int rb_max;
	int green_min;
	int gb_max;
	int gr_max;

	//running
	enum State{N_before,N_after,On_way};
	the_zone current_zone;
	enum State car_state;

	void p_get_ready();
	double p_get_onroad_angle();
	void p_adjust_direction(double k=20);
	void p_running_to_next_node();
	void p_cross_node();
	void p_turn();

	the_car * car_of_pro;
	
public:
	void p_init_carpos(const char *);

	the_project(){}
	//procedure
	void project_init();
	void project_getting();
	void project_binary();

	void project_getpath();

	void project_running(int first_time_flag=1);
	void project_clear();

	void run(int flag=0);
	void set_car(the_car* a){
		car_of_pro = a;
	}
};

#endif