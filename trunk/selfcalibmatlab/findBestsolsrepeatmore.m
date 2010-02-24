function [focs, xcentrs, ycentrs, ars, scrs, bestF, bestX, bestY,bestAR] = findBestsolsrepeatmore(numtries, TF, w,h,Weights,finit,xinit,yinit,fvari, xvari, yvari,arinit)

[m,numFs]=size(TF);

constantinitials=0;

%tolx and tolf are very important, for the data collection part use these
%values but later when you want more accuracy make them lower, like 10^-16

if(constantinitials==1)
    
    tolx=1e-8;
    tolf=1e-8;
else
    tolx=1e-14;
    tolf=1e-14;
end


if(strcmp(version('-release'),'14')==1)
    optionsfsolve  =optimset('Display','off','Jacobian','off','NonlEqnAlgorithm','lm','TolFun',tolf,'TolX',tolx);
else
    optionsfsolve    =optimset('Display','off','Jacobian','off','Algorithm','levenberg-marquardt','TolFun',tolf,'TolX',tolx);
end

sturmfailed=0;
maxfocal=2000;
minfocal=200;
maxbaditerations=10;
bestscore=1000000000000;

bestF=0;
bestX=0;
bestY=0;
bestAR=0;


focs=zeros(numtries,1);
xcentrs=zeros(numtries,1);
ycentrs=zeros(numtries,1);
ars=zeros(numtries,1);
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
    
    arinit=1;
    arvari=0.02;
end


arvari=0.01;

sturmfailed=1;
fvari=(maxfocal-minfocal);


f = @(x)computerEssentialErrorSVDNFramesWeighted(x,TF,Weights);

for i=1:numtries
    
    
    x=[-10 -10 -10 -10];
    badxMaxcount=maxbaditerations;
    cur_ar=x(1)/x(4);
    
    while(x(1)<minfocal || x(1)>maxfocal || x(2)<0 || x(2)>w || x(3)<0 || x(3)>h || imag(x(1))~=0 || cur_ar<0.6 || cur_ar>1.4  )
        badxMaxcount=badxMaxcount-1;
        if(constantinitials==1)
            x0=[ w+h  w/2  h/2 w+h ];
            
        else
            if(sturmfailed==0 && fvari<200)
                frandcurrent=(randn()*fvari)+finit;
                x0=[frandcurrent  (randn()*xvari)+xinit  (randn()*yvari)+yinit frandcurrent*((randn()*arvari)+ arinit)];
            else
                frandcurrent=(rand()*fvari)+minfocal;
                x0=[ frandcurrent  (randn()*xvari)+xinit  (randn()*yvari)+yinit  frandcurrent*((randn()*arvari)+ arinit)];
            end
        end
              %  x0
        
        [x,fval,exitflag,output]  = fsolve(f ,x0,optionsfsolve);
        
        cur_ar=x(4)/x(1);
        
        if(badxMaxcount==0)
            x=[0 0 0 0];
            break;
        end
    end
    
    focs(i,1)=x(1);
    xcentrs(i,1)=x(2);
    ycentrs(i,1)=x(3);
    if(x(1)>eps)
        ars(i,1)=x(4)/x(1);
    else
        ars(i,1)=1;
    end
    scrs(i,1)=sum(abs(fval));
    
    if(scrs(i,1)<bestscore && imag(x(1))==0 && imag(x(2))==0 &&  imag(x(3))==0 )
        
        bestscore=scrs(i,1);
        
        bestF=x(1);
        bestX=x(2);
        bestY=x(3);
        
        if(x(1)>eps)
            bestAR=x(4)/x(1);
        else
            bestAR=0;
        end
        
        
        
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

