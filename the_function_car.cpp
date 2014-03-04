#include"the_car.h"

void car_f(the_car* a,int i)
{
	for(;i>=0;i--)
		a->move_f();
}
void car_b(the_car* a)
{
	a->move_b();
}
void car_l(the_car* a)
{
	a->move_l();
}
void car_r(the_car* a)
{
	a->move_r();
}
void car_s(the_car* a)
{
	a->move_s();
}
void restart(the_car* a)
{
	a->restart();
}

