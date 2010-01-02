function [fcl, centerloc] = nonlinsolveEsstwoframDiffFrames(TF,w,h)


%this function , given a camera center and a focal length and a fundamental
%matrix computes the error with respect to a fundamental matrix
%tic

%TF=TF*1000;

fcl=[0 0];

xcen1=0;
ycen1 =0;
xcen2=0;
ycen2 =0;
centerloc=[xcen1 ycen1 xcen2 ycen2];

[x, useles]=  PeterSturmSelf( TF,w,h);

if(x(1,1)>200 && x(1,1)<1600 && x(1,2)>200 && x(1,2)<1600)
    finit1=x(1,1);
     finit2=x(1,2);
else
    finit1=w+h; %w+h
    finit2=w+h; %w+h
end


xinit1=w/2;
yinit1=h/2;
xinit2=w/2;
yinit2=h/2;
numtries=100;

fvari=50;%this should change according to the size of the image
xvari=50;
yvari=50;

%f = @(x)computerEssentialErrorSquared(x,TF); %squared
f = @(x)computerEssentialErrorDiffFrames(x,TF);

ffinals1=zeros(numtries,1);
xfinals1=zeros(numtries,1);
yfinals1=zeros(numtries,1);

ffinals2=zeros(numtries,1);
xfinals2=zeros(numtries,1);
yfinals2=zeros(numtries,1);

scorearray=zeros(numtries,1);

bestf1=0;
bestx1=0;
besty1=0;
bestf2=0;
bestx2=0;
besty2=0;
bestscore=2000;
curscore=0;

optionsfsolve  =optimset('Display','off','Jacobian','on','Algorithm','levenberg-marquardt','TolFun',1e-6,'TolX',1e-6);

optionslsqnonlin  =optimset('Display','off','Jacobian','on');


for i=1:numtries
    x0=[ (randn()*fvari)+finit1 ; (randn()*xvari)+xinit1 ; (randn()*yvari)+yinit1 ;  (randn()*fvari)+finit2 ; (randn()*xvari)+xinit2 ; (randn()*yvari)+yinit2  ];
    
    
   % [x,fval,exitflag,output]  = fsolve(f ,x0,optionsfsolve);
     [x,resnorm,fval,exitflag] = lsqnonlin(f,x0,[finit1-150; ((w/2)-60); ((h/2)-60) ; finit2-150; ((w/2)-60); ((h/2)-60) ],[finit1+150; ((w/2)+60); ((h/2)+60) ; finit2+150; ((w/2)+60); ((h/2)+60) ],optionslsqnonlin);
    
     ffinals1(i,1)=x(1);
    xfinals1(i,1)=x(2);
    yfinals1(i,1)=x(3);
    ffinals2(i,1)=x(4);
    xfinals2(i,1)=x(5);
    yfinals2(i,1)=x(6);
    
    curscore=sum(abs(fval));
    
     [svScore, detScore, EssScore, EssScoreIA ]= EvalErrorParams1(TF,x(1),x(4),x(2),x(3),x(5),x(6) );
%     curscore=detScore;
    
       
   
       
       scorearray(i,1)=curscore;
    if(curscore<bestscore)
        bestscore=curscore;
        %  disp(['iteration ' num2str(i)]);
        bestf1=x(1);
        bestx1=x(2);
        besty1=x(3);
        bestf2=x(4);
        bestx2=x(5);
        besty2=x(6);
        disp(['iteration ' num2str(i) ' best f1 is ' num2str(bestf1) ' and best x1 is ' num2str(bestx1) ' and best y1 is ' num2str(besty1) ' best f2 is ' num2str(bestf2) ' and best x2 = ' num2str(bestx2) ' and best y2 is ' num2str(besty2) ' and score was ' num2str(curscore) ' det score was ' num2str(detScore) ' SV score was ' num2str(svScore) ' and ess score was ' num2str(EssScore) ' IA score is ' num2str( EssScoreIA)]);
        
        
    end
end



%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% sizearray=scorearray;
% scoremedian=median(scorearray);
% for i=1:numtries
%     if(sizearray(i,1)>scoremedian)
%         sizearray(i,1)=1;
%     else
%         sizearray(i,1)=ceil(10+(sizearray(i,1)*(-9/scoremedian)));
%     end
% end
% 
% 
% scatter(ffinals, scorearray);
% title(['focal lengths versus energy']);
% xlabel('focal length');
% ylabel('value of the energy function ');
% 
% figure;
% 
% scatter(ffinals, log(scorearray));
% title(['log scale focal lengths versus energy']);
% xlabel('focal length');
% ylabel('value of the energy function ');
% 
% figure;
% 
% 
% scatter3(xfinals,yfinals,ffinals, sizearray , 'r')
% title(['all the local minima points scatter plot']);
% xlabel('x coordinates of optical centers');
% ylabel('y coordinates of optical centers');
% zlabel('value of focal length ');
% figure
% 
% plot3(xfinals,yfinals,ffinals,'r.','MarkerSize',12)
% title(['all the local minima points']);
% xlabel('x coordinates of optical centers');
% ylabel('y coordinates of optical centers');
% zlabel('value of focal length ');
% figure
% 
% numclusts=10;
% X=[ ffinals  xfinals   yfinals];
% [idx,ctrs] = kmeans(X,numclusts,'Replicates',5);

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
% figure
% 
% 
% hist(ffinals,numtries/2);
% title(['focal length']);
% figure
% 
% hist(xfinals,numtries/2);
% title(['xcomponent of camera center']);
% figure
% 
% hist(yfinals,numtries/2);
% title(['ycomponent of camera center']);

% display(['mean of f was ' num2str(mean(ffinals)) ' median of f was ' num2str(median(ffinals)) ' variance of f was ' num2str(var(ffinals))]);
% display(['mean of x was ' num2str(mean(xfinals)) ' median of x was ' num2str(median(xfinals)) ' variance of x was ' num2str(var(xfinals))]);
% display(['mean of y was ' num2str(mean(yfinals)) ' median of y was ' num2str(median(yfinals)) ' variance of y was ' num2str(var(yfinals))]);
% display(['best f is ' num2str(bestf) ' and best x is ' num2str(bestx) ' and best y is ' num2str(besty)]);
% display([' the highest cluster number was ' num2str(maxnumclusts) ' whose f is  ' num2str(maxnumclustsF) ' X is  ' num2str(maxnumclustsX) ' Y is  ' num2str( maxnumclustsY)  ]);


%
%toc

fcl=[ bestf1 bestf2];

xcen1=bestx1;
ycen1=besty1;
xcen2=bestx2;
ycen2=besty2;

centerloc=[xcen1 ycen1 xcen2 ycen2];

end

