# yard_bot
Yard bot with perception, localization, mapping, and path planning in python and C

yard_astar folder contains the path planning function

To test yard_astar:  
`cd /path/to/yard_bot/yard_astar/src`  
In direction_a_star_c.c use add_obstacle(map,row,col,height,width) as desired. Set x, y, th_init; xg,yg,thg  
`make`  
`./test_astar`  
`octave view_a_star_c.m`  
Press a key in the terminal to close the octave figure.  
You can also just run from the octave gui.  
Note this example map element map(1,1) is (1,-1) meters; map(99,99) is (99,-99) meters.  
The ros convention we plan to follow in the wheele repo (AvoidObs, Astar) has map origin x,y at map(0,0).  
In the wheele repo, I use a get_map_indices function to help re-use this code but with different index definitions.  

## astar issues
direction_a_star_c.c is slow. Perhaps three dimensions is too much. Try removing the theta dimension and use simp_move in move.c.  
