function [ F, iters ] = ransackovesi(x1,x2)

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
t = 1.96*1.96;  % Distance threshold for deciding outliers
s = 7; 

fittingfn = @vgg_F_from_7pts_2img2;
distfn    = @sampsonF;
degenfn   = @isdegenerate;
scorefunc = @ransacScore;

% x1 and x2 are 'stacked' to create a 6xN array for ransac
[F, inliers,iters] = ransac([x1; x2], fittingfn, distfn, degenfn, s, t,scorefunc);

% Now do a final least squares fit on the data points considered to
% be inliers.
F = fundmatrix(x1(:,inliers), x2(:,inliers));


end


function r = isdegenerate(x)
r = 0;


end