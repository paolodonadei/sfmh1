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


numtries=10;




 [focs, xcentrs, ycentrs, ars,scrs, bestFfinal, bestXfinal, bestYfinal,bestAR] =  findBestsolsrepeatmore(numtries, TF, w,h);


%%%%%%%%%%%%%%%%%%%%% now we go through the results and remove the F one by
%%%%%%%%%%%%%%%%%%%%% one seeing whcih ones ought to be deleted

x0=[bestFfinal  bestXfinal  bestYfinal bestAR];
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



numDeletion=min(numBadframes,maxnumdeletions);
[B,IX] = sort(scorediffs,1,'descend');


finalF=cell(1,numFs-numDeletion);


% % 
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
[focs, xcentrs, ycentrs, cars, scrs, bestFfinal, bestXfinal, bestYfinal, bestAR] = findBestsolsrepeatmore(10, finalF, w,h,ones(numFsfinal,1),x0(1,1),x0(1,2), x0(1,3),10, 10,10,x0(1,4));



%
%
fcl=[ bestFfinal bestFfinal*bestAR];
centerloc=[bestXfinal bestYfinal];

end

