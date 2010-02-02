function [F] = computerEssentialErrorSVD(x,MYF)
%this function , given a camera center and a focal length and a fundamental
%matrix computes the error with respect to a fundamental matrix
fc=x(1,1);
ux=x(1,2);
vy=x(1,3);

K_norm1=zeros(3,3);
K_norm2=zeros(3,3);

K_norm1(1,1)=  fc;
K_norm1(2,2)=  fc;
K_norm1(3,3)=  1;
K_norm1(1,3)= ux;
K_norm1(2,3)= vy;


K_norm2(1,1)= fc;
K_norm2(2,2)= fc;
K_norm2(3,3)=  1;
K_norm2(3,1)= ux;
K_norm2(3,2)= vy;




G=(K_norm2)*MYF*K_norm1;

try 
S = svd(G);
catch 
S= [ 100 ;  10 ;  5];
end


F=abs((S(1,1)-S(2,1))/S(2,1));

end

