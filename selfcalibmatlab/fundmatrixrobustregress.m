
function [F,iters] = fundmatrixrobustregress(x1, x2)


if nargin == 1
    corrs=x1;
    x1=corrs(1:2,:);
    x2=corrs(3:4,:);
end
[m1,n1]=size(x1);
[m2,n2]=size(x1);

if(m1~=m2 || n1~=n2)
    display('size of data is not right');
end

if(m1==2)
    x1=[x1 ; ones(1,n1)];
    x2=[x2 ; ones(1,n1)];
end

[x1, T1] = normalise2dpts(x1);
[x2, T2] = normalise2dpts(x2);

[m,npts]=size(x1);
% Build the constraint matrix
A = [x2(1,:)'.*x1(1,:)'   x2(1,:)'.*x1(2,:)'  x2(1,:)' ...
    x2(2,:)'.*x1(1,:)'   x2(2,:)'.*x1(2,:)'  x2(2,:)' ...
    x1(1,:)'             x1(2,:)'            ones(npts,1) ];

B=A(:,1:7); % it says forget that last column
y=-A(:,8);
b = robustfit(B,y);
%b = regress(-ones(npts,1),B);

c=b(2:8,:);
c(8,1)=1;
c(9,1)=b(1,1);

c=c/c(9,1);

F = reshape(c,3,3);

% Enforce constraint that fundamental matrix has rank 2 by performing
% a svd and then reconstructing with the two largest singular values.
[U,D,V] = svd(F,0);
F = U*diag([D(1,1) D(2,2) 0])*V';
F=F';
%Denormalise
F = T2'*F*T1;
F=F/F(3,3);
iters=0;


end

