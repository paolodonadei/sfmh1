function [fcl, centerloc] = S2nonlinsolveEssNframGeometric(TF,w,h,corrs)
%this function , given a camera center and a focal length and a series of fundamental
%matrices computes the error with respect to a fundamental matrix
%tic
if (nargin ~=4 )
    disp(['error, the weighted method needs the correpsondences']);
end
%TF=TF*10000;
plotting=0;
numtries=10;

[m,numFs]=size(TF);
 Weights=ones(numFs,1);
 
 
 for i=1:numFs
     clear b;
     [b] = funddist(TF{1,i}, [corrs{1,i} ; corrs{2,i}]);
     d(i)=mean((b));
      Weights(i)=1/d(i);
     
 end
 Weights=Weights/sum(Weights);


%Weights

clear focs xcentrs ycentrs scrs bestFfinal bestXfinal bestYfinal;
[focs, xcentrs, ycentrs, ars,scrs, bestFfinal, bestXfinal, bestYfinal,bestARfinal] =  findBestsolsrepeatmore(numtries, TF, w,h,Weights);
 
scorearray=scrs;
ffinals=focs;
xfinals=xcentrs;
yfinals=ycentrs;

bestf=bestFfinal;
bestx= bestXfinal;
besty= bestYfinal;
bestar=bestARfinal;


fcl=[ bestf bestf*bestar];
centerloc=[bestx besty];



end

