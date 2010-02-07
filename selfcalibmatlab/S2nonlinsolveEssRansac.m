function [fcl, centerloc] = S2nonlinsolveEssRansac(TF,w,h)
%this function , given a camera center and a focal length and a fundamental
%matrix computes the error with respect to a fundamental matrix
%tic
if (nargin == 1)
    w=512;
    h=512;
end
%TF=TF*10000;
plotting=0;
maxfocal=2000;
fcl=[0 0];

xcen=0;
ycen =0;
centerloc=[xcen ycen];

[m,n]=size(TF);

sizeFs=n;


numtries=10;


sturmfailed=0;

if(strcmp(version('-release'),'14')==1)
    optionsfsolve  =optimset('Display','off','Jacobian','off','NonlEqnAlgorithm','lm','TolFun',1e-6,'TolX',1e-6);
else
    optionsfsolve    =optimset('Display','off','Jacobian','off','Algorithm','levenberg-marquardt','TolFun',1e-6,'TolX',1e-6);
end

xinit=w/2;
yinit=h/2;


fvari=70;
xvari=70;
yvari=70;

solutions=cell(numtries,sizeFs);
x = PeterSturmSelfRobust( TF,w,h );



if(x(1,1)>200 && x(1,1)<1600)
    finit=x(1,1);
else
    sturmfailed=1;
end


for q=1:sizeFs
    
    
    
    
    
    
    
    f = @(x)computerEssentialErrorSVD(x,TF{q});
    
    
    for i=1:numtries
        if(sturmfailed==0)
            x0=[ (randn()*fvari)+finit  (randn()*xvari)+xinit  (randn()*yvari)+yinit ];
        else
            x0=[ (rand()*(maxfocal))  (randn()*xvari)+xinit  (randn()*yvari)+yinit ];
            
        end
        
        x=[-10 -10 -10];
        
        badxMaxcount=10;
        while(x(1)<0 || x(1)>maxfocal || x(2)<0 || x(2)>w || x(3)<0 || x(3)>h)
            badxMaxcount=badxMaxcount-1;
            [x,fval,exitflag,output]  = fsolve(f ,x0,optionsfsolve);
            if(badxMaxcount==0)
                x=[0 0 0];
                break;
            end
        end
        
        ffinals=x(1);
        xfinals=x(2);
        yfinals=x(3);
        
        
        solutions{i,q}=[ffinals xfinals yfinals];
               disp([' solution at F number ' num2str(q) ' and numtries ' num2str(i) ' gives us:']);
                x
        
    end
    
end

rawscores=zeros(sizeFs,sizeFs);
scoresfinal=zeros(sizeFs,sizeFs);

for q=1:sizeFs
    for m=1:sizeFs
        clear tempVect;
        for i=1:numtries
            tempVect(i)=  computerEssentialErrorSVD( solutions{i,q},TF{m});
        end
        
        rawscores(m,q)=median(tempVect(i));
        
        
    end
end



curMedian=median(reshape(rawscores,sizeFs*sizeFs,1));
MADN=median(abs(reshape(rawscores,sizeFs*sizeFs,1)-curMedian))/0.6745;
DISTS= abs(( rawscores-curMedian)/MADN);


% robust score function

threshold=3;

for q=1:sizeFs
    
    for m=1:sizeFs
        if(DISTS(q,m)<threshold)
            scoresfinal(q,m)=DISTS(q,m);
        else
            scoresfinal(q,m)=threshold;
        end
    end
end


[m,idx]=min(sum(scoresfinal));


count=1;

FFinal{count}=TF{idx};
disp(['using frame ' num2str(idx)]);
for q=1:sizeFs
    if(scoresfinal(q,idx)<threshold && q~=idx)
        disp(['using frame ' num2str(q)]);
        count=count+1;
        FFinal{count}=TF{q};
    end
end

 something not quite right here, check this out
 
 
[x,fval,exitflag,output]  = fsolve(f ,[ finit  xinit  yinit ],optionsfsolve);

bestf =x(1) ;
bestx=x(2);
besty=x(3);






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

