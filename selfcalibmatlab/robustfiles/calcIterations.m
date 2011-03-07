function [N]=calcIterations(updateIterationFunc,ninliers,npts,trialcount,pvis,pvidiff,s,p);

threshold=0.07; % I dont know what else to use or how to detect convergence
if(updateIterationFunc==5 )
    v=mean(abs(pvidiff));
  
      fracinliers =  ninliers/npts;
        pNoOutliers = 1 -  fracinliers^s;
        pNoOutliers = max(eps, pNoOutliers);  % Avoid division by -Inf
        pNoOutliers = min(1-eps, pNoOutliers);% Avoid division by 0.
        N = log(1-p)/log(pNoOutliers);
        
        
    if(v<threshold || N<trialcount)
         N=0; % make sure this is N=0 zzz 
    else
        
     N=N;
        
    end
elseif( updateIterationFunc==9)
      v=mean(abs(pvidiff));
  
      fracinliers =  mean(abs(pvis));
      display(['fraction of inliers is ' num2str(fracinliers)]);
        pNoOutliers = 1 -  fracinliers^s;
        pNoOutliers = max(eps, pNoOutliers);  % Avoid division by -Inf
        pNoOutliers = min(1-eps, pNoOutliers);% Avoid division by 0.
        N = log(1-p)/log(pNoOutliers);
        
        
    if(v<threshold || N<trialcount)
         N=0; % make sure this is N=0 zzz 
    else
        
     N=N;
        
    end
else
     fracinliers =  ninliers/npts;
        pNoOutliers = 1 -  fracinliers^s;
        pNoOutliers = max(eps, pNoOutliers);  % Avoid division by -Inf
        pNoOutliers = min(1-eps, pNoOutliers);% Avoid division by 0.
        N = log(1-p)/log(pNoOutliers);
    
end




end