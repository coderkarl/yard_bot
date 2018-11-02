data = load('-ascii','test_map.txt');
map_x = data(:,1);
map_y = data(:,2);

path_data = load('-ascii','test_path.txt');
x = path_data(:,1);
y = path_data(:,2);

x_init = x(end);
y_init = y(end);
xg = x(1);
yg = y(1);

close all
figure(2)
plot(map_x,map_y,'rx')
hold on

%plot init and goal
plot(x_init,y_init,'bo','Markersize',20)
plot(xg,yg,'go','Markersize',10)
plot(xg,yg,'go','Markersize',30)
axis equal
%pause

%plot path
plot(x,y,'g')

fprintf('End of program\nPress any key\n');
pause
