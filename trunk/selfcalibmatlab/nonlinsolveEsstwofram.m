function [fcl, centerloc] = nonlinsolveEsstwofram(TF,w,h)
%this function , given a camera center and a focal length and a fundamental
%matrix computes the error with respect to a fundamental matrix
%tic

%TF=TF*10000;

fcl=[0 0];

xcen=0;
ycen =0;
centerloc=[xcen ycen];

x = PeterSturmSelf( TF,w,h);

if(x(1,1)>200 && x(1,1)<1600)
    finit=x(1,1);
else
    finit=w+h;
end


xinit=w/2;
yinit=h/2;
numtries=200;

fvari=70;
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
bestscore=1000000000000;
curscore=0;

optionsfsolve  =optimset('Display','off','Jacobian','on','Algorithm','levenberg-marquardt','TolFun',1e-6,'TolX',1e-6);

optionslsqnonlin  =optimset('Display','off','Jacobian','on');


for i=1:numtries
    x0=[ (randn()*fvari)+finit  (randn()*xvari)+xinit  (randn()*yvari)+yinit ];
    
    
   % [x,fval,exitflag,output]  = fsolve(f ,x0,optionsfsolve);
     [x,resnorm,fval,exitflag] = lsqnonlin(f,x0,[finit-150 ((w/2)-60) ((h/2)-60) ],[finit+150 ((w/2)+60) ((h/2)+60)],optionslsqnonlin);
    ffinals(i,1)=x(1);
    xfinals(i,1)=x(2);
    yfinals(i,1)=x(3);
    
    curscore=sum(abs(fval));
    
     [svScore, detScore, EssScore, EssScoreIA ]= EvalErrorParams1(TF,x(1),x(1),x(2),x(3),x(2),x(3) );
     curscore=detScore;   
    
   % disp(['iteration ' num2str(i) ' started from f= ' num2str(x0(1,1)) ' x= ' num2str(x0(1,2)) ' and y= ' num2str(x0(1,3))]);   
   % disp(['iteration ' num2str(i) ' best f is ' num2str(x(1)) ' and best x = ' num2str(x(2)) ' and best y is ' num2str(x(3)) ' and score was ' num2str(curscore) ' det score was ' num2str(detScore) ' SV score was ' num2str(svScore) ' and ess score was ' num2str(EssScore) ' IA score is ' num2str( EssScoreIA)]);
       
       scorearray(i,1)=curscore;
   if(curscore<bestscore && imag(x(1))==0 && x(1)>200 && x(1)<1600 )
        bestscore=curscore;
        %  disp(['iteration ' num2str(i)]);
        bestf=x(1);
        bestx=x(2);
        besty=x(3);
    %   x,resnorm,fval,exitflag
     %    disp(['**BEST: iteration ' num2str(i) ' best f is ' num2str(x(1)) ' and best x = ' num2str(x(2)) ' and best y is ' num2str(x(3)) ' and score was ' num2str(curscore) ' det score was ' num2str(detScore) ' SV score was ' num2str(svScore) ' and ess score was ' num2str(EssScore) ' IA score is ' num2str( EssScoreIA)]);
        
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

