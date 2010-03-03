function [F] = computerEssentialErrorSVDNFramesWeighted(x,MYF,W)
%this function , given a camera center and a focal length and a fundamental
%matrix computes the error with respect to a fundamental matrix

[m,n]=size(MYF);


if (nargin == 2)
   W=ones(n,1);
end

[l,k]=size(W);

if(k~=m || n~=l)
    disp(['the weights are not the right size, in computerEssentialErrorSVDNFramesWeighted']);
    MYF
    W
end


F=0;
for i=1:n

    T = computerEssentialErrorSVD(x,MYF{1,i});

    F=F+(T*W(i,1));
end

sumW=sum(W);

if( sumW>eps)
    F=F/sumW; %make sure the sum of your weights is constant
else
    F=100;
    disp('sum of W is zero inside computerEssentialErrorSVDNFramesWeighted, this shouldnt happen');
end


end

