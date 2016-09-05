#include <stdio.h>

float arc_move(float next_pos[], float x1, float y1, int th1, int motion, float d)
{
	//motion will be one of: -3(rev right),-2(rev left),-1(rev), 1(fwd), 2(fwd left), 3(fwd right)
	float cost;
	int m_index,p;
	int th2;
	//delta_x for 6 motions, 8 orientations in 45 deg steps (0 to 315)
	int delta_x[6][8] = {{-1,-1,1,1,1,1,-1,-1},
						 {-1,-1,-1,1,1,1,1,-1},
						 {-1,-1,0,1,1,1,0,-1},
						 {1,1,0,-1,-1,-1,0,1},
						 {1,1,-1,-1,-1,-1,1,1},
						 {1,1,1,-1,-1,-1,-1,1}};
	int delta_y[6][8] = {{-1,-1,-1,-1,1,1,1,1},
						 {1,-1,-1,-1,-1,1,1,1},
						 {0,-1,-1,-1,0,1,1,1},
						 {0,1,1,1,0,-1,-1,-1},
						 {1,1,1,1,-1,-1,-1,-1},
						 {-1,1,1,1,1,-1,-1,-1}};
	int delta_theta[6] = {45,-45,0,0,45,-45};
	m_index = motion+3 - (motion > 0);
	if(th1 < 0){th1 += 360;}
	if(th1 >= 360){th1 -= 360;}
	p = (th1/45)%8; //current theta index (0 to 7)
	
	next_pos[0] = x1 + delta_x[m_index][p]*d; //x2
	next_pos[1] = y1 + delta_y[m_index][p]*d; //y2
	th2 = th1 + delta_theta[m_index]; //th2
	if(th2 < 0){th2 += 360;}
	if(th2 >= 360){th2 -= 360;}
	next_pos[2] = th2;
	
	cost = abs(delta_x[m_index][p]) + abs(delta_y[m_index][p]);
	if(motion < 0)
	{
		cost = cost*2;
	}
	return cost;
}

void simp_move(float next_pos[], float x1,float y1,int motion,float d)
{
	#define up 1
	#define down -1
	#define left -2
	#define right 2
	#define upLeft 3
	#define downRight -3
	#define upRight 4
	#define downLeft -4
	
	float x2,y2;
	
	switch (motion)
	{
		case up:
			x2 = x1;
			y2 = y1 + d;
			break;
		case down:
			x2 = x1;
			y2 = y1 - d;
			break;
		case left:
			x2 = x1-d;
			y2 = y1;
			break;
		case right:
			x2 = x1+d;
			y2 = y1;
			break;
		case upLeft:
			x2 = x1 - d;
			y2 = y1 + d;
			break;
		case downRight:
			x2 = x1 + d;
			y2 = y1 - d;
			break;
		case upRight:
			x2 = x1+d;
			y2 = y1+d;
			break;
		case downLeft:
			x2 = x1-d;
			y2 = y1-d;
			break;
		default:
			printf("invalid motion value\n");
			x2 = 32767;
			y2 = 32767;		
	}
	next_pos[0] = x2;
	next_pos[1] = y2;
	
}
