s = serial('/dev/ttyACM0','BaudRate',9600,'FlowControl','hardware','Timeout',5);
fopen(s);
values = fscanf(s,'%d %f %f %f %f');
disp(values);