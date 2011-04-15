function [F, iters,pvisot] = fundmatrixrobustgeneral(corrs,typenum,winsize)

debugf=1;

if(nargin <2)
    display('wrong number of arguments');
    return
    
end

if(nargin ==2)
    winsize=500;
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


samplingHash=struct('UNIFORMSAMPLING',1,'MONTECARLO',2);
updateHash=struct('NOCHANGE',1,'COOKUPDATE',2,'LIANG',3,'ACCUMULATION',4,'EXPERIMENTAL1',5,'EXPERIMENTAL2',6);
scoreHash=struct('RANSACSCORE',1,'MSACSCORE',2,'MLESAC',3,'MLESACMIUFROMPVI',4);
initialPVIHash=struct('ONES',1,'POINTZEROFIVE',2,'FROMLEVERAGE',3,'INITIALIZELEVERAGE',4);
iterationsHash=struct('NORMAL',1,'PVICONVERGE',2,'INLIERPVICONVERGE',3);
finalFitHash=struct('NORMAL',1,'WEIGHTED',2,'WEIGHTEDTRIMMED',3);
% 1- RANSACV
% 2- MSAC
% 3- Residual based pvi
% 4- cook distance without leverage update
% 5- cook distance with update - this is best so far
% 6- Liang's method, not imeplemented yet
% 7- cook with update and cook based score using likelihoods
% 8- cook wit update and cook based score and accumulation of pvis over

algorithms(1)= struct('f1', 1, 'f2', 3)
scorefunc =  typenum;
randSampFunc =  typenum;
updatepviFunc = typenum;
initpvis = calcInitialPvis(typenum, x1, x2);
updateIterationFunc=typenum;
findInliersFunc=typenum;
finalfitOption=typenum;
stdest=1;

pvis=initpvis;
if(typenum~=0)
    % x1 and x2 are 'stacked' to create a 6xN array for ransac
    [Fz, inliers,iters,pvis] = ransac([x1; x2], fittingfn, distfn, degenfn, s, t,scorefunc,randSampFunc,initpvis,updatepviFunc,updateIterationFunc,findInliersFunc,stdest,winsize);
    
else
    iters=0;
    inliers=1:1:npts;
end





% pay attention to these guys

if(finalfitOption==1 )
    
    F = fundmatrix(x1, x2,pvis,0);
elseif(finalfitOption==2 )
    F = fundmatrix(x1(:,inliers), x2(:,inliers));
elseif(finalfitOption==3 )
    
    for i=1:npts
        if(pvis(i,1)<0.5)
            pvis(i,1)=0;
        end
    end
      F = fundmatrix(x1, x2,pvis,0);
else
    display(' this is wrong');
end


if(debugf==1)
    global corrsclean;
    [bestInliers, bestF, residuals, meaner,varer,meder,numins] = sampsonF(F, corrsclean );
    
    display([' linear ' num2str(meaner)]);
    
end


pvisot=pvis;
end


function r = isdegenerate(x)
r = 0;


end
