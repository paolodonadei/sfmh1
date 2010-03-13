clear all
close all
clc

[ F, ksm ]  = generateFangl( 0 , 0 , 1.01 , 40 , 1 , 5 , 0 , 0,(pi/180)*0);

xcm=ksm{1,1}(1,3);
ycm=ksm{1,1}(2,3);
arm=ksm{1,1}(1,1)/ksm{1,1}(2,2);

for i=700:1400

    errorgm(i-699,1)=computerEssentialErrorSVDNFramesWeighted([i xcm ycm i/arm],F(2:10));
    errorb1m(i-699,1)=computerEssentialErrorSVDNFramesWeighted([i xcm ycm i/arm],F);
    errorb2m(i-699,1)=computerEssentialErrorSVDNFramesWeighted([i xcm ycm i/arm],F(1));
    errorb3m(i-699,1)=computerEssentialErrorSVDNFramesWeighted([i xcm ycm i/arm],F(4));
end
t=700:1400;

plot(t, errorgm,t,errorb1m,t,errorb2m);
legend('Only good F matrices','all F matrices' , 'only the bad F martix');

figure
plot(t, errorgm)
title('Only good F matrices');

figure
plot(t, errorb1m)
title('all F matrices');

figure
plot(t, errorb2m)
title('only the bad F martix');

figure
plot(t, errorb3m)
title('a good F martix');

figure

subplot(1,2,1)
plot(t, errorb2m)
xlabel('focal length');
ylabel('energy function');
title('parallel frames');


subplot(1,2,2)
plot(t, errorb3m)
xlabel('focal length');
ylabel('energy function');
title('non-parallel frames');
