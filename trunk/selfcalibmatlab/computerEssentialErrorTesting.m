function [F] = computerEssentialErrorTesting(x,MYF)
%this function , given a camera center and a focal length and a fundamental
%matrix computes the error with respect to a fundamental matrix


f11=MYF(1,1);
f12=MYF(1,2);
f13=MYF(1,3);
f21=MYF(2,1);
f22=MYF(2,2);
f23=MYF(2,3);
f31=MYF(3,1);
f32=MYF(3,2);
f33=MYF(3,3);

fc=x(1,1);
ux=x(2,1);
vy=x(3,1);

K_norm1=zeros(3,3);


K_norm1(1,1)=  fc;
K_norm1(2,2)=  fc;
K_norm1(3,3)=  1;
K_norm1(1,3)= ux;
K_norm1(2,3)= vy;

G=(K_norm1')*MYF*K_norm1;
F=((2*(trace((G*(G')).^2)))-((trace(G*(G')))^2));


end

