function [F, iters,pvisot] = fundmatrixrobustgeneral(corrs,typenum)

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
% 5- cook distance with update - this is best so far
% 6- Liang's method, not imeplemented yet
% 7- cook with update and cook based score using likelihoods
% 8- cook wit update and cook based score and accumulation of pvis over

scorefunc =  typenum;
randSampFunc =  typenum;
updatepviFunc = typenum;
initpvis = calcInitialPvis(typenum, x1, x2);
updateIterationFunc=typenum;
findInliersFunc=typenum;

pvis=initpvis;
if(typenum~=0)
    % x1 and x2 are 'stacked' to create a 6xN array for ransac
    [Fz, inliers,iters,pvis] = ransac([x1; x2], fittingfn, distfn, degenfn, s, t,scorefunc,randSampFunc,initpvis,updatepviFunc,updateIterationFunc,findInliersFunc);
    
else
    iters=0;
    inliers=1:1:npts;
end







if(typenum==5 || typenum>=9)

F = fundmatrix(x1, x2,pvis,0);
elseif(typenum==10)
    
    
    for k=1:npts
    if(pvis(k,1)<0.5)
        pvis(k,1)=0;
    end
    
    F = fundmatrix(x1, x2,pvis,0);
    end
else
    
    F = fundmatrix(x1(:,inliers), x2(:,inliers));


end

global corrsclean;
[bestInliers, bestF, residuals, meaner,varer,meder,numins] = sampsonF(F, corrsclean );

display([' linear ' num2str(meaner)]);

 
% 
pvisot=pvis;
end


function r = isdegenerate(x)
r = 0;


end
