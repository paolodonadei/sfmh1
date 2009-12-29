function [ffinals,xfinals, yfinals] = nonlinsolveEsstwofram(TF)
%this function , given a camera center and a focal length and a fundamental
%matrix computes the error with respect to a fundamental matrix
x = PeterSturmSelf( TF,512,512 );

finit=x(1,1);
xinit=256;
yinit=256;
numtries=50;

f = @(x)computerEssentialError(x,TF);

ffinals=zeros(numtries,1);
xfinals=zeros(numtries,1);
yfinals=zeros(numtries,1);


options  =optimset('Display','iter','Jacobian','off','NonlEqnAlgorithm','lm');

for i=1:numtries
    x0=[ randn()*50+finit ; randn()*50+xinit ; randn()*50+yinit ];


    [x,resnorm,residual,exitflag] = fsolve(f ,x0,options);

    ffinals(i,1)=x(1);
    xfinals(i,1)=x(2);
    yfinals(i,1)=x(3);

end


hist(ffinals,numtries/2);
title(['focal length']);
figure

hist(xfinals,numtries/2);
title(['xcomponent of camera center']);
figure

hist(yfinals,numtries/2);
title(['ycomponent of camera center']);

end

