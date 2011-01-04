function [ F, iters ] = fundmatrixunrobust(x1,x2)


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
 iters =0;
F=fundmatrix(x1, x2);


end