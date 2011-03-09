function [errorin, errorout,errorstot, errors] = pvifitness(inliers,pvis)




%  error of the pvis. ideally we would want the label of inliers to be 1 an
% d that of outliers to be 0. So the way we find the error is to find the
% mean of the differences of all the labels. But of course this would
% require the pvis to be scaled from [0-1] which we have done
[m,n]=size(pvis);
errors=zeros(n,1);

for i=1:m
    errors(i,1) =abs(pvis(i)-inliers(i));
    
end


errorin=sum(errors(find(inliers==1)))/(eps+sum(inliers));
errorout=sum(errors(find(inliers==0)))/(m-sum(inliers)+eps);
errorstot=mean( errors);

% display(['mean of inlier pvis is: ' num2str(errorin)]);
% display(['mean of outlier pvis is: ' num2str(errorout)]);
% display(['mean of pvis is: ' num2str(errorstot)]);
% % % 
% display(['mean error of inliers is : ' num2str(errorin)]);
% display(['mean error of outliers is : ' num2str(errorout)]);
% display(['**total error is : ' num2str((errorin+errorout)/2)]);
% 
% subplot(2,2,1)
% hist(pvis,100);
% title([ ' histogram of pvis']);
% subplot(2,2,2)
% hist(errors,100);
% title([ ' histogram of pvi errors']);
% subplot(2,2,3)
% 
% hist(pvis(find(inliers==1)),100);
% title([ ' histogram of inlier pvis']);
% subplot(2,2,4)
% 
% hist(pvis(find(inliers==0)),100);
% title([ ' histogram of outlier pvis']);


end




