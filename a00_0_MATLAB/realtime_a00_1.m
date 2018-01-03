
fclose(instrfind)
s = serial('COM3','BaudRate',9600,'DataBits',8);
set(s, 'Timeout', 10);
fopen(s)

wnd = 10;

x = linspace(0,wnd-1,wnd);
A = zeros(1,wnd);
mean_A = zeros(1,wnd);
t = 1;

% real-time plot of the output and add a pause button
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
grid on;
xlabel('Indeks Data');
ylabel('Rata-Rata Ketinggian Air (cm)');
title('Rata-Rata Ketinggian Air');
% Define the pause button
quitbutton = uicontrol('style','pushbutton',...
   'string','Stop', ...
   'fontsize',12, ...
   'position',[10,10,50,20], ...
   'callback','quitit=1;fclose(s);');
quitit = 0;
indextext = uicontrol('style', 'text',...
    'string', ['Indeks : '],...
    'fontsize', 12,...
    'position', [80, 10, 100, 20]);
heighttext = uicontrol('style', 'text',...
    'string', ['Tinggi Sekarang : '],...
    'fontsize', 12,...
    'position', [180, 10, 150, 20]);
meantext = uicontrol('style', 'text',...
    'string', ['Rata-Rata : '],...
    'fontsize', 12,...
    'position', [340, 10, 200, 20]);
drawnow;

while (quitit == 0)
    while((t <= wnd) && (quitit == 0))
        
        A(t) = fscanf(s,'%d');
        mean_A(t) = sum(A)/t;
        set(outplot1, 'ydata', A);
        set(outplot2, 'ydata', mean_A);
        if (t ~= wnd+1)
            set(indextext, 'string', ['Index : ', num2str(t-1)]);
            set(heighttext, 'string', ['Tinggi Sekarang : ', num2str(A(t))]);
            set(meantext, 'string', ['Rata-Rata : ', num2str(mean_A(t))]);
        end
        t = t + 1;
        drawnow;
    end
    t = 1;
    A = zeros(1,wnd);
    mean_A = zeros(1,wnd);
end
fclose(s)


