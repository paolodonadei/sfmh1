function [scores] = scorefunctions(typenum, size,consideredInliers, residuals,t,pvis)

if(typenum==1 || typenum==2 )
    scores = ransacScore(typenum, size,consideredInliers, residuals,t);
elseif( typenum==6 || typenum==7)
    scores = likelihood(typenum, size,consideredInliers, residuals,t,pvis);
elseif(typenum==Inf )
    scores = likelihoodfixed(typenum, size,consideredInliers, residuals,t,pvis);
elseif( typenum==3 || typenum==4 || typenum==8 || typenum==5 || typenum==9 || typenum==10 || typenum==11 || typenum==12  )
    scores = msacScore(typenum, size,consideredInliers, residuals,t);
else
    display('whaaaaaaaaaaaaaaaaat');
end



end



function   [pviso,initialPvio] = MLESACscore(residuals,mixparam,stdest,windowsize)



% we assume residuals are squared errors
score=0;
stdr=stdest;
winddowsize=windowsize;
gaussDenom=1/((2*pi*stdr*stdr));

score = 0;

for i=1:size
    pin=mixparam;
    po=1-pin;
    
    
    pr=(pin*gaussDenom*exp(-(residuals(i))/(2*stdr*stdr)))+(po*(1/winddowsize));
    
    if(pr>1 || pr<0)
        display(['caught a bad pvi with value ' num2str(pr)]);
    end
    score=score+log(max(eps,pr));
end
score=-score;


end
function   [miu] = mixingparameterMLESAC(residuals,winsize)

npts=size(residuals,1);
z=zeros(npts,1);
po=findPOMLESAC(winsize);
miu=0.5;
counter=0;
oldmiu=10;
while(counter<7 && (abs(oldmiu-miu)<0.02))
    curPo=(1-miu)*po;
    oldmiu=miu;
    for j=1:npts
        pin= miu*findPiMLESAC(residuals(j,1),stdest);
        
        z(j,1)=(pin)/((pin+ curPo));
    end
    miu=mean( z);
    counter=counter+1;
    display(['new miu is ' num2str(miu)]);
end


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