function [ F, iters ] = napsactor(x1,x2)


[m1,n1]=size(x1);
[m2,n2]=size(x1);

if(m1~=m2 || n1~=n2)
    display('size of data is not right');
end

if(m1==2)
    x1=[x1 ; ones(1,n1)];
      x2=[x2 ; ones(1,n1)];
end

[f, f_sq_errors, n_inliers,inlier_index,F] = torr_estimateF( [x1 ; x2]',1,[1000 4], 'mapsac', 1);


end