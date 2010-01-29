close all
clear all
clc

%
% P1=load('../data/euclidean/build2/001.P');
% P2=load('../data/euclidean/build2/002.P');
% P3=load('../data/euclidean/build2/003.P');
%
% width= 1024 ;
% height=768  ;
% [K1, R, t] = vgg_KR_from_P(P1);
% [K2, R, t] = vgg_KR_from_P(P2);
% [K3, R, t] = vgg_KR_from_P(P3);
% K1
% K2
% K3
%[F] =PsTOFs({P1 P2 P3});
% gfocal=1140;
% gocx=  525;
% gocy=390  ;






 count=1;

for i=0:0.0002:0.003
   
    for k=1:10
        [ F, ks ]  = generateF( 0, 4, 1.05,70,0,3 );
        ks{1}
        ks{2}
        ks{3}
        width=512;
        height=512;
        gfocal=(ks{1}(1,1)+ks{1}(2,2))/2;
        gocx=  ks{1}(1,3);
        gocy=ks{1}(2,3)  ;
        G=F;
        NMAT=((rand(3,3)-0.5)*i);
        NMAT(3,3)=0;
        
        G{1}=G{1} +NMAT ;
        [fcln, centerlocn] = S2nonlinsolveEssNfram(G,width,height)
        [fclt, centerloct] = S2nonlinsolveEsstwofram(G,width,height)
        errnfram(k,count)=abs(fcln(1)-gfocal)
        errtwofram(k,count)=abs(fclt(1)-gfocal)
        errnframOC(k,count)=sqrt(((centerlocn(1)-gocx)^2)+((centerlocn(2)-gocy)^2));
        errtwoframOC(k,count)=sqrt(((centerloct(1)-gocx)^2)+((centerloct(2)-gocy)^2));
        
    end
    count=count+1;
    count
end


plot([sum(errnfram)' sum(errtwofram)']);
title('focal length error comparison between N frame method and two frame clustering');
legend('N frame', ' two frame clustering');

figure
plot([sum(errnframOC)' sum(errtwoframOC)']);
title('optical center error comparison between N frame method and two frame clustering');
legend('N frame', ' two frame clustering');


%ok the noise is added in a bad way that it fuckes up th results