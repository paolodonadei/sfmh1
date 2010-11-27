function [F] = computerEssentialErrorTesting(x,MYF)
%this function , given a camera center and a focal length and a fundamental
%matrix computes the error with respect to a fundamental matrix




fc=x(1,1);
ux=x(1,2);
vy=x(1,3);
as=x(1,4);


K_norm1=zeros(3,3);


K_norm1(1,1)=  fc;
K_norm1(2,2)= as* fc;
K_norm1(3,3)=  1;
K_norm1(1,3)= ux;
K_norm1(2,3)= vy;

G=(K_norm1')*MYF*K_norm1;
F=((2*(trace((G*(G'))*(G*(G')))))-((trace(G*(G')))^2));


end

