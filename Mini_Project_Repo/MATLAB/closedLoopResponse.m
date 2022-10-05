%% closedLoopResponse.m
% This script takes reads in time and angular position values
% from arduino code that runs a motor. The values are ploted against a
% simulink simulation in a closed loop figurations that is using a PI
% controller to acheive a rise time of 1 second and overshoot of less than 
% 12%. The graph shows that the motor matches the desired charateristics
% found in the simulink simulation.
%
% required file: miniProject_PI.slx, 4.7.ino
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
% Display what you got--
disp(data)
while (~strncmp(data,'Finished',8)) % Until Arduino signals that it is done
    k=k+1;
    % change string data to cell array using tab delimiter
    dataarray = strsplit(data,char(9));
    % save data converting strings to numbers-
    Time(k) = eval(dataarray{1});
    position(k) = eval(dataarray{2});
    % Read Data from Arduino
    data = fgets(obj);
    % Display what you got
    disp(data)
end
fclose(obj)

%% Set Up Transfer Function
%Transfer function values
K = 1.9;
sigma = 22.22;

%Final position
rad = pi; %rad

%% Run Simulink Simulation
open_system('miniProject_PI')
%
% run the simulation
%
out=sim('miniProject_PI');

%% A Plot of the results
figure
plot(out.Position)
hold on
plot(Time / 1000,position)
hold off
xlabel('Time (sec)')
ylabel('Position (rad)')
%
% Save results in a .mat file to use later
%
save somedata.mat Time position
    

    