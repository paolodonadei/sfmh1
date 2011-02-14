function [scores] = scorefunctions(typenum, size,consideredInliers, residuals,t,pvis)

if(typenum==1 || typenum==2 )
    scores = ransacScore(typenum, size,consideredInliers, residuals,t);
elseif( typenum==3 || typenum==4 || typenum==5 || typenum==6)
    scores = msacScore(typenum, size,consideredInliers, residuals,t);
end




end
function [score] = likelihood(typenum, size,consideredInliers, residuals,t,pvis)
score=0;

for i=1:size
   %implement this
   % why did liang not use his own pvi? and use this stupid pvi?
end

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