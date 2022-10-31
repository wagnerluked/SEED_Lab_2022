% Step experiment input voltage to forward speed transfer function

%% Experimental Data
%Setup serial comms w/ Arduino
port = 'COM4';
obj = serial(port, 'BaudRate', 115200);
obj.terminator = char(10);
fopen(obj);

%read 'Ready' from Arduino
dummy = fgets(obj);

%Read after sending command
disp('Start Arduino Counting Event')
fprintf(obj, '%s\n', 'S'); %sends command signal
data = [];
k=0;

%Read Arduino data
data = fgets(obj);

%Display Data
disp(data)
while(~strncmp(data, 'Finished', 8)
    k += 1;
    dataarray = strsplit(data, char(9));
    Time(k) = eval(dataarray{1});
    position(k) = eval(dataarray{2});
    data = fgets(obj);
    disp(data)
end
fclose(obj)

%% Simulated Data
K = 0.057;
sigma = 5;

% Simulates model
open_system('DistanceStepResponse');
out = sim('DistanceStepResponse');

%% Simulated vs Experimental Comparison
figure(1)
plot(currentTime/1000,rho_dot);
hold on
plot(out.velocity)
hold off
xlim([0 10]);
ylim([0 0.6]);
title("Forward Velocity");
xlabel('Time (s)');
ylabel('Velocity (m/s)');
legend('Experimental', 'Simulated');
