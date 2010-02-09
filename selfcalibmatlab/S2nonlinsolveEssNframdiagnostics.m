function [fcl, centerloc] = S2nonlinsolveEssNframdiagnostics(TF,w,h)
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
[m,numFs]=size(TF);


fundscores=zeros(1,numFs);


numtries=10;


if(strcmp(version('-release'),'14')==1)
    optionsfsolve  =optimset('Display','off','Jacobian','off','NonlEqnAlgorithm','lm','TolFun',1e-6,'TolX',1e-6);
else
    optionsfsolve    =optimset('Display','off','Jacobian','off','Algorithm','levenberg-marquardt','TolFun',1e-6,'TolX',1e-6);
end




[focs, xcentrs, ycentrs, scrs, bestF, bestX, bestY] = findBestsolsrepeat(numtries, TF, w,h);

%%%%%%%%%%%%%%%%%%%%% now we go through the results and remove the F one by
%%%%%%%%%%%%%%%%%%%%% one seeing whcih ones ought to be deleted

x0=[bestF  bestX  bestY];
bestscore=min(scrs);
maxnumdeletions=ceil(numFs/5);
scorediffs=zeros(numFs,1);
numBadframes=0;
allscorediffs=zeros(numFs,1);
for j=1:numFs
    clear TFdeletion;

    count=1;
    for k=1:numFs
        if(k~=j)
            TFdeletion{1,count}=TF{1,k};
            count=count+1;
        end
    end


    clear f;
    f = @(x)computerEssentialErrorSVDNFrames(x,TFdeletion);
    [x,fval,exitflag,output]  = fsolve(f ,x0,optionsfsolve);
    %  disp(['when not using frame ' num2str(j) ' we get error ' num2str(sum(abs(fval)))  '  and best score was ' num2str(bestscore)]);
    %  x

    if(x(1)<0 || x(1)>maxfocal || x(2)<0 || x(2)>w || x(3)<0 || x(3)>h)
        fundscores(1,j)=10000000;
    end
    curscore=sum(abs(fval));

    allscorediffs(j,1)=bestscore-curscore;
    if(  bestscore-curscore>0.001) % this line is very vry important, i pulled this threshold out of air 

        scorediffs(j,1)=bestscore-curscore;
        numBadframes=numBadframes+1;

    end


end



numDeletion=min(numBadframes,maxnumdeletions);
[B,IX] = sort(scorediffs,1,'descend');


finalF=cell(1,numFs-numDeletion);


% 
% for i=1:(numDeletion)
%      disp([' removing frame ' num2str(IX(i))]);
% 
% end

count=1;
for i=(numDeletion+1):numFs
    finalF{1,count}=TF{1,IX(i)};
    count=count+1;
end

%%%%%%%%%%%%%%%%%%%%%%
%final refitting

x0=[bestF  bestX  bestY];


[m,numFsfinal]=size(finalF);
[focs, xcentrs, ycentrs, scrs, bestFfinal, bestXfinal, bestYfinal] = findBestsolsrepeat(10, finalF, w,h,ones(numFsfinal,1),bestF,bestX,bestY,10, 10,10);



%
%
fcl=[ bestFfinal bestFfinal];
centerloc=[bestXfinal bestYfinal];

end

