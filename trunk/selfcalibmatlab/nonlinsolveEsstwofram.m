function [fcl, centerloc] = nonlinsolveEsstwofram(TF)
%this function , given a camera center and a focal length and a fundamental
%matrix computes the error with respect to a fundamental matrix
tic



fcl=[0 0];

xcen=0;
ycen =0;
centerloc=[xcen ycen];

x = PeterSturmSelf( TF);

if(x(1,1)>200 && x(1,1)<1600)
    finit=x(1,1);
else
    finit=512; %w+h
end


xinit=256;
yinit=256;
numtries=100;

fvari=50;
xvari=50;
yvari=50;

f = @(x)computerEssentialError(x,TF);

ffinals=zeros(numtries,1);
xfinals=zeros(numtries,1);
yfinals=zeros(numtries,1);

bestf=0;
bestx=0;
besty=0;
bestscore=1000;
curscore=0;

optionsfsolve  =optimset('Display','off','Jacobian','on','Algorithm','levenberg-marquardt','TolFun',1e-6,'TolX',1e-6);

optionslsqnonlin  =optimset('Display','off','Jacobian','on');


for i=1:numtries
    x0=[ (((rand()-0.5)*2)*fvari)+finit ; (((rand()-0.5)*2)*xvari)+xinit ; (((rand()-0.5)*2)*yvari)+yinit ];
    
    
     % [x,fval,exitflag,output]  = fsolve(f ,x0,optionsfsolve);
    [x,resnorm,fval,exitflag] = lsqnonlin(f,x0,[finit-150; 190; 190],[finit+150; 350; 350],optionslsqnonlin);
    ffinals(i,1)=x(1);
    xfinals(i,1)=x(2);
    yfinals(i,1)=x(3);
    
    curscore=sum((fval.^2));
    if(curscore<bestscore)
        bestscore=curscore;
        %  disp(['iteration ' num2str(i)]);
        bestf=x(1);
        bestx=x(2);
        besty=x(3);
        disp(['iteration ' num2str(i) ' best f is ' num2str(bestf) ' and best x = ' num2str(bestx) ' and best y is ' num2str(besty) ' and score was ' num2str(curscore)]);
        
        
    end
end



%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%




hist(ffinals,numtries/2);
title(['focal length']);
figure

hist(xfinals,numtries/2);
title(['xcomponent of camera center']);
figure

hist(yfinals,numtries/2);
title(['ycomponent of camera center']);

display(['mean of f was ' num2str(mean(ffinals)) ' median of f was ' num2str(median(ffinals)) ' variance of f was ' num2str(var(ffinals))]);
display(['mean of x was ' num2str(mean(xfinals)) ' median of x was ' num2str(median(xfinals)) ' variance of x was ' num2str(var(xfinals))]);
display(['mean of y was ' num2str(mean(yfinals)) ' median of y was ' num2str(median(yfinals)) ' variance of y was ' num2str(var(yfinals))]);
display(['best f is ' num2str(bestf) ' and best x is ' num2str(bestx) ' and best y is ' num2str(besty)]);

%
%

fcl=[ bestf bestf];
xcen=bestx;
ycen=besty;
centerloc=[xcen ycen];
toc
end

