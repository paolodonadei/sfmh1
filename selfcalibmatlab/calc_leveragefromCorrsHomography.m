function [L,A] = calc_leveragefromCorrsHomography(x1, x2)



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

A = zeros(3*npts,9);

O = [0 0 0];
for n = 1:npts
    X = x1n(:,n)';
    x = x2n(1,n); y = x2n(2,n); w = x2n(3,n);
    A(3*n-2,:) = [  O  -w*X  y*X];
    A(3*n-1,:) = [ w*X   O  -x*X];
    A(3*n  ,:) = [-y*X  x*X   O ];
end

LV = leverage(A);

for n = 1:npts
   L(n,1)= ( LV(3*n) + LV(3*n-1)   + LV(3*n-2)  )/3.0;
end


end
