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
numtries=200;

fvari=50;
xvari=50;
yvari=50;

%f = @(x)computerEssentialErrorSquared(x,TF); %squared
f = @(x)computerEssentialError(x,TF);

ffinals=zeros(numtries,1);
xfinals=zeros(numtries,1);
yfinals=zeros(numtries,1);
scorearray=zeros(numtries,1);

bestf=0;
bestx=0;
besty=0;
bestscore=2000;
curscore=0;

optionsfsolve  =optimset('Display','off','Jacobian','on','Algorithm','levenberg-marquardt','TolFun',1e-6,'TolX',1e-6);

optionslsqnonlin  =optimset('Display','off','Jacobian','on');


for i=1:numtries
    x0=[ (randn()*fvari)+finit ; (randn()*xvari)+xinit ; (randn()*yvari)+yinit ];
    
    
    [x,fval,exitflag,output]  = fsolve(f ,x0,optionsfsolve);
    % [x,resnorm,fval,exitflag] = lsqnonlin(f,x0,[finit-150; 190; 190],[finit+150; 350; 350],optionslsqnonlin);
    ffinals(i,1)=x(1);
    xfinals(i,1)=x(2);
    yfinals(i,1)=x(3);
    
    curscore=sum(abs(fval));
    
     [svScore, detScore, EssScore ] = EvalErrorParams1(TF,x(1),x(1),x(2),x(3),x(2),x(3) );
%     curscore=detScore;   
    
       
   
       
       scorearray(i,1)=curscore;
    if(curscore<bestscore)
        bestscore=curscore;
        %  disp(['iteration ' num2str(i)]);
        bestf=x(1);
        bestx=x(2);
        besty=x(3);
        disp(['iteration ' num2str(i) ' best f is ' num2str(bestf) ' and best x = ' num2str(bestx) ' and best y is ' num2str(besty) ' and score was ' num2str(curscore) ' det score was ' num2str(detScore) ' SV score was ' num2str(svScore) ' and ess score was ' num2str(EssScore)]);
        
        
    end
end



%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
sizearray=scorearray;
scoremedian=median(scorearray);
for i=1:numtries
    if(sizearray(i,1)>scoremedian)
        sizearray(i,1)=1;
    else
        sizearray(i,1)=ceil(10+(sizearray(i,1)*(-9/scoremedian)));
    end
end


scatter(ffinals, scorearray);
title(['focal lengths versus energy']);
xlabel('focal length');
ylabel('value of the energy function ');

figure;

scatter(ffinals, ln(scorearray));
title(['log scale focal lengths versus energy']);
xlabel('focal length');
ylabel('value of the energy function ');

figure;


scatter3(xfinals,yfinals,ffinals, sizearray , 'r')
title(['all the local minima points scatter plot']);
xlabel('x coordinates of optical centers');
ylabel('y coordinates of optical centers');
zlabel('value of focal length ');
figure

plot3(xfinals,yfinals,ffinals,'r.','MarkerSize',12)
title(['all the local minima points']);
xlabel('x coordinates of optical centers');
ylabel('y coordinates of optical centers');
zlabel('value of focal length ');
figure

numclusts=10;
X=[ ffinals  xfinals   yfinals];
[idx,ctrs] = kmeans(X,numclusts,'Replicates',5);

scatter3(X(:,1),X(:,2),X(:,3),100,idx,'filled')

maxnumclusts=0;
maxnumclustsF=0;
maxnumclustsX=0;
maxnumclustsY=0;

for i=1:numclusts
    cursize=sum(idx==i);
    
    if(cursize>maxnumclusts)
        maxnumclusts=cursize;
        maxnumclustsF=ctrs(i,1);
        maxnumclustsX=ctrs(i,2);
        maxnumclustsY=ctrs(i,3);
    end
end


figure


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
display([' the highest cluster number was ' num2str(maxnumclusts) ' whose f is  ' num2str(maxnumclustsF) ' X is  ' num2str(maxnumclustsX) ' Y is  ' num2str( maxnumclustsY)  ]);


%
%

fcl=[ bestf bestf];
xcen=bestx;
ycen=besty;
centerloc=[xcen ycen];
toc
end
