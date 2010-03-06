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


%%%%%%%%%%%%%%%%%%%%% now we go through the results and remove the F one by
%%%%%%%%%%%%%%%%%%%%% one seeing whcih ones ought to be deleted

x0=[w  w/2 h/2 w];




bestscore=computerEssentialErrorSVDNFramesWeighted(x0,TF);

    x=[w  w/2 h/2 w];
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
numFleft=numFs;


numPos=sum((allscorediffs>0));

[B,IX] = sort(allscorediffs,'descend');

for i=0:(numFs-1)
    finalF=TF;

    IXsorted=IX(1:i,:);
    IXsorted=sort(IXsorted,'descend');
    for k=1:i
        finalF(:,IXsorted(k,1))=[];
   %     disp(['deleting ' num2str(IXsorted(k,1))]);
    end
  %  disp('____________________');

    [focs, xcentrs, ycentrs, cars, scrs, bestFfinal, bestXfinal, bestYfinal, bestAR] = findBestsolsrepeatmore(3, finalF, w,h);
    solutionz{i+1,1}=[bestFfinal bestXfinal bestYfinal bestAR*bestFfinal];

    errorSVD=computerEssentialErrorSVDNFramesWeighted(solutionz{i+1,1},finalF);
    scorearray(i+1,1)=errorSVD;

    if(errorSVD<threshold)
        break;
    end


end




if(errorSVD>threshold)
    [Y,I] = min(scorearray);

else
    I=i+1;
end

bestFfinal=  solutionz{I,1}(1,1) ;
bestFfinal2= solutionz{I,1}(1,4) ;
bestXfinal=  solutionz{I,1}(1,2) ;
bestYfinal=  solutionz{I,1}(1,3) ;







%
%
fcl=[ bestFfinal bestFfinal2];
centerloc=[bestXfinal bestYfinal];

end

