function [fcl, centerloc] = S2nonlinsolveEssNfram(TF,w,h)
%this function , given a camera center and a focal length and a series of fundamental
%matrices computes the error with respect to a fundamental matrix
%tic

%TF=TF*10000;

fcl=[0 0];

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
    finit=w+h;
    fvari=(w+h)/2;
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

optionsfsolve  =optimset('Display','off','Jacobian','off','NonlEqnAlgorithm','lm','TolFun',1e-6,'TolX',1e-6);

updatelimit=ceil(numtries/5);
countnoupdate=0;
for i=1:numtries
    countnoupdate=countnoupdate+1;
    initbeginF=(randn()*fvari)+finit;
    
    while(initbeginF<200 || initbeginF>1500)
        initbeginF=(randn()*fvari)+finit;
    end
    
    
    x0=[ initbeginF  (randn()*xvari)+xinit  (randn()*yvari)+yinit ];
    
    
    %[x,fval,exitflag,output]  = fsolve(f ,x0,optionsfsolve);
    [x,fval,exitflag,output]  = fsolve(f ,x0,optionsfsolve);
    ffinals(i,1)=x(1);
    xfinals(i,1)=x(2);
    yfinals(i,1)=x(3);
    
    curscore=sum(abs(fval));
    
    [svScore, detScore, EssScore, EssScoreIA ]= EvalErrorParams1(TF{1},x(1),x(1),x(2),x(3),x(2),x(3) );
              curscore=EssScore;
    
    % disp(['iteration ' num2str(i) ' started from f= ' num2str(x0(1,1)) ' x= ' num2str(x0(1,2)) ' and y= ' num2str(x0(1,3))]);
    disp(['iteration ' num2str(i) ' best f is ' num2str(x(1)) ' and best x = ' num2str(x(2)) ' and best y is ' num2str(x(3)) ' and score was ' num2str(curscore) ' det score was ' num2str(detScore) ' SV score was ' num2str(svScore) ' and ess score was ' num2str(EssScore) ' IA score is ' num2str( EssScoreIA)]);
    
    scorearray(i,1)=curscore;
    if(curscore<bestscore && imag(x(1))==0 && x(1)>200 && x(1)<1600 )
        countnoupdate=0;
        bestscore=curscore;
        %  disp(['iteration ' num2str(i)]);
        bestf=x(1);
        bestx=x(2);
        besty=x(3);
        %   x,resnorm,fval,exitflag
        disp(['**BEST: iteration ' num2str(i) ' best f is ' num2str(x(1)) ' and best x = ' num2str(x(2)) ' and best y is ' num2str(x(3)) ' and score was ' num2str(curscore) ' det score was ' num2str(detScore) ' SV score was ' num2str(svScore) ' and ess score was ' num2str(EssScore) ' IA score is ' num2str( EssScoreIA)]);
        
    end
    
 
    
end



%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
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

scatter(ffinals, log(scorearray));
title(['log scale focal lengths versus energy']);
xlabel('focal length');
ylabel('value of the energy function ');

figure;


scatter3(xfinals,yfinals,ffinals, 3 , 'r')
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
%
% numclusts=10;
% X=[ ffinals  xfinals   yfinals];
% [idx,ctrs] = kmeans(X,numclusts,'Replicates',5);
%
% scatter3(X(:,1),X(:,2),X(:,3),100,idx,'filled');
% title(['scatter plot of all the results found using my method and those found by sweeping the peter sturm algorithm']);
% hold
% count=0;
% for i=230:350
% for j=230:350
%     count=count+1;
% [ x, centerloc ] = PeterSturmSelfmoreparams( TF, i, j );
% XF(count,1)=x(1,1);
% XF(count,2)=i;
% XF(count,3)=j;
% end
% end
% scatter3(XF(:,1),XF(:,2),XF(:,3),100,'r','filled');
% hold
%
%
% maxnumclusts=0;
% maxnumclustsF=0;
% maxnumclustsX=0;
% maxnumclustsY=0;
%
% for i=1:numclusts
%     cursize=sum(idx==i);
%
%     if(cursize>maxnumclusts)
%         maxnumclusts=cursize;
%         maxnumclustsF=ctrs(i,1);
%         maxnumclustsX=ctrs(i,2);
%         maxnumclustsY=ctrs(i,3);
%     end
% end
%
%
% figure
%
%
hist(ffinals,numtries/2);
title(['focal length']);
figure

hist(xfinals,numtries/2);
title(['xcomponent of camera center']);
figure

hist(yfinals,numtries/2);
title(['ycomponent of camera center']);
%
% display(['mean of f was ' num2str(mean(ffinals)) ' median of f was ' num2str(median(ffinals)) ' variance of f was ' num2str(var(ffinals))]);
% display(['mean of x was ' num2str(mean(xfinals)) ' median of x was ' num2str(median(xfinals)) ' variance of x was ' num2str(var(xfinals))]);
% display(['mean of y was ' num2str(mean(yfinals)) ' median of y was ' num2str(median(yfinals)) ' variance of y was ' num2str(var(yfinals))]);
% display(['best f is ' num2str(bestf) ' and best x is ' num2str(bestx) ' and best y is ' num2str(besty)]);
% display([' the highest cluster number was ' num2str(maxnumclusts) ' whose f is  ' num2str(maxnumclustsF) ' X is  ' num2str(maxnumclustsX) ' Y is  ' num2str( maxnumclustsY)  ]);



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

