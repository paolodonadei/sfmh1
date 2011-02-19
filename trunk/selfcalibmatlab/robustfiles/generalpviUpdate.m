function   [outpvis, outinitpvi] = generalpviUpdate(updatetype,initialPvi,pvis,residuals,t,inliers,x,currentIter,totalIter)

if(updatetype==1 || updatetype==2)
    outpvis=pvis;
    outinitpvi=initialPvi ;
elseif(updatetype==3)
    [outpvis] =  cookUpdatepureres(initialPvi,pvis,residuals,t,inliers,x,currentIter,totalIter);
    outinitpvi=initialPvi ;
elseif(updatetype==4)
    [outpvis] = cookUpdatefixed(initialPvi,pvis,residuals,t,inliers,x, currentIter,totalIter);
    outinitpvi=initialPvi ;
elseif(updatetype==5 || updatetype==7 )
    [outpvis, outinitpvi] = cookUpdatelevup(initialPvi,pvis,residuals,t,inliers,x, currentIter,totalIter);
elseif(updatetype==6)
    [outpvis] = cookUpdatepureresLIANG(initialPvi,pvis,residuals,t,inliers,x, currentIter,totalIter);
    outinitpvi=initialPvi ;
elseif(updatetype==8 )
    [outpvis, outinitpvi] = cookUpdatelevupaccumulate(initialPvi,pvis,residuals,t,inliers,x, currentIter,totalIter);
end
% 
global inlierOutlier;
[errorin,errorout] = pvifitness(inlierOutlier',outpvis);
close all
end

function [cdi] = findCookDistance(Leverages, residuals)

p=8; % rank of matrix A

[npts,m]=size(Leverages);

cdi=zeros(npts,1);
 [r]=studentizeResiduals( residuals,Leverages);
for i=1:npts
    if(abs(Leverages(i,1)-1)<eps)
        cdi(i,1)=5; % i dont know what to do about this
    else
        cdi(i,1)=(r(i,1)*r(i,1)*Leverages(i,1))/(p*(1-Leverages(i,1)));
    end
    if(cdi(i,1)>5) % i did this to see a proper histogram, this makes no diff because p(7) is zero anyays
        cdi(i,1)=5;
    end
end
% hist(Leverages,100);
% title('histogram of leverage');
% figure
% hist(r,100);
% title('histogram of residuals');
% figure
% hist(cdi,100);
% title('histogram of cook distance');
% figure
end

function [pvis]=findProbabilitiesRobust(rawvals,stdv,meanv)

if(nargin  >1)
     myrstd=stdv;
else
    myrstd= 1.4826*mad(rawvals,1); % calculatre teh median standard deviation before squaring
end


if(nargin  >2)
    rawvals=rawvals-meanv;
else
  rawvals=rawvals;
end

%normalize distribution , now we have a normalized distribution
% then made the variance 1/2 by multiplying the distribution by sqrt(1/2)
rawvals=rawvals./myrstd;
rawvals=rawvals.*sqrt(1/2);



% finding probabilities from these values by assuming a zero mean gaussian
% as the distribution and then using the MADN as the STD and then finding
% probabilities
%rawvals =  normalizeData(rawvals,0);
npts=size(rawvals,1);




pvis=zeros(npts,1);
for i=1:npts
    pvis(i,1)=erfc(rawvals(i,1));
    if(pvis(i,1)>1 || pvis(i,1)<0)
    display(['found a pvi with value ' num2str(pvis(i,1))]);
    end
end


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
function   [pviso,initialPvi] = cookUpdatelevupaccumulate(initialPvi,pvis,residuals,t,inliers,x, currentIter,totalIter)

[pvison,initialPvin] = cookUpdatelevup(initialPvi,pvis,residuals,t,inliers,x, currentIter,totalIter);

pviso=(pvison+pvis)./2;
initialPvin=(initialPvi+initialPvin)./2;
end

function   [pviso,initialPvi] = cookUpdatelevup(initialPvi,pvis,residuals,t,inliers,x, currentIter,totalIter)

% if(currentIter< (0.1*totalIter))
%     pviso=pvis;
%     return
% end
%display(['started updating pvi from iteration ' num2str(currentIter)']);

Fnew = fundmatrix(x(:,inliers));
[bestInliers, bestF, residualsnew, meaner,varer,meder,numins] = sampsonF(Fnew, x,1.96*1.96 );


Lnew = calc_leveragefromCorrs(x(:,bestInliers));

[m,nptsInlier]=size(bestInliers);



for i=1:nptsInlier
    initialPvi(bestInliers(1,i) ,1)=Lnew(i,1);
end


[cdi] = findCookDistance(initialPvi,  residualsnew);
[pviso]=findProbabilitiesRobust(cdi,1/3); % here we assume a std for cook's distance



% hist(cdi,100); title('histogram ofcook distance');
% figure
% hist(pviso,100); title('pvis hisrogram');
% close all
end

function   [pviso] = cookUpdatefixed(initialPvi,pvis,residuals,t,inliers,x, currentIter,totalIter)
% if(currentIter< (0.1*totalIter))
%     pviso=pvis;
%     return
% end
Fnew = fundmatrix(x(:,inliers));
[bestInliers, bestF, residualsnew, meaner,varer,meder,numins] = sampsonF(Fnew, x,1.96*1.96 );

[cdi] = findCookDistance(initialPvi,  residualsnew);
[pviso]=findProbabilitiesRobust(cdi,1/3);

end

function   [pviso] = cookUpdatepureres(initialPvi,pvis,residuals,t,inliers,x, currentIter,totalIter)

if(currentIter< (0.1*totalIter))
    pviso=pvis;
    return
end
Fnew = fundmatrix(x(:,inliers));
[bestInliers, bestF, residualsnew, meaner,varer,meder,numins] = sampsonF(Fnew, x,1.96*1.96 );

[pviso]=findProbabilitiesRobust(residualsnew');

end
function [r]=studentizeResiduals(e,l)
p=8;
e=sqrt(e);
n=size(e,2);

MSRES=sum(e)/(n-p);

r=zeros(n,1);
for i=1:n
    if(abs(l(i,1)-1)<eps)
        r(i,1)=r(i,1); % cant divide by zero so just dont studentize
    else
   r(i,1)=e(1,i)/sqrt((MSRES*(1-l(i,1))));
    end
end


end
function   [pviso] = cookUpdatepureresLIANG(initialPvi,pvis,residuals,t,inliers,x, currentIter,totalIter)

[npts,m]=size(initialPvi);
if(currentIter< (0.1*totalIter))
    pviso=pvis;
    return
end
Fnew = fundmatrix(x(:,inliers));
[bestInliers, bestF, residualsnew, meaner,varer,meder,numins] = sampsonF(Fnew, x,1.96*1.96 );

resunsquared= sqrt(residualsnew);

stdr= std(resunsquared);
varr=var(resunsquared);

Q=0:0.3:(100*0.3);
Q(size(Q,2))=inf;

n = histc(resunsquared,Q);
n=n./npts;

dpdfs=zeros(size(Q,2),1);

prevpdf=0;

gausDenmo=1/(sqrt(pi*2*stdr*stdr));
for i=(size(Q,2)-1):-1:1
    
    val=0;
    if(i==(size(Q,2)-1))
        val=(Q(i)+Q(i)+0.3)/2;
    else
        val=(Q(i)+Q(i+1))/2;
    end 
        
        dpdfs(i,1)=prevpdf+((n(i)*gausDenmo*exp(-(val*val)/(2*stdr*stdr))));
        prevpdf=dpdfs(i,1);
    end
    
    
    % normalizing
    for i=size(Q,2):-1:1
        dpdfs(i,1)=dpdfs(i,1)/dpdfs(1,1);
    end
    
    
    pviso=zeros(npts,1);
    
    for i=1:npts
        asign=0;
        for j=2:size(Q,2)
            if((resunsquared(1,i))> Q(j-1) && (resunsquared(1,i))<= Q(j))
                pviso(i,1)=dpdfs(j,1);
                asign=1;
                break;
            end
        end
        if(asign<eps)
            pviso(i,1)=dpdfs(size(Q,2),1);
        end
    end
    
end

