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

repeat=5;

count=1;

for i=0:0.05:1

    for k=1:repeat
        change this to the new F generation
        
        [ F, ks ]  = generateF( 0, 0, 1.00,rand()*80,0,5 );
        %         ks{1}
        %         ks{2}
        %         ks{3}
        [nn, numfunds]=size(F);
        fundflip=ceil((rand()*(numfunds)));
        
        width=512;
        height=512;
        gfocal=(ks{1,1}(1,1)+ks{1,1}(2,2))/2;
        gocx=  ks{1,1}(1,3);
        gocy=ks{1,1}(2,3)  ;
        G=F;
        NMAT=((rand(3,3)-0.5)*i*(abs(sum(sum((G{1,fundflip}))))));
        NMAT(3,3)=0;

        G{1,fundflip}=G{1,fundflip} +NMAT ;
        
        [fcln, centerlocn]      = S2nonlinsolveEssNfram(G,width,height)
        [fclt, centerloct]      = S2nonlinsolveEsstwofram(G,width,height)
        [fcld, centerlocdiag]   = S2nonlinsolveEssNframdiagnostics(G,width,height)
        [fclm, centerlocM]      = S2nonlinsolveEssNframestimator(G,width,height)
        [ sln, centerloc ]      = PeterSturmSelfRobust( G,width,height )

        errps(k,count)=abs(sln(1)-gfocal);
        errnfram(k,count)=abs(fcln(1)-gfocal);
        errtwofram(k,count)=abs(fclt(1)-gfocal);
        errdiagn(k,count)=abs(fcld(1)-gfocal);
        errM(k,count)=abs(fclm(1)-gfocal);


        sum( errps)
        sum(errnfram)
        sum(errtwofram)
        sum( errdiagn)
        sum(errM)

        errnframOC(k,count)=sqrt(((centerlocn(1)-gocx)^2)+((centerlocn(2)-gocy)^2));
        errtwoframOC(k,count)=sqrt(((centerloct(1)-gocx)^2)+((centerloct(2)-gocy)^2));
        errdiagOC(k,count)=sqrt(((centerlocdiag(1)-gocx)^2)+((centerlocdiag(2)-gocy)^2));
        errMOC(k,count)=sqrt(((centerlocM(1)-gocx)^2)+((centerlocM(2)-gocy)^2));

        disp(['display iteration ' num2str(k+((count-1)*repeat))  ]);
    end
    count=count+1;

end


plot([sum(errnfram)' sum(errtwofram)' sum(errps)' sum(errdiagn)' sum(errM)']);
title('focal length error comparison between N frame method and two frame clustering');
legend('N frame', ' two frame clustering' , 'error peter sturm', 'case deletion', 'M-estimator');
saveas(gcf,['erroFlength_' nowtime '.png']);
saveas(gcf,['erroFlength_' nowtime '.eps']);
figure
plot([sum(errnframOC)' sum(errtwoframOC)' sum(errdiagOC)'  sum(errMOC)']);
title('optical center error comparison between N frame method and two frame clustering');
legend('N frame', ' two frame clustering', ' case deletion', ' M-Estimator');
saveas(gcf,['erroOC_' nowtime '.png']);
saveas(gcf,['erroOC_' nowtime '.eps']);

save( ['variables_' nowtime '.m'])

toc
%ok the noise is added in a bad way that it fuckes up th results