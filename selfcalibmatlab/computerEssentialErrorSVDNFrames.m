function [F] = computerEssentialErrorSVDNFrames(x,MYF)
%this function , given a camera center and a focal length and a fundamental
%matrix computes the error with respect to a fundamental matrix
[m,n]=size(MYF);

F=zeros(9,1);
for i=1:n
    
T = computerEssentialErrorSVD(x,MYF{1,i});

F=F+T;
end




end

