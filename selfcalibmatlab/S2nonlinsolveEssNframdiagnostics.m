function [fcl, centerloc] = S2nonlinsolveEssNframdiagnostics(TF,w,h)
%this function , given a camera center and a focal length and a series of fundamental
%matrices computes the error with respect to a fundamental matrix
%tic
if (nargin == 1)
    w=512;
    h=512;
end

threshold=findSVDthreshold(TF,w,h); % dont knwo about this

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
    x0=[bestFfinal  bestXfinal  bestYfinal bestAR*bestFfinal];
else
    x0=[w  w/2 h/2 w];
end



bestscore=computerEssentialErrorSVDNFramesWeighted(x0,TF);


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

    [focs, xcentrs, ycentrs,cars, scrs, bestF, bestX, bestY, bestAR] = findBestsolsrepeatmore(1, TFdeletion, w,h,ones(numFs-1,1),x0(1,1),x0(1,2), x0(1,3),0,0,0,x0(1,4)/x0(1,1) );

    if(bestF>eps)
        x=[bestF bestX bestY bestAR*bestF];
    else
        x=[w  w/2 h/2 w];
    end


    curscore=computerEssentialErrorSVDNFramesWeighted(x,TFdeletion);

    allscorediffs(j,1)=bestscore-curscore;



end

%modify this so that if a solution is not obtained it will iterztively remove more and more fundamental matrices, you should have a fail safe option in all algorithms
%in this case you might have to use allscorediff rather than score dioffs
% count=1;
% for i=(numDeletion+1):numFs
%     finalF{1,count}=TF{1,IX(i)};
%     count=count+1;
% end

errorSVD=100;
numFleft=numFs-1;
countt=1;

numPos=sum((allscorediffs>0));
while(errorSVD>threshold && numFleft>2 )

    [Y,I] = max(allscorediffs);
    allscorediffs(I,1)=-100000;
    numPos=sum((allscorediffs>0));

    numFtobedeleted(countt,1)=I;

    numFtobedeleted=sort(numFtobedeleted,'descend');
    %this not working
    finalF=TF;
    for i=1:size(numFtobedeleted)
        finalF(:,numFtobedeleted(i,1))=[];
        disp(['deleting ' num2str(numFtobedeleted(i,1))]);
    end
    disp('____________________');
    [m,numFleft]=size(finalF);


    [focs, xcentrs, ycentrs, cars, scrs, bestFfinal, bestXfinal, bestYfinal, bestAR] = findBestsolsrepeatmore(2, finalF, w,h);
    solutionz{countt,1}=[bestFfinal bestXfinal bestYfinal bestAR*bestFfinal];

    errorSVD=computerEssentialErrorSVDNFramesWeighted(solutionz{countt,1},finalF);
    scorearray(countt,1)=errorSVD;
    countt=countt+1;

end

if(countt>1)

    if(errorSVD>threshold)
        [Y,I] = min(scorearray);

    else
        I=countt-1;
    end
    bestFfinal=  solutionz{I,1}(1,1) ;
    bestFfinal2= solutionz{I,1}(1,4) ;
    bestXfinal=  solutionz{I,1}(1,2) ;
    bestYfinal=  solutionz{I,1}(1,3) ;
else
    bestFfinal=  x0(1,1) ;
    bestFfinal2= x0(1,4) ;
    bestXfinal=  x0(1,2) ;
    bestYfinal=  x0(1,3) ;
end





%
%
fcl=[ bestFfinal bestFfinal2];
centerloc=[bestXfinal bestYfinal];

end

