function [threshold]= findSVDthreshold(TF,w,h,x)

if (nargin == 3)
    x=[w w/2 h/2 w];
end

if(x(1,1)<eps)
    x=[w w/2 h/2 w];
end


[m,numFs]=size(TF);

for j=1:numFs
    obj(j,1)=computerEssentialErrorSVDNFramesWeighted(x,{TF{j}});
end

%hist(obj,100)

% curMedian=median(obj);
% MADN=1.96*(median(abs(obj-curMedian))/0.6745);
% oh god help me what do i do

MADN=3*prctile(obj,68.27);

threshold=min(MADN,0.0817060256336);


threshold=0.01217060256336;


end