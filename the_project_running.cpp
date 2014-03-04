#include "the_project.h"

void car_f(the_car*,int i =0);
void car_b(the_car*);
void car_l(the_car*);
void car_r(the_car*);
void car_s(the_car*);

int THE_ANGLE=6;

double the_project::p_get_onroad_angle()	//路为底
{
	return get_angle(car_pos[0].x-car_pos[2].x,car_pos[0].y-car_pos[2].y,
			current_zone.ahead->position.x-car_pos[1].x,
			current_zone.ahead->position.y-car_pos[1].y);
}


void the_project::p_get_ready()
{
	path_tree.find_zone(path_tree.head,car_pos[1],40,&current_zone);
	if(current_zone.ahead==0)
		throw("Can't find zone of initial point...");
	else if(current_zone.back==0){
		if(current_zone.ahead->parent==0)
			throw("Already out...");
		else
			p_turn();
	}
	else{
		car_state = On_way;
		p_adjust_direction();
	}
}

void the_project::p_adjust_direction(double an)
{
	double k = p_get_onroad_angle();
 	while(1){
		if(k<-1* an)	//偏离
			car_r(car_of_pro);
		else if(k>an)
			car_l(car_of_pro);
		else if(k>= -1 * an && k<= an)
			break;
		p_findcar(0);
		k = p_get_onroad_angle();
	}
}

void the_project::p_running_to_next_node()
{
	while(!is_near_point(car_pos[1],current_zone.ahead->position)){
		double k = p_get_onroad_angle();
		if(!(k>= -1*THE_ANGLE && k<= THE_ANGLE))
			p_adjust_direction();
		car_f(car_of_pro);
		p_findcar(0);
	}
	cout << "到达节点"<<current_zone.ahead->flag<<'\n';

	//adjust when reaching a node
	int count=0;
	/*
	while(!is_near_point(car_pos[2],current_zone.ahead->position) && count < 2){
		car_f(car_of_pro);
		p_findcar(0);
		cout << count ++ <<'\n';
	}
	cout << "调整结束...\n";
	*/

	car_state = N_before;
}

void the_project::p_cross_node()
{
	while(!is_near_point(car_pos[2],current_zone.ahead->position)){
		car_f(car_of_pro);
		p_findcar(0);
	}
	car_s(car_of_pro);
}

void the_project::p_turn()
{
	the_node * temp = current_zone.ahead;
	current_zone.ahead = current_zone.ahead->parent;
	current_zone.back = temp;

	p_adjust_direction(15);
	car_state = On_way;
}
