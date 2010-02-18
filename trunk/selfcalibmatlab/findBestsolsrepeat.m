function [focs, xcentrs, ycentrs, scrs, bestF, bestX, bestY] = findBestsolsrepeat(numtries, TF, w,h,Weights,finit,xinit,yinit,fvari, xvari, yvari)

[m,numFs]=size(TF);


%tolx and tolf are very important, for the data collection part use these
%values but later when you want more accuracy make them lower, like 10^-16
if(strcmp(version('-release'),'14')==1)
    optionsfsolve  =optimset('Display','off','Jacobian','off','NonlEqnAlgorithm','lm','TolFun',1e-18,'TolX',1e-18);
else
    optionsfsolve    =optimset('Display','off','Jacobian','off','Algorithm','levenberg-marquardt','TolFun',1e-18,'TolX',1e-18);
end

sturmfailed=0;
maxfocal=2000;
minfocal=200;
maxbaditerations=10;
bestscore=1000000000000;
bestF=0;
bestX=0;
bestY=0;


focs=zeros(numtries,1);
xcentrs=zeros(numtries,1);
ycentrs=zeros(numtries,1);
scrs=zeros(numtries,1);

if (nargin < 5)
    Weights=ones(numFs,1);
end


if (nargin <6)

    xvari=50;
    yvari=50;

    x = PeterSturmSelfRobust( TF,w,h );
    finit=x(1,1);
    if(finit>minfocal && finit<maxfocal && imag(finit)==0)
        finit=finit;
        fvari=30;
    else
        sturmfailed=1;
        fvari=(maxfocal-minfocal);
    end
    xinit=w/2;
    yinit=h/2;
end

  sturmfailed=1;
        fvari=(maxfocal-minfocal);

f = @(x)computerEssentialErrorSVDNFramesWeighted(x,TF,Weights);

for i=1:numtries


    x=[-10 -10 -10];
    badxMaxcount=maxbaditerations;
    while(x(1)<minfocal || x(1)>maxfocal || x(2)<0 || x(2)>w || x(3)<0 || x(3)>h || imag(x(1))~=0)
        badxMaxcount=badxMaxcount-1;
        if(sturmfailed==0 && fvari<200)
            x0=[ (randn()*fvari)+finit  (randn()*xvari)+xinit  (randn()*yvari)+yinit ];
        else
            x0=[ (rand()*fvari)+minfocal  (randn()*xvari)+xinit  (randn()*yvari)+yinit ];
        end
        [x,fval,exitflag,output]  = fsolve(f ,x0,optionsfsolve);
       x0
       x
        if(badxMaxcount==0)
            x=[0 0 0];
            break;
        end
    end

    focs(i,1)=x(1);
    xcentrs(i,1)=x(2);
    ycentrs(i,1)=x(3);
    scrs(i,1)=sum(abs(fval));

    if(scrs(i,1)<bestscore && imag(x(1))==0 && imag(x(2))==0 &&  imag(x(3))==0 )

        bestscore=scrs(i,1);

        bestF=x(1);
        bestX=x(2);
        bestY=x(3);

    end



end

if(bestF>200 && bestF<1600 && imag(bestF)==0 && isnan(bestF)==0)
    bestF=bestF;
else
    bestF=w+h;
end

if(bestX<w && bestX>0 && imag(bestX)==0 && isnan(bestX)==0)
    bestX=bestX;
else
    bestX=w/2;
end

if(bestY<w && bestY>0 && imag(bestY)==0 && isnan(bestY)==0)
    bestY=bestY;
else
    bestY=h/2;
end




end

