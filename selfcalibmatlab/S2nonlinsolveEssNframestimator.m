function [fcl, centerloc] =S2nonlinsolveEssNframestimator(TF,w,h)
%this function , given a camera center and a focal length and a series of fundamental
%matrices computes the error with respect to a fundamental matrix
%tic
if (nargin == 1)
    w=512;
    h=512;
end
%TF=TF*10000;
plotting=0;
fcl=[0 0];
maxfocal=2000;


xcen=0;
ycen =0;
centerloc=[xcen ycen];
[m,n]=size(TF);

numFs=n;

WEIGHTS=ones(numFs,1);

count=0;
petexs=0;
sturmfailed=0;
for i=1:numFs

    x = PeterSturmSelf( TF{1,i},w,h);

    if(x(1,1)>200 && x(1,1)<1600)
        count=count+1;
        petexs(count)=x(1,1);
    end

end

finit=mean(petexs);


if(finit>200 && finit<1600 && imag(finit)==0)
    finit=finit;
    fvari=150;
else
    sturmfailed=1;
end


xinit=w/2;
yinit=h/2;




%f = @(x)computerEssentialErrorSquared(x,TF); %squared



bestf=finit;
bestx=xinit;
besty=yinit;

if(strcmp(version('-release'),'14')==1)
    optionsfsolve  =optimset('Display','off','Jacobian','off','NonlEqnAlgorithm','lm','TolFun',1e-6,'TolX',1e-6);
else
    optionsfsolve    =optimset('Display','off','Jacobian','off','Algorithm','levenberg-marquardt','TolFun',1e-6,'TolX',1e-6);
end


for i=1:5

    if(sturmfailed==0)
        x0=[ bestf  bestx    besty ];
    else
        x0=[ (rand()*(maxfocal))  xinit  yinit ];
    end


    f = @(x)computerEssentialErrorSVDNFramesWeighted(x,TF,WEIGHTS);
    [x,fval,exitflag,output]  = fsolve(f ,x0,optionsfsolve);
    if(x(1)<0 || x(1)>maxfocal || x(2)<0 || x(2)>w || x(3)<0 || x(3)>h)
        ffinals=0;
        xfinals=0;
        yfinals=0;
    else
        ffinals=x(1);
        xfinals=x(2);
        yfinals=x(3);
    end;

    curscore=sum(abs(fval));

    bestf=x(1);
    bestx=x(2);
    besty=x(3);


    erFs=zeros(numFs,1);
    for j=1:numFs
        curer = computerEssentialErrorSVD(x,TF{j});
        erFs(j,1)=sum(abs(curer));

    end

    curMedian=median(erFs);
    MADN=median(abs(erFs-curMedian))/0.6745;
    DISTS= abs((erFs-curMedian)/MADN);

    for j=1:numFs
        if(DISTS(j,1) <1)
            WEIGHTS(j,1)=1;
        elseif(DISTS(j,1) >=1 && DISTS(j,1) <3)
            WEIGHTS(j,1)=1/DISTS(j,1) ;
        else
            WEIGHTS(j,1)=0;

        end
    end
    % DISTS
    % WEIGHTS
    %     %   put the robust weighing here
    % x
end





%toc
if(bestf>200 && bestf<1600 && imag(bestf)==0 && isnan(bestf)==0)
    bestf=bestf;
else
    bestf=w+h;
end

if(bestx<w && bestx>0 && imag(bestx)==0 && isnan(bestx)==0)
    bestx=bestx;
else
    bestx=w/2;
end

if(besty<w && besty>0 && imag(besty)==0 && isnan(besty)==0)
    besty=besty;
else
    besty=h/2;
end


fcl=[ bestf bestf];
xcen=bestx;
ycen=besty;
centerloc=[xcen ycen];

end

