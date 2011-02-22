function [inliers] =findInliers(residuals, pvis,t,pt,findInliersFunc)
if(findInliersFunc==5)
   
    inliers=find(pvis<pt);
    
else
    
    inliers=find(residuals<t);
    
end

    
    
    
end