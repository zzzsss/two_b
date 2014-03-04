#include "the_tree.h"

void clear_node(the_node* a)
{
	delete a;
}

void the_tree::insert(CvPoint a,int flag)
{
	//linked-list
	if(flag==-1){//p1...
		if(head==0)
			head = new the_node(a);
		else{
			int i =	1;
			the_node * temp = head;
			while(temp->p1){
				temp = temp->p1;
				i++;
			}
			temp->p1 = new the_node(a,temp,0,0,0,i);
		}
		return;
	}

	//the_tree
	int flag_copy = flag;
	int state;
	if(flag==0)
		head = new the_node(a);
	else{
		the_node * temp = head;

		do{
			state = flag % 10;
			flag = flag / 10;
		
			if(state==0)
				break;
			else if(state==1){
				if(temp->p1==0)
					temp->p1 = new the_node(a,temp,0,0,0,flag_copy);
				else
					temp = temp->p1;
			}
			else if(state==2){
				if(temp->p2==0)
					temp->p2 = new the_node(a,temp,0,0,0,flag_copy);
				else
					temp = temp->p1;
			}
			else if(state==3){
				if(temp->p3==0)
					temp->p3 = new the_node(a,temp,0,0,0,flag_copy);
				else
					temp = temp->p3;
			}
		}while(1);
	}
}

void the_tree::for_each_down(the_node* p,void (*f)(the_node *))
{
	if(p==0)
		return;
	if(p->p1!=0)
		for_each_down(p->p1,f);
	if(p->p2!=0)
		for_each_down(p->p1,f);
	if(p->p3!=0)
		for_each_down(p->p1,f);
	f(p);
}

void the_tree::for_each_back(the_node* p,void (*f)(the_node *))
{
	while(p->parent != 0){
		f(p);
		p=p->parent;
	}
}

void the_tree::for_each_down_i(the_node* p,void (*f)(the_node *,IplImage*),IplImage * i)
{
	if(p==0)
		return;
	if(p->p1!=0)
		for_each_down_i(p->p1,f,i);
	if(p->p2!=0)
		for_each_down_i(p->p1,f,i);
	if(p->p3!=0)
		for_each_down_i(p->p1,f,i);
	f(p,i);
}

the_tree::~the_tree()
{
	for_each_down(head,clear_node);
}

bool the_tree::find_zone(the_node *n,CvPoint a,double r,the_zone* z)
{
	if(is_near_point(n->position,a,r)){
		z->ahead = n;
		z->back = 0;
		return 1;
	}
	if(n->p1!=0){
		if(is_on_line(n->position,n->p1->position,a)){
			z->ahead = n;
			z->back = n->p1;
			return 1;
		}
		else
			if(find_zone(n->p1,a,r,z))
				return 1;
	}
	if(n->p2!=0){
		if(is_on_line(n->position,n->p2->position,a)){
			z->ahead = n;
			z->back = n->p2;
			return 1;
		}
		else
			if(find_zone(n->p2,a,r,z))
				return 1;
	}
	if(n->p3!=0){
		if(is_on_line(n->position,n->p3->position,a)){
			z->ahead = n;
			z->back = n->p3;
			return 1;
		}
		else
			if(find_zone(n->p1,a,r,z))
				return 1;
	}

	return 0;
}