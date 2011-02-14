function [scores] = scorefunctions(typenum, size,consideredInliers, residuals,t)

if(typenum==1 || typenum==2 )    
            scores = ransacScore(typenum, size,consideredInliers, residuals,t);
elseif( typenum==3 || typenum==4 || typenum==5)
      scores = msacScore(typenum, size,consideredInliers, residuals,t);
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