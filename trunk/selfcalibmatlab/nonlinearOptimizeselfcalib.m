function [focs, xcentrs, ycentrs,  scrs, bestF, bestX, bestY,focsinitial, xcentrsinitial, ycentrsinitial] = nonlinearOptimizeselfcalib(numtries, TF, w,h,Weights,finit,xinit,yinit,fvari, xvari, yvari,arinit,arvari)



constantinitials=0;
lowsearchrange=0;

%tolx and tolf are very important, for the data collection part use these
%values but later when you want more accuracy make them lower, like 10^-16

if(lowsearchrange==1)

    tolx=1e-7;
    tolf=1e-7;
else
    tolx=1e-19;
    tolf=1e-19;
end



if(strcmp(version('-release'),'14')==1)
    optionsfsolve  =optimset('Display','off','Jacobian','off','NonlEqnAlgorithm','lm','TolFun',tolf,'TolX',tolx);
else
    optionsfsolve    =optimset('Display','off','Jacobian','off','Algorithm','levenberg-marquardt','TolFun',tolf,'TolX',tolx);
end


maxfocal=1600;
minfocal=200;
maxbaditerations=5;
bestscore=1000000000000;

bestF=0;
bestX=0;
bestY=0;



focs=zeros(numtries,1);
xcentrs=zeros(numtries,1);
ycentrs=zeros(numtries,1);
scrs=zeros(numtries,1);

focsinitial=zeros(numtries,1);
xcentrsinitial=zeros(numtries,1);
ycentrsinitial=zeros(numtries,1);

f = @(x)computerEssentialErrorSVDNFramesWeighted(x,TF,Weights);

for i=1:numtries


    x=[-10 -10 -10 ];
    badxMaxcount=maxbaditerations;

    while(x(1)<minfocal || x(1)>maxfocal || x(2)<0 || x(2)>w || x(3)<0 || x(3)>h || imag(x(1))~=0   )
        badxMaxcount=badxMaxcount-1;
        if(constantinitials==1)
            x0=[ w  w/2  h/2  ];

        else

                frandcurrent=(rand()*(fvari-minfocal))+minfocal;
                x0=[ frandcurrent  (randn()*xvari)+xinit  (randn()*yvari)+yinit];

        end
       %x0
           focsinitial(i,1)=x0(1);
    xcentrsinitial(i,1)=x0(2);
    ycentrsinitial(i,1)=x0(3);
        [x,fval,exitflag,output]  = fsolve(f ,x0,optionsfsolve);

   

        if(badxMaxcount==0)
            x=[0 0 0 0];
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




end

