
function [M, inliers,trialcount,pvis] = ransac(x, fittingfn, distfn, degenfn, s, t,errorFunc,randSampFunc ,initialPvi,updatepviFunc,updateIterationFunc,findInliersFunc)

debugf=1;

maxTrials = 6000;
maxDataTrials = 100;
feedback = 0;


if(debugf==1)
    global inlierOutlier;
    global corrsclean;
    inlierAccuracy=zeros(maxTrials,1);
    pvisAccuracy=zeros(maxTrials,1);
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
    fprintf( fid,[' trialcount, N, randSample , randSamplePvis ,accuracy of pvis,   accuracy of random sampling, error, ninliers , curError,  bestSet ']);
end





firstScoreFlag=0; % this is used to make sure the first score is set with calculated pvis not with fake uniform



[rows, npts] = size(x);
pvis= 1*ones(npts,1); % the initial pvi is not really an initial pvi, but the pvi calculation function uses it to mix new pvis, so we cant use it as a pvi
newpvis= 1*ones(npts,1);


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
        
        ind =randsampleengine(npts, s,pvis,randSampFunc);
        
        % Test that these points are not a degenerate configuration.
        degenerate = feval(degenfn, x(:,ind));
        
        if ~degenerate
            % Fit model to this random selection of data points.
            % Note that M may represent a set of models that fit the data in
            % this case M will be a cell array of models
            
            if(s==7)
                M = vgg_F_from_7pts_2img2(x(:,ind));
            elseif(s>7)
                
                M =  fundmatrix(x(:,ind));
            else
                display('wrong S');
                return;
            end
            
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
        numRealInliers=0;
        for vv=1:s
            fprintf(fid,[ num2str(ind(vv)) ]);
            if((exist('inlierOutlier'))==1 && size(inlierOutlier,2)>0 && inlierOutlier(1,ind(vv))==1)
                fprintf(fid,'* ');
                numRealInliers=numRealInliers+1;
            end
            if(vv~=s) fprintf(fid,[ ' - ']); end
        end
        fprintf(fid,[ ' , ']);
        for vv=1:s
            fprintf(fid,[ num2str(pvis(ind(vv))) ]);
            if(vv~=s) fprintf(fid,[ ' - ']); end
        end
        
        erpvis=0;
        if((exist('inlierOutlier'))==1)
            erpvis=mean(abs(pvis-(inlierOutlier')));
        end
     
        fprintf(fid,[ ' , ' num2str(erpvis)]);
        pvisAccuracy(trialcount+1,1)=erpvis;
        numRealInliers=numRealInliers/s;
        
        inlierAccuracy(trialcount+1,1)=numRealInliers;
        
        pviselected =[pviselected  pvis(ind)'];
        fprintf(fid,[ ' , ' num2str(inlierAccuracy(trialcount+1,1)) ' , ' ]);
    end
    % Once we are out here we should have some kind of model...
    % Evaluate distances between points and model returning the indices
    % of elements in x that are inliers.  Additionally, if M is a cell
    % array of possible models 'distfn' will return the model that has
    % the most inliers.  After this call M will be a non-cell object
    % representing only one model.
    [binliers, M, residuals, meaner,varer,meder,numins] = feval(distfn, M, x, t);
    
    if(debugf==1 && exist('corrsclean'))
        [dbinliers, dM, dresiduals, dmeaner,dvarer,dmeder,dnumins] = feval(distfn, M, corrsclean, t);
        fprintf(fid,[ ' e= ' num2str(dmeaner) ' , ' ]);
    elseif (debugf==1 && ~exist('corrsclean'))
        fprintf(fid,[ ' e= NA , ' ]);
    end
    
    
    
    [inliers] =findInliers(residuals, newpvis,t,0.0027,1);
    %
    %     ginliers=find(znewpvis>0.0027);
    %
    %     global inlierOutlier;
    
    % [ xx, centerloc ] = PeterSturmSelf( M,512,512 );
    %  display(['focal length was ' num2str(xx(1))]);
    
    % Find the number of inliers to this model.
    ninliers = length(inliers);
    
    curerror=scorefunctions(errorFunc, size(x,2),inliers ,residuals,t,pvis);
    
    
    
    if(debugf==1)
        fprintf(fid,[  num2str(ninliers)  ' , ' num2str(curerror)]);
    end
    
    
    if curerror < besterror    % Largest set of inliers so far...
        %   display(['best focal length was ' num2str(xx(1))]);
        
        if((exist('inlierOutlier'))==1 && debugf==1)
            display([' the best set had ' num2str( ninliers) ' inliers of which ' num2str((sum(inlierOutlier(inliers))/ninliers)*100) ' % were real inliers']);
        end
        [newpvis,initialPvi] = generalpviUpdate(updatepviFunc,initialPvi,pvis,residuals,t,inliers,x,trialcount , N);
        
        pvidiff=pvis-newpvis;
        
        
        if(firstScoreFlag<eps)% making sure pvis are used correctly
            curerror=scorefunctions(errorFunc, size(x,2),inliers ,residuals,t,pvis);
            firstScoreFlag=1;
        end
        
        if(debugf==1)
            fprintf(fid,[ ' ,  * , ']);
        end
        
        besterror = curerror;  % Record data for this model
        bestinliers = inliers;
        bestM = M;
        
        % Update estimate of N, the number of trials to ensure we pick,
        % with probability p, a data set with no outliers.
        N=calcIterations(updateIterationFunc,ninliers,npts,trialcount,pvis,pvidiff,s,p);
        
        
        %   if(N > trialcount)% update pvis if we need to further iterate, but if the best pvis were just found then forget it
        pvis=newpvis;
        %  end
        
        
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
    
    fprintf(fid,['\n name of error function is , ' num2str(errorFunc)]);
    if (nargin >7) fprintf(fid,['\n name of sampling function is , ' num2str(randSampFunc)]); end
    if (nargin >9)   fprintf(fid,['\n name of update pvi function is , ' num2str(updatepviFunc)]); end
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
    figure
    plot( inlierAccuracy(1:(trialcount),:));
    title('accuracy of random sampling versus iteration');
   figure
    plot( pvisAccuracy(1:(trialcount),:));
    title('error of Pvis versus iteration');
end

end