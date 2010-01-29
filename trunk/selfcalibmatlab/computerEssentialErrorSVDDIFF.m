function [F] = computerEssentialErrorSVDDIFF(x,MYF)
%this function , given a camera center and a focal length and a fundamental
%matrix computes the error with respect to a fundamental matrix
fc1=x(1,1);
ux1=x(1,2);
vy1=x(1,3);

fc2=x(1,4);
ux2=x(1,5);
vy2=x(1,6);

K_norm1=zeros(3,3);
K_norm2=zeros(3,3);

K_norm1(1,1)=  fc1;
K_norm1(2,2)=  fc1;
K_norm1(3,3)=  1;
K_norm1(1,3)= ux1;
K_norm1(2,3)= vy1;


K_norm2(1,1)= fc2;
K_norm2(2,2)= fc2;
K_norm2(3,3)=  1;
K_norm2(3,1)= ux2;
K_norm2(3,2)= vy2;




G=(K_norm2)*MYF*K_norm1;

try 
S = svd(G);
catch 
S= [ 100 ;  10 ;  5];
end


F=[abs((S(1,1)-S(2,1))/S(2,1))];

end

