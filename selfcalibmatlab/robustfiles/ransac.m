
function [M, inliers,trialcount,pvis] = ransac(x, distfn, degenfn, s, t,errorFunc,randSampFunc ,initialPvi,updatepviFunc,updateIterationFunc,stdest,winsize,prever)

debugf=0;
numTest=0;




maxTrials = 8000;
maxDataTrials = 100;
feedback = 0;

iterationLastUpdated=0;
if(debugf==1)
    
    
    close all
    global inlierOutlier;
    global corrsclean;
    
    imageA=ones(2000,1 ,3);
    imagesteps=2000/size(x,2);
    imagethickness=40;
    
    tempim=zeros(2000,imagethickness ,3);
    for kk=1:size(x,2)
        
        if(inlierOutlier(1,kk)==1)
            tempim((1+(kk-1)*(imagesteps)):((kk)*(imagesteps)),:,2)=tempim((1+(kk-1)*(imagesteps)):((kk)*(imagesteps)),:,2)+1;
        else
            tempim((1+(kk-1)*(imagesteps)):((kk)*(imagesteps)),:,1)=tempim((1+(kk-1)*(imagesteps)):((kk)*(imagesteps)),:,1)+1;
        end
        tempim(((kk)*(imagesteps)),:,3)=   tempim(((kk)*(imagesteps)),:,3)+1;
    end
    
    imageA=[imageA tempim zeros(2000,20 ,3)];
    
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
        dbgfnameBX=[debugdirname '/' seedname num2str(counter) 'BX.csv'];
        fexists=exist(dbgfname,'file');
    end
    fid = fopen(dbgfname, 'w');
    fprintf( fid,[' trialcount, N, randSample , randSamplePvis ,accuracy of pvis,   accuracy of random sampling, error, ninliers , curError,  bestSet ']);
    
    PVIBOX=[];
    PVIBOX=[PVIBOX inlierOutlier'];
end





firstScoreFlag=0; % this is used to make sure the first score is set with calculated pvis not with fake uniform



[rows, npts] = size(x);
pvis= 0.5*ones(npts,1); % the initial pvi is not really an initial pvi, but the pvi calculation function uses it to mix new pvis, so we cant use it as a pvi
newpvis= 0.5*ones(npts,1);


p = 0.99;         % Desired probability of choosing at least one sample
% free from outliers

bestM = NaN;      % Sentinel value allowing detection of solution failure.
trialcount = 0;
besterror =  100000000000;
N = 1;            % Dummy initialisation for number of trials.
if(prever==2)
    s=8;
    numTest=1;
    N=10000;
end

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
    
    
    if(prever==2 )
        [vbinliers, vM, vresiduals, vmeaner,vvarer,vmeder,vnumins] = feval(distfn, M, x, t,numTest,ind);
        if(vmeaner>5)
       %     display(['skippped ' num2str(vmeaner)]);
            trialcount = trialcount+1;
            
            continue;
        end
     %   display(['NOT skippped' num2str(vmeaner)]);
    end
    
    [binliers, M, residuals, meaner,varer,meder,numins] = feval(distfn, M, x, t,0);
    
    if(debugf==1 && exist('corrsclean'))
        [dbinliers, dM, dresiduals, dmeaner,dvarer,dmeder,dnumins] = feval(distfn, M, corrsclean, t,numTest);
        fprintf(fid,[ ' e= ' num2str(dmeaner) ' , ' ]);
    elseif (debugf==1 && ~exist('corrsclean'))
        fprintf(fid,[ ' e= NA , ' ]);
    end
    
    
    
    inliers=find(residuals<t);
    
    %
    %     ginliers=find(znewpvis>0.0027);
    %
    %     global inlierOutlier;
    
    % [ xx, centerloc ] = PeterSturmSelf( M,512,512 );
    %  display(['focal length was ' num2str(xx(1))]);
    
    % Find the number of inliers to this model.
    ninliers = length(inliers);
    
    curerror=scorefunctions(errorFunc, size(x,2),inliers ,residuals,t,pvis,stdest,winsize,x);
    
    
    
    if(debugf==1)
        
        fprintf(fid,[  num2str(ninliers)  ' , ' num2str(curerror)]);
    end
    
    
    if curerror < besterror    % Largest set of inliers so far...
        
        besterror = curerror;  % Record data for this model
        bestinliers = inliers;
        bestM = M;
        
        iterationLastUpdated=0;
        %   display(['best focal length was ' num2str(xx(1))]);
        
        if((exist('inlierOutlier'))==1 && debugf==1)
            display([' the best set had ' num2str( ninliers) ' inliers of which ' num2str((sum(inlierOutlier(inliers))/ninliers)*100) ' % were real inliers']);
        end
        [newpvis,initialPvi] = generalpviUpdate(updatepviFunc,initialPvi,pvis,residuals,t,inliers,x,trialcount , N);
        
        pvidiff=pvis-newpvis;
        
        
        if(firstScoreFlag<eps)% making sure pvis are used correctly
            curerror=scorefunctions(errorFunc, size(x,2),inliers ,residuals,t,pvis,stdest,winsize,x);
            firstScoreFlag=1;
        end
        
        if(debugf==1)
            fprintf(fid,[ ' ,  * , ']);
            
            tempim=zeros(2000,imagethickness ,3);
            tempim(:,:,1)=tempim(:,:,1)+1;
            
            
            for vv=1:size(bestinliers,2)
                kk=bestinliers(1,vv);
                tempim((1+(kk-1)*(imagesteps)):((kk)*(imagesteps)),:,2)=tempim((1+(kk-1)*(imagesteps)):((kk)*(imagesteps)),:,2)+1;
            end
            
            
            for kk=1:size(x,2)
                tempim(((kk)*(imagesteps)),:,3)=   tempim(((kk)*(imagesteps)),:,3)+1;
            end
            
            
            
            imageA=[imageA tempim zeros(2000,5,3)];
            
        end
        
        
        
        % Update estimate of N, the number of trials to ensure we pick,
        % with probability p, a data set with no outliers.
        N=calcIterations(updateIterationFunc,ninliers,npts,trialcount,pvis,pvidiff,s,p,inliers);
        
        
        %   if(N > trialcount)% update pvis if we need to further iterate, but if the best pvis were just found then forget it
        pvis=newpvis;
        %  end
        if(debugf==1)
            PVIBOX=[PVIBOX pvis];
        end
        %    display(['number of inliers is ' num2str(ninliers)]);
    else
        if(debugf==1)
            fprintf(fid,[ ' ,   , ']);
        end
        
        
    end
    iterationLastUpdated=iterationLastUpdated+1;
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
    csvwrite(dbgfnameBX,PVIBOX);
    
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
    
    stringv={'outlier','inlier'};
    if(1==2)
        
        for k=1:size(PVIBOX,1)
            figure
            hist(PVIBOX(k,:),20);
            title([' point number ' num2str(k) ' ' stringv{1+inlierOutlier(1,k)}]);
            figure
            plot(PVIBOX(k,:));
            close all
        end
        
    end
    
    tempim=ones(2000,imagethickness ,3);
    
    proposedinliers=zeros(size(x,2),1);
    
    for vv=1:size(bestinliers,2)
        proposedinliers(bestinliers(1,vv),1)=1;
    end
    
    proposedinliers=(proposedinliers-inlierOutlier') ;
    for kk=1:size(x,2)
        if(proposedinliers(kk,1)>0)
            tempim((1+(kk-1)*(imagesteps)):((kk)*(imagesteps)),:,:)=  0;
        elseif (proposedinliers(kk,1)<0)
            tempim((1+(kk-1)*(imagesteps)):((kk)*(imagesteps)),:,2)=  0;
            tempim((1+(kk-1)*(imagesteps)):((kk)*(imagesteps)),:,3)=  0;
        end
        
    end
    
    imageA=[imageA zeros(2000,15,3) tempim ];
    
    % writing probabilities
    tempim=zeros(2000,imagethickness ,3);
    
    for kk=1:size(x,2)
        tempim((1+(kk-1)*(imagesteps)):((kk)*(imagesteps)),:,2)=  tempim((1+(kk-1)*(imagesteps)):((kk)*(imagesteps)),:,2)+(1*pvis(kk,1));
        
    end
    
    imageA=[imageA zeros(2000,20,3) tempim ];
    
    imshow( imageA);
    imwrite(imageA,[debugdirname '/matches.png']);
    title({'the first column shows the ground truth, red is outlier, green is inlier';'the second last shows the errors, red is points that were taken as outliers but were inliers in reality, opposite with the blacks';' last column shows the pvis, intensity shows the value of the probability'});
    
end

end
