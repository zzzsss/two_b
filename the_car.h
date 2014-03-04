#ifndef THE_CAR_
#define THE_CAR_

#include "the_port.h"

class the_car{
	ComPort the_port;
	char state;//s,f,b,r,l...x,y,1(oppo.l),2(oppo.r)
public:
	the_car():the_port(ComPort("COM5")){
		state = 'f';
		move_s();
	}
	void restart()
	{
		the_port.Send('s');
		//the_port = ComPort("Com5");
	}
	void move_f()
	{
		cout << "forward...\n";
		if(state=='f')
			return;
		the_port.Send('f');
		state = 'f';
	}
	void move_b()
	{
		cout << "back...\n";
		if(state=='f')
			return;
		the_port.Send('b');
		state = 'b';
	}
	void move_r()
	{
		cout << "right...\n";
		if(state=='r')
			return;
		//? l or x
		the_port.Send('x');
		state = 'r';
	}
	void move_l()
	{
		cout << "left...\n";
		if(state=='l')
			return;
		the_port.Send('y');
		state = 'l';
	}
	void move_s()
	{
		cout << "stop...\n";
		if(state=='s')
			return;
		the_port.Send('s');
		state = 's';
	}
};

#endif
