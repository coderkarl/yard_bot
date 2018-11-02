#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "move.h"

#define NUM_ROWS 70
#define NUM_COLS 55
#define NUM_THETA 8
#define DIAG_WEIGHT 2
#define REV_WEIGHT 1
#define MAX_OPEN 10000


//The a-star path-finding data of a map grid cell
typedef struct {
	float f; //f = g+heuristic
	float g; //cumulative motion cost, should be actual distance, maybe weight reverse
	float x; //x coordinate in meters
	float y; //y coordinate in meters
	int theta; //orientation in deg (45 deg res)
}Cell;

//compare function used for the available qsort function
//This is for sorting an array of my Cell structures from greatest to least
int compare (const void * a, const void * b)
{

  Cell *cellA = (Cell *)a;
  Cell *cellB = (Cell *)b;

  //This will not properly compare floats
  //return ( cellB->f - cellA->f );
  
  //The following ensures floats are compared properly
  // conditon ? IF_TRUE ? IF_FALSE
  return cellB->f > cellA->f ? 1 : -1;
}

void add_obstacle(int map[NUM_ROWS][NUM_COLS],int iRow,int iCol, int height, int width)
{
	int r,c;
	for (r = iRow; r < iRow + height; r++)
	{
		for (c = iCol; c < iCol + width; c++)
		{
			map[r][c] = 1;
		}
	}
}

void md(int n)
{
	//printf("debug line %d\n",n);
}

int main()
{
	int j,k;
	FILE *fpMap;
	FILE *fpPath;
	
	Cell open[MAX_OPEN] = {{0,0,0,0,0}};
	
	int finished[NUM_ROWS][NUM_COLS][NUM_THETA] = {{{0}}};
	int action[NUM_ROWS][NUM_COLS][NUM_THETA] = {{{0}}};
	
	int map[NUM_ROWS][NUM_COLS] = {{0}};
	//assume element 1,1 is (1,-1) meters
	//assume element 99,99 is (99,-99) meters
	
	md(1);
	
	if((fpMap = fopen("test_map.txt","w"))==NULL)
	{
		printf("\aERROR opening test_map.txt\n");
		return (100);
	}
	
	if((fpPath = fopen("test_path.txt","w"))==NULL)
	{
		printf("\aERROR opening test_path.txt\n");
		return (100);
	}
	
	md(2);
	//Obstacles      r ,c ,h, w
	add_obstacle(map,2,15,2,35); //front pines
	add_obstacle(map,2,50,20,2); //east pines
	add_obstacle(map,5,17,1,1); //front tree
	add_obstacle(map,5,25,1,1); //front tree
	add_obstacle(map,8,25,1,1); //front tree
	
	add_obstacle(map,16,16,6,12); //house
	add_obstacle(map,22,17,3,5); //back porch
	add_obstacle(map,27,5,5,10); //garage
	add_obstacle(map,27,17,1,1); //garage post
	add_obstacle(map,32,17,1,1); //garage post
	
	//add_obstacle(map,75,20,4,3); //shed
	
	//add_obstacle(map,36,34,7,1); //enclose test
	//add_obstacle(map,36,36,7,1);
	//add_obstacle(map,43,34,1,3);
	
	//add_obstacle(map,24,1,1,16);
	add_obstacle(map,24,20,15,1);
	
	md(3);
	//Write obstacle coordinates to file
	//fprintf(fpMap,"x y\n");
	for(j = 0; j<NUM_ROWS; j++)
	{
		for(k = 0; k<NUM_COLS; k++)
		{
			if(map[j][k] == 1)
			{
				fprintf(fpMap,"%d %d\n",k,-j);
			}
		}
	}
	md(4);
	//The discretized map is for obstacle checking and A star planning
	//We will still keep track of the continuous position calculated by arc moves
	float x_init = 35;
	float y_init = -60;
	int th_init = 270; //deg, just 45 deg res

	float xg = 10;
	float yg = -20;
	int thg = 270;
	int rg = round(-yg);
	int cg = round(xg);
	int pg = (thg/45)%8;

	float x1,y1,g1,f2,g2,h2,DIST,x2,y2,cost;
	int th1,th2,p1,p2;
	float next_pos[3] = {0,0,0};
	int r1,c1, nOpen, dCount, m, r2,c2, r_init, c_init, count;
	int numMotions = 6, rev_motion;
	int motions[6] = {-3,-2,-1, 1, 2, 3};
	int done, no_sol, a;
	x1 = x_init;
	y1 = y_init;
	th1 = th_init;
	r1 = round(-y_init);
	c1 = round(x_init);
	p1 = (th1/45)%8;

	finished[r1][c1][p1] = 1;
	g1 = 0;
	//h2 = sqrt((xg-x1)^2 + (yg-y1)^2);
	//f2 = g1+h2;
	nOpen = 0;
	done = 0;
	no_sol = 0;
	DIST = 1;

	printf("Begin while\n");
	dCount = 0;
	while(!done && !no_sol)
	{
		md(5);
		for(m = 0; m<numMotions; m++)
		{
			cost = arc_move(next_pos,x1,y1,th1,motions[m],DIST);
			if(next_pos[0] == 32767)
			{
				printf("Invalid motion in move\n");
				return 1;
			}
			x2 = next_pos[0];
			y2 = next_pos[1];
			th2 = next_pos[2];
			/*printf("motion = %d\n",motions[m]);
			printf("x2 = %.0f, y2 = %.0f, th = %d\n",x2,y2,th2);
			printf("Enter key\n");
			scanf("%d",&a);*/
			r2 = round(-y2);
			c2 = round(x2);
			p2 = (th2/45)%8;
			
			if(r2 >= 0 && r2 < NUM_ROWS && c2 >= 0 && c2 < NUM_COLS)
			{
				if(map[r2][c2] == 0 && finished[r2][c2][p2] == 0)
				{
					//cost now taken care of in arc_move()
					/*cost = DIST;
					if(motions[m] >= 2 || motions[m] <= -2)
					{
						cost = cost*DIAG_WEIGHT;
					}
					if(motions[m] < 0)
					{
						cost = cost*REV_WEIGHT;
					}*/
					
					//if(action[r1][c1][p1] * motions[m] < 0){cost = cost*10;} //Really slows it down, similar path in the end
					g2 = g1 + cost*DIST;
					
					//h2 = sqrt((xg-x2)*(xg-x2) + (yg-y2)*(yg-y2
                    h2 = (xg-x2)*(xg-x2) + (yg-y2)*(yg-y2);
					f2 = g2+h2;
					if(nOpen < MAX_OPEN)
					{
						nOpen += 1;
						open[nOpen-1].f = f2;
						open[nOpen-1].g = g2;
						open[nOpen-1].x = x2;
						open[nOpen-1].y = y2;
						open[nOpen-1].theta = th2;
					}
					else
					{
						printf("nOpen = %d\n",nOpen);
						open[0].f = f2;
						open[0].g = g2;
						open[0].x = x2;
						open[0].y = y2;
						open[0].theta = th2;
					}
					finished[r2][c2][p2] = 1;
					action[r2][c2][p2] = motions[m];
				}
			}
		}
		if(nOpen == 0)
		{
			no_sol = 1;
			printf("No Solution found\n");
		}
		else
		{
			md(6);
			/*printf("Before*****************\n");
			for(k=0; k< nOpen; k++)
			{
				printf("open[%d] = %0.1f, %0.1f, %0.1f, %0.1f, %0.1f\n",k,open[k].f,open[k].g,open[k].x,open[k].y,open[k].theta);
			}
			printf("Sort it\n");
			*/
			if(nOpen > 1)
			{
				qsort(open,nOpen,sizeof(Cell),compare);
			}
			/*printf("After***********\n");
			for(k=0; k< nOpen; k++)
			{
				printf("open[%d] = %0.1f, %0.1f, %0.1f, %0.1f, %0.1f\n",k,open[k].f,open[k].g,open[k].x,open[k].y,open[k].theta);
			}
			printf("Enter key\n");
			scanf("%d",&a);
			*/
			g1 = open[nOpen-1].g;
			x1 = open[nOpen-1].x;
			y1 = open[nOpen-1].y;
			th1 = open[nOpen-1].theta;
			dCount = dCount + 1;
			/*if(dCount % 1 == 0)
			{
				printf("x1 = %0.1f, y1 = %0.1f\n",x1,y1);
			}*/
			
			r1 = round(-y1);
			c1 = round(x1);
			p1 = (th1/45)%8;
			nOpen -= 1;
			if(nOpen < 0)
			{
				nOpen = 0;
			}
			if(r1 == rg && c1 == cg && p1 == pg)
			{
				done = 1;
				printf("done\n");
			}
		}
	}

	//count = 1;
	if(done)
	{
		md(7);
		//x(count) = x1;
		//y(count) = y1;
		//fprintf(fpPath,"%.1f %.1f\n",x1,y1);
		r_init = round(-y_init);
		c_init = round(x_init);
		
		//Step backwards and store the optimum path
		//printf("x: %.1f, y: %.1f\n",x1,y1);
		fprintf(fpPath,"%.1f %.1f\n",x1,y1);
		while( (abs(r1-r_init) > 0 || abs(c1-c_init) > 0) && dCount >= -2)
		{
			//printf("action: %d\n",action[r1][c1][p1]);
			rev_motion = -action[r1][c1][p1];
			cost = arc_move(next_pos,x1,y1,th1,rev_motion,DIST);
			if(next_pos[0] == 32767)
			{
				printf("Invalid motion in move\n");
				return 1;
			}
			x1 = next_pos[0];
			y1 = next_pos[1];
			th1 = next_pos[2];
			r1 = round(-y1);
			c1 = round(x1);
			p1 = (th1/45)%8;
			//count = count+1;
			//x(count) = x1;
			//y(count) = y1;
			//printf("x: %.1f, y: %.1f\n",x1,y1);
			fprintf(fpPath,"%.1f %.1f\n",x1,y1);
			dCount = dCount - 1;
		}
		//plot(x,y,'g')
	}
	//Close files
	if(fclose(fpMap) == EOF)
	{
		printf("\aERROR closing test_map.txt\n");
		return (102);
	}
	if(fclose(fpPath) == EOF)
	{
		printf("\aERROR closing test_path.txt\n");
		return (102);
	}	
	
	return 0;
}
