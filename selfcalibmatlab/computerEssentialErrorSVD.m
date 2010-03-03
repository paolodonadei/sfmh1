function [F] = computerEssentialErrorSVD(x,MYF)
%this function , given a camera center and a focal length and a fundamental
%matrix computes the error with respect to a fundamental matrix

%shape of x is
% x(1,1) focal length
% x(1,2) optical center x coordinate
% x(1,3) optical center y coordinate
% x(1,4) aspect ratio,  OPTINAL
% x(1,5) skew,  OPTINAL


fc1=x(1,1);
ux=x(1,2);
vy=x(1,3);

[m,n]=size(x);


fc2=fc1;
skew=0;
%if we have an aspect ratio
if(n>3)
    fc2=x(1,4);
end

if(n>4)
    skew=x(1,5);
end

K_norm1=zeros(3,3);
K_norm2=zeros(3,3);

K_norm1(1,1)=  fc1;
K_norm1(2,2)=  fc2;
K_norm1(3,3)=  1;
K_norm1(1,2)= skew;
K_norm1(1,3)= ux;
K_norm1(2,3)= vy;


K_norm2(1,1)= fc1;
K_norm2(2,2)= fc2;
K_norm2(3,3)=  1;
K_norm2(3,1)= ux;
K_norm2(2,1)= skew;
K_norm2(3,2)= vy;




G=(K_norm2)*MYF*K_norm1;

try
    S = svd(G);
catch
    S= [ 100 ;  10 ;  5];
end

if( S(2,1)>eps)
    F=(((S(1,1)-S(2,1))/S(2,1)))^2;
    
else
    
    F=100;
end

% 
% threshold=0.05;
% if(F <threshold)
%     WEIGHT=(1-((F/threshold)^2))^2;
% elseif(F >=threshold )
%     WEIGHT=0 ;
% else
%     disp(['what happened']);
% end
% 
% F=F*WEIGHT;

end

