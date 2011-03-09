function [N]=calcIterations(updateIterationFunc,ninliers,npts,trialcount,pvis,pvidiff,s,p,inliers)

threshold=0.06; % I dont know what else to use or how to detect convergence
if(updateIterationFunc==5 || updateIterationFunc==9 )
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
elseif (updateIterationFunc==10 )
        v=mean(abs(pvidiff(inliers',1)));
  
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
    
    
else
     fracinliers =  ninliers/npts;
        pNoOutliers = 1 -  fracinliers^s;
        pNoOutliers = max(eps, pNoOutliers);  % Avoid division by -Inf
        pNoOutliers = min(1-eps, pNoOutliers);% Avoid division by 0.
        N = log(1-p)/log(pNoOutliers);
    
end




end