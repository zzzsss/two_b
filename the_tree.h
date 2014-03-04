#ifndef THE_TREE_H
#define THE_TREE_H

#include "cv.h"
#include "highgui.h"

const int RA = 40;

bool is_near_point(CvPoint sta,CvPoint run,double radium=RA);
bool is_on_line(CvPoint s1,CvPoint s2,CvPoint r);
double distance(CvPoint sta,CvPoint run);
double get_vector_angle(CvPoint v0,CvPoint vt,CvPoint u0,CvPoint ut);

class the_tree;
class the_project;
class the_node;

struct the_zone{
		the_node * ahead;
		the_node * back;
		the_zone(the_node * a=0,the_node * b=0):ahead(a),back(b){}
	};

struct the_node{
	//friend class the_project;
	//friend class the_maze_project;
	//friend class the_tree;
	the_node * parent;
	the_node * p1;
	the_node * p2;
	the_node * p3;
	//anti-clock direction
	CvPoint position;
	int flag;

	the_node(CvPoint a=cvPoint(0,0),the_node * p=0,the_node * b=0,the_node * c=0,the_node * d=0,int e=0){
		flag = e;	
		position = a;
		parent = p;
		p1 = b;
		p2 = c;
		p3 =d;
	}
};

class the_tree{
	friend class the_project;
	friend class the_maze_project;
	the_node * head;
	
public:
	the_tree():head(0){}
	
	void insert(CvPoint p,int flag);//numbers of nodes <= 31
	void for_each_down(the_node* p,void (*f)(the_node *));
	void for_each_back(the_node* p,void (*f)(the_node *));

	~the_tree();

	bool find_zone(the_node *,CvPoint,double,the_zone *);
	
	void for_each_down_i(the_node* p,void (*f)(the_node *,IplImage*),IplImage * i);
};

#endif
