function [threshold]= findSVDthreshold(TF,w,h)

x=[w w/2 h/2 w];

[m,numFs]=size(TF);

for j=1:numFs
obj(j,1)=computerEssentialErrorSVDNFramesWeighted(x,{TF{j}});    
end

%hist(obj,100)

curMedian=median(obj);
MADN=median(abs(obj-curMedian))/0.6745;



threshold=min(1.96*MADN,0.05);





end