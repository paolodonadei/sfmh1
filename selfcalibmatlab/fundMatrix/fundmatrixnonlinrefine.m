function [F] = fundmatrixnonlinrefine(xx,initF,normalize)

x1=xx(1:3,:);
x2=xx(4:6,:);

npts=size(xx,2);

t=1.96;


iterMax= 1000;
options=[1E-09, 1E-25, 1E-25, 1E-20, 1E-26];



p0(1,1)=initF(1,1);
p0(2,1)=initF(1,2);
p0(3,1)=initF(1,3);
p0(4,1)=initF(2,1);
p0(5,1)=initF(2,2);
p0(6,1)=initF(2,3);
p0(7,1)=initF(3,1);
p0(8,1)=initF(3,2);
p0(9,1)=initF(3,3);

%[ret, popt, info, covar]=levmar('FMatrixNonLinError', p0, xx, iterMax, options,'unc',x,t);

a = 4; b = 2.1; c = 4;
f = @(x)FMatrixNonLinError(x,xx,t);

[popt,fval,exitflag,output,grad] = fminunc(f ,p0);


F(1,1)=popt(1,1);
F(1,2)=popt(2,1);
F(1,3)=popt(3,1);
F(2,1)=popt(4,1);
F(2,2)=popt(5,1);
F(2,3)=popt(6,1);
F(3,1)=popt(7,1);
F(3,2)=popt(8,1);
F(3,3)=popt(9,1);



% Enforce constraint that fundamental matrix has rank 2 by performing
% a svd and then reconstructing with the two largest singular values.
[U,D,V] = svd(F,0);
F = U*diag([D(1,1) D(2,2) 0])*V';



F=F/F(3,3);

end

