% s = serial(‘COM11?,’BaudRate’,9600,’DataBits’,8);
% 
% fopen(s)
% 
% >>fwrite(s,’1’);
% 
% >>A=fscanf(s,’%c’,s.BytesAvailable);
% 
% >>disp(A); //display received character, should be same as sent character
% 
% >> fclose(s)

fclose(instrfind)
s = serial('COM3','BaudRate',9600,'DataBits',8);
fopen(s)
% fwrite (s,'1');

wnd = 111;

x = linspace(0,wnd,wnd+1);
A = zeros(1,wnd+1);
mean_A = zeros(1,wnd+1);
t = 1;

% real-time plot of the output and add a quit button
figure
subplot(1,2,1);
outplot1 = plot(x, A,'-o');
axis([0 wnd-1 0 Inf]);
grid on;
xlabel('Indeks Data');
ylabel('Ketinggian Air (cm)');
title('Ketinggian Air');

subplot(1,2,2);
outplot2 = plot(x, mean_A,'-o');
axis([0 wnd-1 0 Inf]);
% axis([0 3.5 0 volt_scale]);
grid on;
xlabel('Indeks Data');
ylabel('Rata-Rata Ketinggian Air (cm)');
title('Rata-Rata Ketinggian Air');
% Define the erase button
quitbutton = uicontrol('style','pushbutton',...
   'string','Stop', ...
   'fontsize',12, ...
   'position',[10,10,50,20], ...
   'callback','quitit=1;fclose(s);'); %delete(gcf);');
quitit = 0;
% bpmtext = uicontrol('style', 'text',...
%     'string', ['BPM: '],...
%     'fontsize', 12,...
%     'position', [80, 10, 100, 20]);
drawnow;

% for i=1:wnd
%     A(i) = fscanf(s,'%d');
%     set(outplot, 'ydata', A);
%     disp(A(i));
% end

% Run the program until button pressed

while (quitit == 0)
    while((t <= wnd) && (quitit == 0))
%         if (t ~= 1)
%             
%             if (t > 2)
%         end
%         if(t > 1)
%         else
%             mean_A(t+1) = 0;
%         end
        A(t+1) = fscanf(s,'%d');
        mean_A(t+1) = sum(A)/t;
        % Plot (or update the plot of) the output
        set(outplot1, 'ydata', A);
        set(outplot2, 'ydata', mean_A);
%         disp(mean_A);
%         disp(A);
        t = t + 1;
        %set(aveplot, 'ydata', dif);
        drawnow;
    end
    % Reset the 250 tick window and increment runtime counter
    t = 1;
    A = zeros(1,wnd+1);
    mean_A = zeros(1,wnd+1);
end
fclose(s)


