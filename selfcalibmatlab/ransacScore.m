function [score] = ransacScore( size,consideredInliers, residuals,t)
score=0;

for i=1:size
    if(abs(residuals(i))>t)
        score=score+1;
        
    end
    
end

end