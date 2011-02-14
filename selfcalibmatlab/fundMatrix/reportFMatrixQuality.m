function [ err ] =reportFMatrixQuality(F, x )

[bestInliers, bestF, residuals, meaner,varer,meder,numins] = sampsonF(F, x, 1.96*1.96);

display([' value of the errror was ' num2str(meaner)]);
display([' number of inliers is ' num2str(numins) ' out of ' num2str(size(x,2))]);
err=meaner;
end