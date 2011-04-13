function [scores] = scorefunctions(typenum, size,consideredInliers, residuals,t,pvis)

if(typenum==1 || typenum==2 )
    scores = ransacScore(typenum, size,consideredInliers, residuals,t);
elseif( typenum==6 || typenum==7)
    scores = likelihood(typenum, size,consideredInliers, residuals,t,pvis);
elseif(typenum==Inf )
    scores = likelihoodfixed(typenum, size,consideredInliers, residuals,t,pvis);
else %( typenum==3 || typenum==4 || typenum==8 || typenum==5  )
    scores = msacScore(typenum, size,consideredInliers, residuals,t);
end



end

function [score] = likelihoodfixed(typenum, size,consideredInliers, residuals,t,pvis)
score=0;
for i=1:size
    pr=pvis(i,1);
    score=score+log(max(eps,pr));
end

score=-score;

end


function [score] = likelihood(typenum, size,consideredInliers, residuals,t,pvis)
% we assume residuals are s


score = 0;

for i=1:size

    score=score+log(max(eps,pr));
end
score=-score;
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