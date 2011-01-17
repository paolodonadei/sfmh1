function [pvi] = calc_initial_pvi_cook(x1, x2)

global inlierOutlier;

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

[x1n, T1] = normalise2dpts(x1);
[x2n, T2] = normalise2dpts(x2);

[m,npts]=size(x1);
% Build the constraint matrix
A = [x2n(1,:)'.*x1n(1,:)'   x2n(1,:)'.*x1n(2,:)'  x2n(1,:)' ...
    x2n(2,:)'.*x1n(1,:)'   x2n(2,:)'.*x1n(2,:)'  x2n(2,:)' ...
    x1n(1,:)'             x1n(2,:)'            ];



[b,bint,r] = regress(-ones(npts,1),A);
L = leverage(A);

h=zeros(npts,1);
r=r.^2;
for i=1:npts
    h(i,1)=(r(i,1)*L(i,1))/((1-L(i,1))*(1-L(i,1)));
end

%     
% 
% 
% hh=1-h;
% h=hh;
% minh=min(h);
% maxh=max(h);
% rangeh=maxh-minh;
% pvi=(h-minh)*(1/rangeh);
pvi=h;

end