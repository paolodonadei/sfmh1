function [inliers] =findInliers(residuals, pvis,t,pt,findInliersFunc)
if(findInliersFunc==5 || findInliersFunc==9 )
   
    inliers=find(pvis<pt);
    
else
    
    inliers=find(residuals<t);
    
end

    
    
    
end