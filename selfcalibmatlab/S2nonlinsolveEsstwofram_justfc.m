function [fcl, centerloc] = S2nonlinsolveEsstwofram_justfc(TF,w,h)
%this function , given a camera center and a focal length and a fundamental
%matrix computes the error with respect to a fundamental matrix
%tic

%TF=TF*10000;

fcl=[0 0];

xcen=0;
ycen =0;
centerloc=[xcen ycen];

[m,n]=size(TF);

sizeFs=n;

%f = @(x)computerEssentialErrorSquared(x,TF); %squared
%f = @(x)computerEssentialError(x,TF);
numtries=100;
ffinals=zeros(numtries,sizeFs);
xfinals=zeros(numtries,sizeFs);
yfinals=zeros(numtries,sizeFs);
scorearray=zeros(numtries,sizeFs);


for q=1:sizeFs

    x = PeterSturmSelf( TF{q},w,h);

    if(x(1,1)>200 && x(1,1)<1600)
        finit=x(1,1);
    else
        finit=w+h;
    end


    xinit=w/2;
    yinit=h/2;


    fvari=50;
    xvari=50;
    yvari=50;



    bestf=0;
    bestx=0;
    besty=0;
    bestscore=1000000000000;
    curscore=0;
    f = @(x)computerEssentialErrorSVD_justfc(x,TF{q},xinit,yinit);
    optionsfsolve  =optimset('Display','off','Jacobian','off','NonlEqnAlgorithm','lm','TolFun',1e-6,'TolX',1e-6);




    for i=1:numtries
        x0=[ (randn()*fvari)+finit  (randn()*xvari)+xinit  (randn()*yvari)+yinit ];


        [v,fval,exitflag,output]  = fsolve(f ,x0(1,1),optionsfsolve);

        x=[v xinit yinit];
        ffinals(i,q)=x(1);
        xfinals(i,q)=x(2);
        yfinals(i,q)=x(3);

        curscore=sum(abs(fval));

     

        % disp(['iteration ' num2str(i) ' started from f= ' num2str(x0(1,1)) ' x= ' num2str(x0(1,2)) ' and y= ' num2str(x0(1,3))]);
        %disp(['fund matrix: ' num2str(q) ' iteration ' num2str(i) ' best f is ' num2str(x(1)) ' and best x = ' num2str(x(2)) ' and best y is ' num2str(x(3)) ' and score was ' num2str(curscore) ' det score was ' num2str(detScore) ' SV score was ' num2str(svScore) ' and ess score was ' num2str(EssScore) ' IA score is ' num2str( EssScoreIA)]);

        scorearray(i,q)=curscore;
        if(curscore<bestscore && imag(x(1))==0 && x(1)>200 && x(1)<1600 )
            bestscore=curscore;
            %  disp(['iteration ' num2str(i)]);
            bestf=x(1);
            bestx=x(2);
            besty=x(3);
            %   x,resnorm,fval,exitflag
           % disp(['fund matrix: ' num2str(q) '**BEST: iteration ' num2str(i) ' best f is ' num2str(x(1)) ' and best x = ' num2str(x(2)) ' and best y is ' num2str(x(3)) ' and score was ' num2str(curscore) ' det score was ' num2str(detScore) ' SV score was ' num2str(svScore) ' and ess score was ' num2str(EssScore) ' IA score is ' num2str( EssScoreIA)]);

        end
    end

end

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
for q=1:sizeFs
    sizearray=scorearray;
    scoremedian(q)=median(scorearray(:,q));
    for i=1:numtries
        if(sizearray(i,q)>scoremedian(q))
            sizearray(i,q)=1;
        else
            sizearray(i,q)=ceil(10+(sizearray(i,q)*(-9/scoremedian(q))));
        end
    end
end


figure
hold
title(['focal lengths versus energy']);
xlabel('focal length');
ylabel('value of the energy function ');

for q=1:sizeFs
    scatter(ffinals(:,q), scorearray(:,q));
end
hold

%%%%%%%%%%%%%%%%%%%%%
styles={'r' ,'g', 'b', 'y', 'r' ,'c'};
figure
hold
title(['all the local minima points scatter plot']);
xlabel('x coordinates of optical centers');
ylabel('y coordinates of optical centers');
zlabel('value of focal length ');

for q=1:sizeFs

    scatter3(xfinals(:,q),yfinals(:,q),ffinals(:,q), 3 ,styles{mod(q,6)+1})
end
hold

%%%%%%%%%%%%%%%%%%%%%

figure
hold

title(['all the local minima points']);
xlabel('x coordinates of optical centers');
ylabel('y coordinates of optical centers');
zlabel('value of focal length ');

for q=1:sizeFs
    plot3(xfinals(:,q),yfinals(:,q),ffinals(:,q),styles{mod(q,6)+1},'MarkerSize',12)
end
hold

% figure
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
%     for j=230:350
%         count=count+1;
%         [ x, centerloc ] = PeterSturmSelfmoreparams( TF, i, j );
%         XF(count,1)=x(1,1);
%         XF(count,2)=i;
%         XF(count,3)=j;
%     end
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
% %
hist(reshape(ffinals,sizeFs*numtries,1),numtries/2);
title(['focal length']);
figure

hist(reshape(xfinals,sizeFs*numtries,1),numtries/2);
title(['xcomponent of camera center']);
figure

hist(reshape(yfinals,sizeFs*numtries,1),numtries/2);
title(['ycomponent of camera center']);
%
% display(['mean of f was ' num2str(mean(ffinals)) ' median of f was ' num2str(median(ffinals)) ' variance of f was ' num2str(var(ffinals))]);
% display(['mean of x was ' num2str(mean(xfinals)) ' median of x was ' num2str(median(xfinals)) ' variance of x was ' num2str(var(xfinals))]);
% display(['mean of y was ' num2str(mean(yfinals)) ' median of y was ' num2str(median(yfinals)) ' variance of y was ' num2str(var(yfinals))]);
% display(['best f is ' num2str(bestf) ' and best x is ' num2str(bestx) ' and best y is ' num2str(besty)]);
% display([' the highest cluster number was ' num2str(maxnumclusts) ' whose f is  ' num2str(maxnumclustsF) ' X is  ' num2str(maxnumclustsX) ' Y is  ' num2str( maxnumclustsY)  ]);
%
fedges=200:5:1600;
fhists = histc(ffinals,fedges) ;

[m,n]=size(fhists);
for i=1:m
    for j=1:n
        if(fhists(i,j)>sizeFs)
            fhists(i,j)=sizeFs;
        end
    end
end
[val,findex]=max(sum( fhists,2));

myf=0;
myx=0;
myy=0;

[m,n]=size(ffinals);
count=0;
for i=1:m
    for j=1:n
        if(ffinals(i,j)>fedges(findex) && ffinals(i,j)<fedges(findex+1))
            count=count+1;
            myf=myf+ffinals(i,j);
            myx=myx+xfinals(i,j);
            myy=myy+yfinals(i,j);
        end
    end
end

myf=myf/count;
myx=myx/count;
myy=myy/count;

disp(['best found f is ' num2str(myf) ' and best x is ' num2str(myx) ' and best y is ' num2str(myy)]);
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
% comments:
% 
% use the camera center to generate consensus cuz it seems more stable
% see how you can extend this to teh case where the two frames are different, thats the interesting case
% I still have not found a way to cluster the data, i was looking at pca
% some of the ways we can do this is this:
% 1) cluster the data of each algorithm and then only deal with the cluster
% centers, 
%2) just find the peak of the histogram for focal length and then find the
%optical center using the points that belong to the peak of the focal
%length
%3) and this is the best method, if i could find an explicit representation
%of this curve in space using properties of the essential mtrix as said in
%the fusiello paper this could really help out
end

