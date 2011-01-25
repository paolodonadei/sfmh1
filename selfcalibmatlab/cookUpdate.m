function   [pvis] = cookUpdate(initialPvi,pvis,residuals);

global inlierOutlier;

% here the initial pvis are the leverage

L=initialPvi;
[npts,m]=size(initialPvi);
pvis=zeros(npts,1);
h=zeros(npts,1);
r=residuals';
rmean=mean(r);
r=r./(9*rmean);
for i=1:npts
    if(abs(L(i,1)-1)<eps)
        h(i,1)=100; % i dont know what to do about this
    else
        h(i,1)=(r(i,1)*L(i,1))/((1-L(i,1))*(1-L(i,1)));
    end
end

%
%
hist(h,50);
title('cook dist');
figure
hist(r,50); 
title('resdual');
figure
hist(L,50);
title('leverage');
figure
myrstd=mad(h,1); % calculatre teh median standard deviation before squaring
h=h.^2;
myvar=myrstd*myrstd;
mdenom=1/(sqrt(2*pi*myvar));
for i=1:npts
    pvis(i,1)=(exp(-h(i,1)/(2*myvar)))*mdenom;
end
pvis = normalizeData(pvis,0);
% minh=min(h);
% h=h-minh;
% 
% rangeh=mad(h,1)*3;
% hh=(h/rangeh);
% pvis=1-hh;
% 
% for i=1:npts
%     if(pvis(i,1)>1)
%         pvis(i,1)=1;
%     end
%     if(pvis(i,1)<0)
%         pvis(i,1)=0;
%     end
% end

[errorin,errorout] = pvifitness(inlierOutlier',pvis)
close all

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