function [ffinals,xfinals, yfinals] = nonlinsolveEsstwofram(TF)
%this function , given a camera center and a focal length and a fundamental
%matrix computes the error with respect to a fundamental matrix
tic
x = PeterSturmSelf( TF,512,512 );

if(x(1,1)>200 && x(1,1)<1600)
    finit=x(1,1);
else
    finit=512; %w+h
end
xinit=256;
yinit=256;
numtries=500;

f = @(x)computerEssentialError(x,TF);

ffinals=zeros(numtries,1);
xfinals=zeros(numtries,1);
yfinals=zeros(numtries,1);

bestf=0;
bestx=0;
besty=0;
bestscore=10000;
curscore=0;

options  =optimset('Display','off','Jacobian','on','NonlEqnAlgorithm','lm','TolX',1e-09,'TolFun',1e-09,'LineSearchType','cubicpoly');

for i=1:numtries
    x0=[ randn()*50+finit ; randn()*50+xinit ; randn()*50+yinit ];


    [x,fval,exitflag,output]  = fsolve(f ,x0,options);

    ffinals(i,1)=x(1);
    xfinals(i,1)=x(2);
    yfinals(i,1)=x(3);

    curscore=sum(abs(fval));
    if(curscore<bestscore)
        bestscore=curscore;

        bestf=x(1);
        bestx=x(2);
        besty=x(3);
    end
end


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

toc
end

