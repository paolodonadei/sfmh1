close all
clear all
clc
P1=load('../../data/euclidean/build2/001.P');
P2=load('../../data/euclidean/build2/002.P');
P3=load('../../data/euclidean/build2/003.P');

width= 1024 ;
height=768  ;
[K1, R, t] = vgg_KR_from_P(P1);
[K2, R, t] = vgg_KR_from_P(P2);
[K3, R, t] = vgg_KR_from_P(P3);
K1
K2
K3
[F] =PsTOFs({P1 P2 P3});

count=1;
gfocal=1140;
gocx=  525;
gocy=390  ;
for i=0:0.5:10
    G=F;
    G{1}=G{1} + ((rand(3,3)-0.5)*i);
    [fcln, centerlocn] = S2nonlinsolveEssNfram(G,width,height);
    [fclt, centerloct] = S2nonlinsolveEsstwofram(G,width,height);
    errnfram(count)=abs(fcln(1)-gfocal);
    errtwofram(count)=abs(fclt(1)-gfocal);
    errnframOC(count)=sqrt(((centerlocn(1)-gocx)^2)+((centerlocn(2)-gocy)^2));
    errtwoframOC(count)=sqrt(((centerloct(1)-gocx)^2)+((centerloct(2)-gocy)^2));
    count=count+1;
    count
end


plot([errnfram errtwofram]);
title('focal length error comparison between N frame method and two frame clustering');
legend('N frame', ' two frame clustering');

figure
plot([errnframOC errtwoframOC]);
title('optical center error comparison between N frame method and two frame clustering');
legend('N frame', ' two frame clustering');
