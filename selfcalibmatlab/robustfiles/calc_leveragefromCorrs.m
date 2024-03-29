function [L] = calc_leveragefromCorrs(x1, x2)



if nargin == 1
    corrs=x1;
    [m3,n3]=size(corrs);
    if(m3==4)
        x1=corrs(1:2,:);
        x2=corrs(3:4,:);
    else
        x1=corrs(1:3,:);
        x2=corrs(4:6,:);
    end
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

L = leverage(A);


end
