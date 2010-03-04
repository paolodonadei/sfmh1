function [threshold]= findSVDthreshold(TF,w,h,x)

if (nargin == 3)
    x=[w w/2 h/2 w];
end

[m,numFs]=size(TF);

for j=1:numFs
    obj(j,1)=computerEssentialErrorSVDNFramesWeighted(x,{TF{j}});
end

%hist(obj,100)

% curMedian=median(obj);
% MADN=1.96*(median(abs(obj-curMedian))/0.6745);

MADN=1.96*prctile(obj,68.27);

threshold=min(MADN,0.05);





end