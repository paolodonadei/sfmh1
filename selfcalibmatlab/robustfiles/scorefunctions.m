function [scores] = scorefunctions(typenum, size,consideredInliers, residuals,t,pvis,stdest,winsize)

if(typenum==1 )
    scores = ransacScore(typenum, size,consideredInliers, residuals,t);
elseif( typenum==2  )
    scores = msacScore(typenum, size,consideredInliers, residuals,t);
elseif( typenum==3 )
    miu = mixingparameterMLESAC(residuals',winsize,stdest);
    scores =  MLESACscore(residuals, miu,stdest,winsize);
elseif( typenum==4 )
    miu = mean(pvis);
    scores =  MLESACscore(residuals, miu,stdest,winsize);
else
    display('whaaaaaaaaaaaaaaaaat');
end



end



function   [score] = MLESACscore(residuals,mixparam,stdest,windowsize)


sized=size(residuals,2);
% we assume residuals are squared errors
score=0;

pin=mixparam;
po=1-pin;

score = 0;

oOutlier=findPOMLESAC(windowsize);

for i=1:sized
    
    
    
    pr=(pin*findPiMLESAC(residuals(1,i),stdest))+(po*oOutlier);
    
    if(pr>1 || pr<0)
        display(['caught a bad pvi with value ' num2str(pr)]);
    end
    score=score+log(max(eps,pr));
end
score=-score;


end
function   [miu] = mixingparameterMLESAC(residuals,winsize,stdest)

npts=size(residuals,1);
z=zeros(npts,1);
po=findPOMLESAC(winsize);
miu=0.5;
counter=0;
oldmiu=10;
while(counter<7 && (abs(oldmiu-miu)>0.02))
    curPo=(1-miu)*po;
    oldmiu=miu;
    for j=1:npts
        pin= miu*findPiMLESAC(residuals(j,1),stdest);
        
        z(j,1)=(pin)/((pin+ curPo));
    end
    miu=mean( z);
    counter=counter+1;
  %  display(['new miu is ' num2str(miu)]);
    
end
%display(['___________________']);

end
function [pin]= findPiMLESAC(e,stdest)

gaussDenom=1/((2*pi*stdest*stdest));
pin=gaussDenom*exp(-e/(2*stdest*stdest));
end

function [po]= findPOMLESAC(winsize)

po=1/winsize;

end


function [score] = msacScore(typenum, size,consideredInliers, residuals,t)
score=0;

for i=1:size
    if(abs(residuals(i))<t)
        score=score+abs(residuals(i));
    else
        score=score+t;
        
    end
    
end

end


function [score] = ransacScore(typenum, size,consideredInliers, residuals,t)
score=0;

for i=1:size
    if(abs(residuals(i))<t)
        score=score+1;
        
    end
    
end
score=-score;
end