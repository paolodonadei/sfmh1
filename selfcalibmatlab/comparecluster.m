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

nowtime=num2str(sum(round(100*clock)));
tic

repeat=2;



width=512;
height=512;

numframes=3;
centerdev=50;
numBADFr=1;
skew=0;
framediff=0;
AR=1;

steps=0.05;
finalr=1;

count=1;

identi=['numframes: ' num2str(numframes) ' AR: ' num2str(AR) ' framediff: ' num2str(framediff) ' skew : ' num2str(skew) ' bad frames: ' num2str(numBADFr) ' centerdev: ' num2str(centerdev)  ' repeat: ' num2str(repeat)];

for i=0:steps:finalr
    
    for k=1:repeat
        %  change this to the new F generation
        clear G F ks;
        
        [ F, ks ]  = generateF( framediff, skew, AR,rand()*centerdev,0,numframes,i,numBADFr );
        %         ks{1}
        %         ks{2}
        %         ks{3}
        [nn, numfunds]=size(F);
        
        
        gfocal=(ks{1,1}(1,1)+ks{1,1}(2,2))/2;
        gocx=  ks{1,1}(1,3);
        gocy=ks{1,1}(2,3)  ;
        G=F;
        
        
        [fcln, centerlocn]      = S2nonlinsolveEssNfram(G,width,height)
        [fclt, centerloct]      = S2nonlinsolveEsstwofram(G,width,height)
        [fcld, centerlocdiag]   = S2nonlinsolveEssNframdiagnostics(G,width,height)
        [fclm, centerlocM]      = S2nonlinsolveEssNframestimator(G,width,height)
        [ sln, centerloc ]      = PeterSturmSelfRobust( G,width,height )
        [ fclR, centerlocR ]      =  S2nonlinsolveEssRansac( G,width,height )
        
        
        
        errps(k,count)=abs(sln(1)-gfocal);
        errnfram(k,count)=abs(fcln(1)-gfocal);
        errtwofram(k,count)=abs(fclt(1)-gfocal);
        errdiagn(k,count)=abs(fcld(1)-gfocal);
        errM(k,count)=abs(fclm(1)-gfocal);
        errR(k,count)=abs(fclR(1)-gfocal);
        
        errpssum= sum( errps)
        errnframsum= sum(errnfram)
        errtwoframsum= sum(errtwofram)
        errdiagnsum= sum( errdiagn)
        errMsum= sum(errM)
        errRsum= sum(errR)
        
        errnframOC(k,count)=sqrt(((centerlocn(1)-gocx)^2)+((centerlocn(2)-gocy)^2));
        errtwoframOC(k,count)=sqrt(((centerloct(1)-gocx)^2)+((centerloct(2)-gocy)^2));
        errdiagOC(k,count)=sqrt(((centerlocdiag(1)-gocx)^2)+((centerlocdiag(2)-gocy)^2));
        errMOC(k,count)=sqrt(((centerlocM(1)-gocx)^2)+((centerlocM(2)-gocy)^2));
        errROC(k,count)=sqrt(((centerlocR(1)-gocx)^2)+((centerlocR(2)-gocy)^2));
        
        disp(['display iteration ' num2str(k+((count-1)*repeat))  ]);
    end
    count=count+1;
    
end

xaxis=0:steps:finalr;

plot(xaxis,(errnframsum/repeat)',xaxis, (errtwoframsum/repeat)',xaxis, (errpssum/repeat)',xaxis, (errdiagnsum/repeat)',xaxis, (errMsum/repeat)',xaxis, (errRsum/repeat)');

title({'focal length error comparison between N frame method and two frame clustering';identi})


legend('N frame', ' two frame clustering' , 'error peter sturm', 'case deletion', 'M-estimator', 'RANSAC');
saveas(gcf,['erroFlength_' nowtime '.png']);
saveas(gcf,['erroFlength_' nowtime '.eps'],'epsc');
figure
plot(xaxis,(sum(errnframOC)/repeat)',xaxis, (sum(errtwoframOC)/repeat)',xaxis, (sum(errdiagOC)/repeat)',xaxis,  (sum(errMOC)/repeat)',xaxis,  (sum(errROC)/repeat)');

title({'optical center error comparison between N frame method and two frame clustering';identi})

legend('N frame', ' two frame clustering', ' case deletion', ' M-Estimator','RANSAC');
saveas(gcf,['erroOC_' nowtime '.png']);
saveas(gcf,['erroOC_' nowtime '.eps'],'epsc');

save( ['variables_' nowtime '.mat'])

toc
%ok the noise is added in a bad way that it fuckes up th results