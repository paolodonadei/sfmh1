function [F, iters] = fundmatrixrobustgeneral(corrs,typenum)

if(nargin ~= 2)
    display('wrong number of arguments');
    return
    
end


x1=corrs(1:2,:);
x2=corrs(3:4,:);


[m1,n1]=size(x1);
[m2,n2]=size(x1);

if(m1~=m2 || n1~=n2)
    display('size of data is not right');
end

if(m1==2)
    x1=[x1 ; ones(1,n1)];
    x2=[x2 ; ones(1,n1)];
end

npts=size(x1,2);

t = 1.96*1.96;  % Distance threshold for deciding outliers
s = 7;

fittingfn = @vgg_F_from_7pts_2img2;
distfn    = @sampsonF;
degenfn   = @isdegenerate;



% 1- RANSACV
% 2- MSAC
% 3- Residual based pvi
% 4- cook distance without leverage update
% 5- cook distance with update
% 6- Liang's method, not imeplemented yet
% 7- cook with update and cook based score , not imeplemented yet
% 8- cook wit update and cook based score and accumulation of pvis over
% trials , not imeplemented yet

scorefunc =  typenum;
randSampFunc =  typenum;
updatepviFunc = typenum;
initpvis = calcInitialPvis(typenum, x1, x2);


if(typenum~=0)
    % x1 and x2 are 'stacked' to create a 6xN array for ransac
    [Fz, inliers,iters] = ransac([x1; x2], fittingfn, distfn, degenfn, s, t,scorefunc,randSampFunc,initpvis,updatepviFunc);
    
else
    iters=0;
    inliers=1:1:npts;
end


F = fundmatrix(x1(:,inliers), x2(:,inliers));


end


function r = isdegenerate(x)
r = 0;


end



debug the liang method make sure it works right, if the histpogram is collecting points for the wrong intervamls then the whole thing is wrong
    also add in the pvi score calculation for everyone, and why didnt we use pvi calculation for everyone? in the old paper
    collect results
    
