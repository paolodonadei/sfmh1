function [ x ] = Houmanselfcalib1( F,w,h )


%first estimate the F assuming known other parameters
ux1=w/2;
vy1=h/2;

ux2=w/2;
vy2=h/2;

x  = PollefeyVisualwithPOLDTWOFRAMEFAM( F,ux1*2,vy1*2 );



disp(['f1 is ' num2str(x(1,1)) ' and f2 is ' num2str(x(1,2)) ' and ux1 is ' num2str(ux1)  ' and ux2 is ' num2str(ux2) ' vy1 is '  num2str(vy1)  ' vy2 is '  num2str(vy2) ]);
%now estimate x center using the known F

[K1_1 K2_1] = estimateUXfromFandVY( F,x,ux1,ux2,vy1,vy2 );

% big problem here is that the x belongs to one camera not two, so i have
% to estimate to uxs but for now lets assume the camera centers have moved
% the same


ux1=K1_1(1,3);
vy1=K1_1(2,3);

ux2=K2_1(1,3);
vy2=K2_1(2,3);


x(1,1)=K1_1(1,1);
x(1,2)=K2_1(1,1);

disp(['f1 is ' num2str(x(1,1)) ' and f2 is ' num2str(x(1,2)) ' and ux1 is ' num2str(ux1)  ' and ux2 is ' num2str(ux2) ' vy1 is '  num2str(vy1)  ' vy2 is '  num2str(vy2) ]);

%now estimate y center using the known F and new ux and the new F (I wonder
%if the new F will be different)


[K1_2 K2_2] = estimateVYfromFandUX( F,x,ux1,ux2,vy1,vy2 );

%now reestimate the F
ux1=K1_2(1,3);
vy1=K1_2(2,3);

ux2=K2_2(1,3);
vy2=K2_2(2,3);

x(1,1)=K1_2(1,1);
x(1,2)=K2_2(1,1);

disp(['f1 is ' num2str(x(1,1)) ' and f2 is ' num2str(x(1,2)) ' and ux1 is ' num2str(ux1)  ' and ux2 is ' num2str(ux2) ' vy1 is '  num2str(vy1)  ' vy2 is '  num2str(vy2) ]);

[ x ] = PollefeyVisualwithPOLDTWOFRAMEFAM( F,ux1*2,vy1*2 );

disp(['f1 is ' num2str(x(1,1)) ' and f2 is ' num2str(x(1,2)) ' and ux1 is ' num2str(ux1)  ' and ux2 is ' num2str(ux2) ' vy1 is '  num2str(vy1)  ' vy2 is '  num2str(vy2) ]);







end





