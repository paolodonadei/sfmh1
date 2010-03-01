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


xcen=0;
ycen =0;
centerloc=[xcen ycen];
[m,numFs]=size(TF);


fundscores=zeros(1,numFs);


numtries=1;


x= [0 0 0 0];
badcounter=1;
maxbad=10;
while(sum(x)<eps && badcounter<maxbad)
    if(badcounter==1)
        WEIGHTS=ones( numFs,1);
    else

        WEIGHTS=rand( numFs,1);
    end
    badcounter=badcounter+1;
    [focs, xcentrs, ycentrs, ars,scrs, bestFfinal, bestXfinal, bestYfinal,bestAR] =  findBestsolsrepeatmore(numtries, TF, w,h,WEIGHTS);
    x= [bestFfinal  bestXfinal  bestYfinal bestAR];
end

%%%%%%%%%%%%%%%%%%%%% now we go through the results and remove the F one by
%%%%%%%%%%%%%%%%%%%%% one seeing whcih ones ought to be deleted
if(bestFfinal>eps)
x0=[bestFfinal  bestXfinal  bestYfinal bestAR];
else
 x0=[w  w/2 h/2 1];
end

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

    clear focs xcentrs ycentrs scrs bestFfinal bestXfinal bestYfinal;

    [focs, xcentrs, ycentrs,cars, scrs, bestF, bestX, bestY, bestAR] = findBestsolsrepeatmore(1, TFdeletion, w,h,ones(numFs-1,1),x0(1,1),x0(1,2), x0(1,3),0,0,0,x0(1,4) );
    x=[bestF bestX bestY bestAR];

    curscore=mean(scrs);

    allscorediffs(j,1)=bestscore-curscore;
    if(  bestscore-curscore>0.0001) % this line is very vry important, i pulled this threshold out of air

        scorediffs(j,1)=bestscore-curscore;
        numBadframes=numBadframes+1;

    end


end

modify this so that if a solution is not obtained it will iterztively remove more and more fundamental matrices, you should have a fail safe option in all algorithms
in this case you might have to use allscorediff rather than score dioffs

numDeletion=min(numBadframes,maxnumdeletions);
[B,IX] = sort(scorediffs,1,'descend');


finalF=cell(1,numFs-numDeletion);


% % %
%  for i=1:(numDeletion)
%      disp([' removing frame ' num2str(IX(i))]);
% %
%  end

count=1;
for i=(numDeletion+1):numFs
    finalF{1,count}=TF{1,IX(i)};
    count=count+1;
end

%%%%%%%%%%%%%%%%%%%%%%
%final refitting




[m,numFsfinal]=size(finalF);
[focs, xcentrs, ycentrs, cars, scrs, bestFfinal, bestXfinal, bestYfinal, bestAR] = findBestsolsrepeatmore(10, finalF, w,h,ones(numFsfinal,1));



%
%
fcl=[ bestFfinal bestFfinal*bestAR];
centerloc=[bestXfinal bestYfinal];

end

