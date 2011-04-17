function [F, iters,pvisot] = fundmatrixrobustgeneral(corrs,typenum,winsize)

debugf=0;

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

algNameHash=struct('RANSAC',1,'MSAC',2,'MLESAC',3,'LIANG',4,'COOKUPDATE1',5,'COOKUPDATE2',6,'COOKUPDATE3',7,'COOKUPDATE4',8,'COOKUPDATE5',9);
samplingHash=struct('UNIFORMSAMPLING',1,'MONTECARLO',2);
updateHash=struct('NOCHANGE',1,'COOKUPDATE',2,'LIANG',3,'ACCUMULATION',4,'EXPERIMENTAL1',5,'EXPERIMENTAL2',6);
scoreHash=struct('RANSACSCORE',1,'MSACSCORE',2,'MLESAC',3,'MLESACMIUFROMPVI',4,'LEVERAGESCORE',5);
initialPVIHash=struct('ONES',1,'POINTZEROFIVE',2,'FROMLEVERAGE',3,'INITIALIZELEVERAGE',4);
iterationsHash=struct('NORMAL',2,'PVICONVERGE',1,'MEDIANCONVERG',3,'INLIERPVICONVERGE',4);
finalFitHash=struct('NORMAL',2,'WEIGHTED',1,'WEIGHTEDTRIMMED',3);
% 1- RANSACV
% 2- MSAC
% 3- Residual based pvi
% 4- cook distance without leverage update
% 5- cook distance with update - this is best so far
% 6- Liang's method, not imeplemented yet
% 7- cook with update and cook based score using likelihoods
% 8- cook wit update and cook based score and accumulation of pvis over


% RANSAC
algorithms(1).SAMPLINGMETHOD= samplingHash.UNIFORMSAMPLING;
algorithms(1).UPDATEPVIMETHOD= updateHash.NOCHANGE;
algorithms(1).SCOREMETHOD=scoreHash.RANSACSCORE;
algorithms(1).INITIALPVIMETHOD=initialPVIHash.ONES;
algorithms(1).ITERATIONNUMBERMETHOD=iterationsHash.NORMAL;
algorithms(1).FINALFITTINGMETHOD=finalFitHash.NORMAL ;


% MSAC
algorithms(2)=algorithms(1);
algorithms(2).SCOREMETHOD=scoreHash.MSACSCORE;


%MLESAC
algorithms(3)=algorithms(1);
algorithms(3).SCOREMETHOD=scoreHash.MLESAC;

% cookupdate

algorithms(5).SAMPLINGMETHOD= samplingHash.MONTECARLO;
algorithms(5).UPDATEPVIMETHOD= updateHash.COOKUPDATE;
algorithms(5).SCOREMETHOD=scoreHash.MSACSCORE;
algorithms(5).INITIALPVIMETHOD=initialPVIHash.POINTZEROFIVE;
algorithms(5).ITERATIONNUMBERMETHOD=iterationsHash.PVICONVERGE;
algorithms(5).FINALFITTINGMETHOD=finalFitHash.WEIGHTED;


% cookupate 2 score variation
algorithms(6)=algorithms(5);
algorithms(6).SCOREMETHOD=scoreHash.MLESAC;


% cookupate 3 score variation
algorithms(7)=algorithms(5);
algorithms(7).SCOREMETHOD=scoreHash.MLESACMIUFROMPVI;

% cookupate 4 score variation
algorithms(8)=algorithms(5);
algorithms(8).SCOREMETHOD=scoreHash.RANSACSCORE;

% cookupate 5 score variation
algorithms(9)=algorithms(5);
algorithms(9).SCOREMETHOD=scoreHash.LEVERAGESCORE;

initpvis = calcInitialPvis(algorithms(typenum).INITIALPVIMETHOD, x1, x2);

stdest=1;

pvis=initpvis;

pvis=initpvis;
if(typenum~=0)
    % x1 and x2 are 'stacked' to create a 6xN array for ransac
    [Fz, inliers,iters,pvis] = ransac([x1; x2], distfn, degenfn, s, t,algorithms(typenum).SCOREMETHOD,algorithms(typenum).SAMPLINGMETHOD,initpvis,algorithms(typenum).UPDATEPVIMETHOD,algorithms(typenum).ITERATIONNUMBERMETHOD,stdest,winsize);
    
else
    iters=0;
    inliers=1:1:npts;
end





% pay attention to these guys

if(algorithms(typenum).FINALFITTINGMETHOD==1 )
    
    F = fundmatrix(x1, x2,pvis,0);
elseif(algorithms(typenum).FINALFITTINGMETHOD==2 )
    F = fundmatrix(x1(:,inliers), x2(:,inliers));
elseif(algorithms(typenum).FINALFITTINGMETHOD==3 )
    
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
    
    display([' final error : ' num2str(meaner)]);
    display([' iterations :  ' num2str(iters)]);
end


pvisot=pvis;
end


function r = isdegenerate(x)
r = 0;


end
