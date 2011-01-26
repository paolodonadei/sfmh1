
function [M, inliers,trialcount] = ransac(x, fittingfn, distfn, degenfn, s, t,errorFunc,randSampFunc ,initialPvi,updatepviFunc)

Octave = exist('OCTAVE_VERSION') ~= 0;
debugf=0;

if(debugf==1)
    pviselected =[];
    debugdirname='debugransac';
    seedname='ransacdebug';
    fexists=1;
    counter=1;
    dbgfname='';
    if(exist(debugdirname,'dir')==0)
        mkdir(debugdirname);
    end
    
    while( fexists~=0)
        counter= counter+1;
        dbgfname=[debugdirname '/' seedname num2str(counter) '.csv'];
        fexists=exist(dbgfname,'file');
    end
    fid = fopen(dbgfname, 'w');
    fprintf( fid,[' trialcount, N, randSample , randSamplePvis , ninliers , curError,  bestSet ']);
end

maxTrials = 6000;
maxDataTrials = 100;
feedback = 0;
if nargin < 9
    initialPvi=ones(1,size(x,2));
end

pvis= initialPvi;




[rows, npts] = size(x);

p = 0.99;         % Desired probability of choosing at least one sample
% free from outliers

bestM = NaN;      % Sentinel value allowing detection of solution failure.
trialcount = 0;
besterror =  100000000000;
N = 1;            % Dummy initialisation for number of trials.

while N > trialcount
    
    if(debugf==1)
        fprintf(fid,[ '\n' num2str(trialcount) ' , ' num2str(N) ' , ']);
    end
    % Select at random s datapoints to form a trial model, M.
    % In selecting these points we have to check that they are not in
    % a degenerate configuration.
    degenerate = 1;
    count = 1;
    while degenerate
        % Generate s random indicies in the range 1..npts
        % (If you do not have the statistics toolbox, or are using Octave,
        % use the function RANDOMSAMPLE from my webpage)
        
        
        if nargin < 8
            ind =  randsample(npts, s);
        else
            ind = feval(randSampFunc,npts, s,pvis);
        end
        
        
        
        % Test that these points are not a degenerate configuration.
        degenerate = feval(degenfn, x(:,ind));
        
        if ~degenerate
            % Fit model to this random selection of data points.
            % Note that M may represent a set of models that fit the data in
            % this case M will be a cell array of models
            M = feval(fittingfn, x(:,ind));
            
            % Depending on your problem it might be that the only way you
            % can determine whether a data set is degenerate or not is to
            % try to fit a model and see if it succeeds.  If it fails we
            % reset degenerate to true.
            if isempty(M)
                degenerate = 1;
                %     display('empty F');
            end
            
        end
        
        
        
        % Safeguard against being stuck in this loop forever
        count = count + 1;
        if count > maxDataTrials
            warning('Unable to select a nondegenerate data set');
            break
        end
    end
    
    if(debugf==1)
        for vv=1:s
            fprintf(fid,[ num2str(ind(vv)) ]);
            if(vv~=s) fprintf(fid,[ ' - ']); end
        end
        fprintf(fid,[ ' , ']);
        for vv=1:s
            fprintf(fid,[ num2str(pvis(ind(vv))) ]);
            if(vv~=s) fprintf(fid,[ ' - ']); end
        end
        fprintf(fid,[ ' , ']);
        
        pviselected =[pviselected  pvis(ind)'];
    end
    % Once we are out here we should have some kind of model...
    % Evaluate distances between points and model returning the indices
    % of elements in x that are inliers.  Additionally, if M is a cell
    % array of possible models 'distfn' will return the model that has
    % the most inliers.  After this call M will be a non-cell object
    % representing only one model.
    [inliers, M, residuals, meaner,varer,meder,numins] = feval(distfn, M, x, t);
    
  % [ xx, centerloc ] = PeterSturmSelf( M,512,512 );
  %  display(['focal length was ' num2str(xx(1))]);
    
    % Find the number of inliers to this model.
    ninliers = length(inliers);
    
    curerror=feval(errorFunc, size(x,2),inliers ,residuals,t);
    
    if(debugf==1)
        fprintf(fid,[  num2str(ninliers)  ' , ' num2str(curerror)]);
    end
    
    
    if curerror < besterror    % Largest set of inliers so far...
     %   display(['best focal length was ' num2str(xx(1))]);
        if nargin >9
            [pvis,initialPvi ] = feval(updatepviFunc,initialPvi,pvis,residuals,t,inliers,x);
        end
        
        if(debugf==1)
            fprintf(fid,[ ' ,  * , ']);
        end
        
        besterror = curerror;  % Record data for this model
        bestinliers = inliers;
        bestM = M;
        
        % Update estimate of N, the number of trials to ensure we pick,
        % with probability p, a data set with no outliers.
        fracinliers =  ninliers/npts;
        pNoOutliers = 1 -  fracinliers^s;
        pNoOutliers = max(eps, pNoOutliers);  % Avoid division by -Inf
        pNoOutliers = min(1-eps, pNoOutliers);% Avoid division by 0.
        N = log(1-p)/log(pNoOutliers);
        %    display(['number of inliers is ' num2str(ninliers)]);
    else
        if(debugf==1)
            fprintf(fid,[ ' ,   , ']);
        end
    end
    
    trialcount = trialcount+1;
    if feedback
        fprintf('trial %d out of %d         \r',trialcount, ceil(N));
    end
    
    % Safeguard against being stuck in this loop forever
    if trialcount > maxTrials
        warning( ...
            sprintf('ransac reached the maximum number of %d trials',...
            maxTrials));
        break
    end
end
fprintf('\n');

if ~isnan(bestM)   % We got a solution
    M = bestM;
    inliers = bestinliers;
else
    M = [];
    inliers = [];
    error('ransac was unable to find a useful solution');
end

if(debugf==1)
    [ST,I] = dbstack;
   
     fprintf(fid,['\n\n\n name of function is , '  ST(2).name]);
    fprintf(fid,['\n mean of pvis was , ' num2str(mean(pvis))]);
    fprintf(fid,['\n median of pvis was , ' num2str(median(pvis))]);
    fprintf(fid,['\n mean of pvis selected was , ' num2str(mean( pviselected'))]);
    fprintf(fid,['\n median of pvis selected  was , ' num2str(median( pviselected'))]);
    fprintf(fid,['\n max of pvis selected was , ' num2str(max( pviselected'))]);
    fprintf(fid,['\n min of pvis selected  was , ' num2str(min( pviselected'))]);
    
    fprintf(fid,['\n name of error function is , ' func2str(errorFunc)]);
    if (nargin >7) fprintf(fid,['\n name of sampling function is , ' func2str(randSampFunc)]); end
    if (nargin >9)   fprintf(fid,['\n name of update pvi function is , ' func2str(updatepviFunc)]); end
    tm=clock;
    fprintf(fid,['\n time is : , ' num2str(tm(1)) ' , ' num2str(tm(2)) ' , ' num2str(tm(3)) ' , ' num2str(tm(4)) ' , ' num2str(tm(5)) ' , ']);
    fclose(fid);
    
%     hist(pvis,100);
%     title([ ' histogram of pvis for '  ST(2).name]);
%     saveas(gcf,[debugdirname '/'   seedname num2str(counter) 'pvis_all.png']);
%     figure
%     hist(pviselected',100);
%     title([ ' histogram of pvis selected for '  ST(2).name]);
%     saveas(gcf,[debugdirname '/'   seedname num2str(counter) 'pvis_selected.png']);
end

end
