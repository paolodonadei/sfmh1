function [r] = FMatrixNonLinError(p0, x,t)
F(1,1)=p0(1,1);
F(1,2)=p0(2,1);
F(1,3)=p0(3,1);
F(2,1)=p0(4,1);
F(2,2)=p0(5,1);
F(2,3)=p0(6,1);
F(3,1)=p0(7,1);
F(3,2)=p0(8,1);
F(3,3)=p0(9,1);

% 
% [U,D,V] = svd(F,0);
% F = U*diag([D(1,1) D(2,2) 0])*V';
% 
% 
% 
% F=F/F(3,3);

[bestInliers, bestF, residualsnew, meaner,varer,meder,numins] = sampsonF(F, x,t );

r=mean(sqrt(residualsnew'));
p0'
r(1,1)
end