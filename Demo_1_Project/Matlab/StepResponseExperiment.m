%% openLoopResponse.m
% This script takes reads in time and angular velocity values
% from arduino code that runs a motor. The values are ploted against a
% simulink simulation in an open loop configuration to determine an 
% approximate 1st order transfer function. The graph shows that the motor
% behavior matches the transfer funtion
%
% required file: miniProject_openLoop.slxc, 4.6.ino
%
%% Read in Data from Arduino
% port must be set to the communication port used by the Arduino
% you can find the port by going to 'tools -> Port' in the Arduion
% application. For a PC, it will be something like COM6
port='COM3';
obj = serial(port, 'BaudRate', 115200);
obj.terminator = char(10);
fopen(obj)
%
% do a read to get Ready! from Arduino
%
dummy = fgets(obj);
%
% Read and display some data
%
%for i=1:5,
%    data = fgets(obj);
%    disp(data)
%end;
%
% Read data after sending command to Arduino
%
disp('Starting Counting Event in Arduino')
fprintf(obj,'%s\n','S'); % send start signal to Arduino
data=[];
k=0

% Read Data from Arduino
data = fgets(obj);
% Display what you got
disp(data)
while (~strncmp(data,'Finished',8)) % Until Arduino signals that it is done
    k=k+1;
    % change string data to cell array using tab delimiter
    dataarray = strsplit(data,char(9));
    % save data converting strings to numbers
    Time(k) = eval(dataarray{1});
    Rho_dot1(k) = eval(dataarray{2});
    Phi_dot1(k) = eval(dataarray{3});
    Rho_dot2(k) = eval(dataarray{4});
    Phi_dot2(k) = eval(dataarray{5});
    % Read Data from Arduino
    data = fgets(obj);
    % Display what you got
    disp(data)
end;
fclose(obj)

%% Set Up Variables for Simulink


%% Run Simulink Simulation
open_system('projectname')
%
% run the simulation
%
out=sim('Projectname');

%% A Plot of the results
figure
plot(out.Rho_dot)
hold on
plot(Time / 1000, Rho_dot1)
plot(Time / 1000, Rho_dot2)
hold off
xlabel('Time (sec)')
ylabel('Velocity (in/sec)')

figure
plot(out.Phi_dot)
hold on
plot(Time / 1000, Phi_dot1)
plot(Time / 1000, Phi_dot2)
hold off
xlabel('Time (sec)')
ylabel('Angular Velocity (rad/sec)')
%
% Save results in a .mat file to use later
%
save somedata.mat Time velocity
    