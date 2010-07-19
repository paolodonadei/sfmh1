function [fcl, centerloc] = nonlinsolveEsstwoframSFMscore(TF,w,h,corrs)
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


bestfobj=0;
bestxobj=0;
bestyobj=0;

bestscorereconst=1000000000000;
curscorereconst=0;

bestscoreobj=1000000000000;
curscoreobj=0;

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
    
    
    % disp(['iteration ' num2str(i) ' started from f= ' num2str(x0(1,1)) ' x= ' num2str(x0(1,2)) ' and y= ' num2str(x0(1,3))]);
    % disp(['iteration ' num2str(i) ' best f is ' num2str(x(1)) ' and best x = ' num2str(x(2)) ' and best y is ' num2str(x(3)) ' and score was ' num2str(curscore) ' det score was ' num2str(detScore) ' SV score was ' num2str(svScore) ' and ess score was ' num2str(EssScore) ' IA score is ' num2str( EssScoreIA)]);
    
    
    %%%%%%%%%CHANGING SCORE%%%%%%%%%%%%%%%%%%
    Km=zeros(3,3);
    Km(1,1)=x(1);
    Km(2,2)=x(1);
    Km(1,3)=x(2);
    Km(2,3)=x(3);
    Km(3,3)=1;
    
    %     Km(1,1)=658.889793432494;
    %     Km(2,2)=658.889793432494;
    %     Km(1,3)=287.459559359891;
    %     Km(2,3)=286.19738780158;
    
    
    
    X=zeros(3,2);
    
    X(1,1)= corrs{1,1}(1,1);
    X(2,1)=corrs{1,1}(2,1) ;
    X(3,1)=1 ;
    
    X(1,2)=corrs{2,1}(1,1);
    X(2,2)=corrs{2,1}(2,1);
    X(3,2)=1 ;
    
    Em = getEssentialMatrix(TF,Km,Km) ;
    [PXcam] = getCameraMatrix(Em);
    [P2] = getCorrectCameraMatrix(PXcam, Km,Km, X);
    P1 = Km*[eye(3),zeros(3,1)];
    P2 = Km*P2;
    [er] = getReconstructionError(P1,P2,corrs);
    
    curscorereconst=er;
    
    
    scorearray(i,1)=curscorereconst;
    if(curscorereconst<bestscorereconst && imag(x(1))==0 && x(1)>200 && x(1)<1600 )
        bestscorereconst=curscorereconst;
        %  disp(['iteration ' num2str(i)]);
        bestf=x(1);
        bestx=x(2);
        besty=x(3);
        %   x,resnorm,fval,exitflag
        %    disp(['**BEST: iteration ' num2str(i) ' best f is ' num2str(x(1)) ' and best x = ' num2str(x(2)) ' and best y is ' num2str(x(3)) ' and score was ' num2str(curscore) ' det score was ' num2str(detScore) ' SV score was ' num2str(svScore) ' and ess score was ' num2str(EssScore) ' IA score is ' num2str( EssScoreIA)]);
        
    end
    
    curscoreobj=svScore;
    if(curscoreobj<bestscoreobj && imag(x(1))==0 && x(1)>200 && x(1)<1600 )
        bestscoreobj=curscoreobj;
        %  disp(['iteration ' num2str(i)]);
        bestfobj=x(1);
        bestxobj=x(2);
        bestyobj=x(3);
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

display(['best f according to reconstruction was ' num2str(bestf)]);
display(['best f according to objective function was ' num2str(bestfobj)]);




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

