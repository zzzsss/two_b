//functions of points

#include "cv.h"
#include <cmath>

const int ECLIPSE=40;

//坐标系非标准

double distance(CvPoint sta,CvPoint run)
{
	double x1=sta.x,y1=sta.y,x2=run.x,y2=run.y;
	return sqrt((y2-y1)*(y2-y1)+(x2-x1)*(x2-x1));
}

bool is_near_point(CvPoint sta,CvPoint run,double radium)
{
	return distance(sta,run)<=radium;
}

double get_angle(double x1,double y1,double x2,double y2);

double get_vector_angle(CvPoint v0,CvPoint vt,CvPoint u0,CvPoint ut){//v/u
	return get_angle(vt.x-v0.x,vt.y-v0.y,ut.x-u0.x,ut.y-u0.y);
}


bool is_on_line(CvPoint s1,CvPoint s2,CvPoint r)
{
	//if(((r.x<s1.x)&&(r.x<s2.x))||((r.x>s1.x)&&(r.x>s2.x))||((r.y<s1.y)&&(r.y<s2.y))||((r.y>s1.y)&&(r.y>s2.y)))
	//	return 0;
	double d1 = distance(s1,r);
	double d2 = distance(s2,r);
	double d = distance(s1,s2);

	if(d1+d2 > 2*sqrt(d*d/4+d*d/36)//||abs(get_vector_angle(s1,s2,r,s2)) > 100||abs(get_vector_angle(s2,s1,r,s1)) < 100
		)
				return 0;
	return 1;
}



//figure out the angle
double get_angle(double x1,double y1,double x2,double y2)	//以向量为x轴，角度为负180至正180 ； v1/v2
{
	const double PI = 3.1415926;
	double i1,i2;
	i1 = (x1*x2+y1*y2)/(x2*x2+y2*y2);
	i2 = (y1*x2-x1*y2)/(x2*x2+y2*y2);

	if(i1>0)
		return atan(i2/i1)/PI * 180;
	else if(i1==0){
		if(i2>0)
			return 90;
		else
			return -90;
	}
	else{
		if(i2<0)
			return atan(i2/i1)/PI * 180-180;
		else if(i2==0)
			return 180;
		else
			return atan(i2/i1)/PI * 180+180;
	}
}
		
bool is_op_direction(double x1,double y1,double x2,double y2)
{
	double i=get_angle(x1,y1,x2,y2);
	if((i>=110 && i<=180) || (i>=180 && i<=-110))
		return 1;
	else
		return 0;
}

CvPoint move_point(CvPoint a,double angle,int l)
{
	const double PI = 3.1415926;
	CvPoint temp = a;
	temp.x = a.x + l * cos(angle*PI/180);
	temp.y = a.y + l * sin(angle*PI/180);
	return temp;
}
CvPoint rotate_point(CvPoint center,CvPoint r,double angle)	//顺时针
{
	const double PI = 3.1415926;
	double tx = r.x - center.x;
	double ty = r.y - center.y;
	CvPoint temp;
	temp.x = center.x + tx * cos(angle*PI/180) - ty * sin(angle*PI/180);
	temp.y = center.y + tx * sin(angle*PI/180) + ty * cos(angle*PI/180);
	return temp;
}