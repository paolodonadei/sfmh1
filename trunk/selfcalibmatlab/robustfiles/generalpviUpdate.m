function   [outpvis, outinitpvi] = generalpviUpdate(updatetype,initialPvi,pvis,residuals,t,inliers,x,currentIter,totalIter)

if(updatetype==1 || updatetype==2)
    outpvis=pvis;
    outinitpvi=initialPvi ;
elseif(updatetype==3)
    [outpvis] =  cookUpdatepureres(updatetype,initialPvi,pvis,residuals,t,inliers,x,currentIter,totalIter);
       outinitpvi=initialPvi ;
elseif(updatetype==4)
    [outpvis] = cookUpdatefixed(initialPvi,pvis,residuals,t,inliers,x, currentIter,totalIter);
      outinitpvi=initialPvi ;
elseif(updatetype==5)
      [outpvis, outinitpvi] = cookUpdatelevup(initialPvi,pvis,residuals,t,inliers,x, currentIter,totalIter);
    
end

end

function [cdi] = findCookDistance(Leverages, residuals)


L=initialPvi;
[npts,m]=size(initialPvi);
pvis=zeros(npts,1);
h=zeros(npts,1);
r=residuals';
rmean=mean(r);
r=r./(9*rmean);
for i=1:npts
    if(abs(initialPvi(i,1)-1)<eps)
        h(i,1)=eps; % i dont know what to do about this
    else
        h(i,1)=(r(i,1)*initialPvi(i,1))/((1-initialPvi(i,1))*(1-initialPvi(i,1)));
    end
end
end

function [pi]=findProbabilitiesRobust(rawvals)

% finding probabilities from these values by assuming a zero mean gaussian
% as the distribution and then using the MADN as the STD and then finding
% probabilities
npts=size(rawvals,2);
myrstd=mad(rawvals,1); % calculatre teh median standard deviation before squaring
rawvals=rawvals.^2;
myvar=myrstd*myrstd;
mdenom=1/(sqrt(2*pi*myvar));
for i=1:npts
    pi(i,1)=(exp(-myvar(i,1)/(2*myvar)))*mdenom;
end
pvis = normalizeData(pvis,0);

end



function [outdata] = normalizeData(indata,cap)

if(cap==0)
    
    mind=min(indata);
    maxd=max(indata);
    ranged=maxd-mind;
    outdata=(indata-mind)*(1/ranged);
else
    mind=min(indata);
    indata=indata-mind;
    
    ranged=mad(indata,1)*3;
    outdata=(indata/ranged);
    [npts,m]=size( outdata);
    for i=1:npts
        if( outdata(i,1)>1)
            outdata(i,1)=1;
        end
        if( outdata(i,1)<0)
            outdata(i,1)=0;
        end
    end
    
    
    
end

end



function   [pviso,initialPvi] = cookUpdatelevup(initialPvi,pvis,residuals,t,inliers,x, currentIter,totalIter)


Fnew = fundmatrix(x(:,inliers));
[bestInliers, bestF, residualsnew, meaner,varer,meder,numins] = sampsonF(Fnew, x,1.96*1.96 );


Lnew = calc_leveragefromCorrs(x(:,bestInliers));

[m,nptsInlier]=size(bestInliers);



for i=1:nptsInlier
    initialPvi(bestInliers(1,i) ,1)=Lnew(i,1);
end


[cdi] = findCookDistance(initialPvi,  residualsnew);
[pviso]=findProbabilitiesRobust(cdi);

end

function   [pviso] = cookUpdatefixed(initialPvi,pvis,residuals,t,inliers,x, currentIter,totalIter)

Fnew = fundmatrix(x(:,inliers));
[bestInliers, bestF, residualsnew, meaner,varer,meder,numins] = sampsonF(Fnew, x,1.96*1.96 );

[cdi] = findCookDistance(initialPvi,  residualsnew);
[pviso]=findProbabilitiesRobust(cdi);

end

function   [pviso] = cookUpdatepureres(initialPvi,pvis,residuals,t,inliers,x, currentIter,totalIter)


if(currentIter< (0.1*totalIter))
    pviso=pvis;
    return
end
Fnew = fundmatrix(x(:,inliers));
[bestInliers, bestF, residualsnew, meaner,varer,meder,numins] = sampsonF(Fnew, x,1.96*1.96 );

[pviso]=findProbabilitiesRobust(residualsnew);

end