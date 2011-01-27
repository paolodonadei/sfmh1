function   [pvis,initialPvi] = cookUpdatelevup(initialPvi,pvis,residuals,t,inliers,x)
% 
% global inlierOutlier;

% here the initial pvis are the leverage
% display(['first leverage was ' num2str(mean(initialPvi))]);


% the freakin number of inliers doesnt increase when i refit
% also the leverages are increasing when i update FK THIS

Fnew = fundmatrix(x(:,inliers));
[bestInliers, bestF, residualsnew, meaner,varer,meder,numins] = sampsonF(Fnew, x,1.96*1.96 );
display([' old mean was ' num2str(mean(residuals)) ' and new is ' num2str(mean(residualsnew))]);

Lnew = calc_leveragefromCorrs(x(:,bestInliers));


[m,nptsInlier]=size(bestInliers);


for i=1:nptsInlier

    initialPvi(bestInliers(1,i) ,1)=Lnew(i,1);
end



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

% 
% 
% hist(h,50);
% title('cook dist');
% figure
% hist(r,50);
% title('resdual');
% figure
% hist(initialPvi,50);
% title('leverage');
% figure
% display(['second leverage was ' num2str(mean(initialPvi))]);

myrstd=mad(h,1); % calculatre teh median standard deviation before squaring
h=h.^2;
myvar=myrstd*myrstd;
mdenom=1/(sqrt(2*pi*myvar));
for i=1:npts
    pvis(i,1)=(exp(-h(i,1)/(2*myvar)))*mdenom;
end
pvis = normalizeData(pvis,0);

% [errorin,errorout] = pvifitness(inlierOutlier',pvis)
% close all

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