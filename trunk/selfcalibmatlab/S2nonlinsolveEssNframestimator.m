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
fvari=300;
xvari=50;
yvari=50;

xcen=0;
ycen =0;
centerloc=[xcen ycen];
[m,n]=size(TF);

numFs=n;


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

psstd=sqrt(var(petexs));
finit=mean(petexs);

if(psstd<150)
    psstd=150;
end


if(finit>200 && finit<1600 && imag(finit)==0)
    finit=finit;
    fvari=150;
else
    sturmfailed=1;
end


xinit=w/2;
yinit=h/2;
numtries=200;



%f = @(x)computerEssentialErrorSquared(x,TF); %squared
f = @(x)computerEssentialErrorSVDNFrames(x,TF);

ffinals=zeros(numtries,1);
xfinals=zeros(numtries,1);
yfinals=zeros(numtries,1);
scorearray=zeros(numtries,1);

bestf=0;
bestx=0;
besty=0;
bestscore=1000000000000;
curscore=0;

% optionsfsolve  =optimset('Display','off','Jacobian','off','NonlEqnAlgorithm','lm','TolFun',1e-6,'TolX',1e-6);
optionsfsolve    =optimset('Display','off','Jacobian','off','Algorithm','levenberg-marquardt','TolFun',1e-6,'TolX',1e-6);



updatelimit=ceil(numtries/5);
countnoupdate=0;
for i=1:numtries
    countnoupdate=countnoupdate+1;
    
    if(sturmfailed==0)
        x0=[ (randn()*fvari)+finit  (randn()*xvari)+xinit  (randn()*yvari)+yinit ];
    else
        x0=[ (rand()*(maxfocal))  (randn()*xvari)+xinit  (randn()*yvari)+yinit ];
    end
    
    
    %[x,fval,exitflag,output]  = fsolve(f ,x0,optionsfsolve);
    [x,fval,exitflag,output]  = fsolve(f ,x0,optionsfsolve);
    if(x(1)<0 || x(1)>maxfocal || x(2)<0 || x(2)>w || x(3)<0 || x(3)>h)
        ffinals(i,1)=0;
        xfinals(i,1)=0;
        yfinals(i,1)=0;
    else
        ffinals(i,1)=x(1);
        xfinals(i,1)=x(2);
        yfinals(i,1)=x(3);
    end;
    
    curscore=sum(abs(fval));
    
    [svScore, detScore, EssScore, EssScoreIA ]= EvalErrorParams1(TF{1},x(1),x(1),x(2),x(3),x(2),x(3) );
    curscore=EssScore;
    
    % disp(['iteration ' num2str(i) ' started from f= ' num2str(x0(1,1)) ' x= ' num2str(x0(1,2)) ' and y= ' num2str(x0(1,3))]);
    %  disp(['iteration ' num2str(i) ' best f is ' num2str(x(1)) ' and best x = ' num2str(x(2)) ' and best y is ' num2str(x(3)) ' and score was ' num2str(curscore) ' det score was ' num2str(detScore) ' SV score was ' num2str(svScore) ' and ess score was ' num2str(EssScore) ' IA score is ' num2str( EssScoreIA)]);
    
    scorearray(i,1)=curscore;
    if(curscore<bestscore && imag(x(1))==0 && x(1)>200 && x(1)<1600 )
        countnoupdate=0;
        bestscore=curscore;
        %  disp(['iteration ' num2str(i)]);
        bestf=x(1);
        bestx=x(2);
        besty=x(3);
        %   x,resnorm,fval,exitflag
        %   disp(['**BEST: iteration ' num2str(i) ' best f is ' num2str(x(1)) ' and best x = ' num2str(x(2)) ' and best y is ' num2str(x(3)) ' and score was ' num2str(curscore) ' det score was ' num2str(detScore) ' SV score was ' num2str(svScore) ' and ess score was ' num2str(EssScore) ' IA score is ' num2str( EssScoreIA)]);
        
    end
    
    
    
end

x
bests=[ bestf  bestx  besty    ]
erFs=zeros(numFs,1);
for i=1:numFs
    curer = computerEssentialErrorSVD(bests,TF{i});
    erFs(i,1)=sum(abs(curer));
    
end

erFs





%toc
if(bestf>200 && bestf<1600 && imag(bestf)==0)
    bestf=bestf;
else
    bestf=w+h;
end


fcl=[ bestf bestf];
xcen=bestx;
ycen=besty;
centerloc=[xcen ycen];

end

