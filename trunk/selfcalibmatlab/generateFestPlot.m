function [ t,means_F,medians_F,  variances_F ,means_XY,medians_XY,  variances_XY ] =     generateFestPlot(type, variance,repeat, startp, endp )


seqname='merton1';
numN=200;
tt=1.96; %threshold for deciding outliers , not sure
numPoints=20;


tStartprogram=tic;

if (nargin == 9)
    startp=0;
    endp=1;
end


savefoldername='datafilesdFest';
direxists=1;
dircount=0;

mtime=clock;
subdirname=['subdir_m_' num2str(mtime(1,2)) '_d_' num2str(mtime(1,3)) '_h_' num2str(mtime(1,4)) '_s_' num2str(ceil(mtime(1,5))) '_v'];
while(direxists==1)
    
    dircount=dircount+1;
    
    curdirname=[savefoldername '/' subdirname num2str(dircount)];
    
    if(exist(curdirname,'dir')==0)
        mkdir(curdirname);
        direxists=0;
    end
    
end


styles={'-.or' ,'-.xg', '-.+b', '-.*y', '-.vr' ,'-..c'};
t=zeros(1,numPoints);
label='empty';
nowtime=num2str(sum(round(100*clock)));
%Algs
fid = fopen([curdirname '/exp' nowtime '.txt'], 'w');
fidgraph = fopen([curdirname '/graphdata' nowtime '.txt'], 'w');
dispfid = fopen([curdirname '/dispcommands' nowtime '.txt'], 'w');

AlgNames={ 'Un-robust','RANSAC'};
AlgFuncs={ @fundmatrixunrobust ,@ransackovesi};



numalgs=size(AlgFuncs,2);

%outputs
means_F=zeros(numalgs,numPoints);
medians_F=zeros(numalgs,numPoints);
variances_F=zeros(numalgs,numPoints);

means_XY=zeros(numalgs,numPoints);
medians_XY=zeros(numalgs,numPoints);
variances_XY=zeros(numalgs,numPoints);

%arg paramters




step=(  endp-startp)/numPoints;
n=startp:step:endp;

t=n(1,1:numPoints);
label='noise-level';




numTotalIterations=numPoints*repeat;
currIteration=0;

%allSolutions=cell(numPoints,repeat,numalgs);


fprintf(fidgraph, ' , ');
for k=1:numalgs
    fprintf(fidgraph, [  AlgNames{1,k} '_meanE , ' AlgNames{1,k} '_medianE , '  AlgNames{1,k} '_variance , ']);
end
fprintf(fidgraph, '\n');




for i=1:numPoints
    comletesolsf= fopen([curdirname '/sols' num2str(i) '_nowtime_.txt'], 'w');
    fprintf(comletesolsf, ' , ');
    for k=1:numalgs
        fprintf(comletesolsf, [  AlgNames{1,k} '_sampsonerrorMean , ' AlgNames{1,k} '_sampsonerrorVariance , '  AlgNames{1,k} '_sampsonerrormedian , ' AlgNames{1,k} '_nInliers , ' AlgNames{1,k} '_iterations , ' ]);
    end
    
    
    % no, in selfplot this line is there, i dont think i need it clear  current_errors_F current_errors_XY current_BADPTS ;
    current_errors_samp_mean{i}=zeros(numalgs,repeat);
    current_errors_samp_var{i}=zeros(numalgs,repeat);
    current_errors_samp_median{i}=zeros(numalgs,repeat);
    current_errors_inlier{i}=zeros(numalgs,repeat);
    current_errors_iterations{i}=zeros(numalgs,repeat);
    
    for j=1:repeat
        fprintf(comletesolsf, [' \n ' num2str(j) ' , '] );
        tStart=tic;
        currIteration=currIteration+1;
        
        clear Fgt k1 k2 corrs inlierOutlier  I1  I2   R1  t1 R2 t2 totalAgltime x1 x2 xombined
        totalAgltime=0;
        
        
        [ Fgt,k1,k2,corrs,inlierOutlier, I1, I2,  R1, t1,R2,t2 ] = generateCorrsforF(numN, n(1,i), variance, type, seqname );
        
        [mm,nn]=size(corrs);
        % augmenting corrs with zeros for homo coordinates
        x1 = [ corrs(1:2,:); ones(1,nn)];    % Extract x1 and x2 from x
        x2 = [ corrs(3:4,:); ones(1,nn)];
        xcombined = [x1 ; x2];
        disp(['****iteration ' num2str(currIteration) ' out of ' num2str(numTotalIterations) '   AND calling generateCorrsforF( ' num2str((numN)) ' , ' num2str( n(1,i)) ' , '  num2str( variance) ' , '  seqname ')'] );
        fprintf(dispfid,['****iteration ' num2str(currIteration) ' out of ' num2str(numTotalIterations) '   AND calling generateCorrsforF( ' num2str((numN)) ' , ' num2str( n(1,i)) ' , '  num2str( variance) ' , '  seqname ')']);
        for k=1:numalgs
            
            tic;
            clear F iterations
            [F, current_errors_iterations{i}(k,j)]=AlgFuncs{k}(x1,x2); %assuming camera size is 512x512
            PtElapsed=toc;
            totalAgltime=totalAgltime+PtElapsed;
            
            [bestInliers, bestF, d, current_errors_samp_mean{i}(k,j),current_errors_samp_var{i}(k,j),current_errors_samp_median{i}(k,j),current_errors_inlier{i}(k,j)] = sampsonF(F, xcombined, tt);
            
            fprintf(comletesolsf, [  num2str(current_errors_samp_mean{i}(k,j)) ' , ' num2str(current_errors_samp_var{i}(k,j)) ' , ' num2str(current_errors_samp_median{i}(k,j)) ' , '  num2str(current_errors_inlier{i}(k,j))  ' , ']);
            
            disp(['algorithm: ' AlgNames{k} ' had mean error ' num2str(current_errors_samp_mean{i}(k,j)) ' and error variance: ' num2str(current_errors_samp_var{i}(k,j))  ' inliers: ' current_errors_inlier{i}(k,j) ' time: ' num2str(PtElapsed)]);
            fprintf(dispfid,['\nalgorithm: ' AlgNames{k} ' had mean error ' num2str(current_errors_samp_mean{i}(k,j)) ' and error variance: ' num2str(current_errors_samp_var{i}(k,j)) ' and error median: ' num2str(current_errors_samp_median{i}(k,j)) ' iterations: ' current_errors_iterations{i}(k,j) ' inliers: ' current_errors_inlier{i}(k,j) ' time: ' num2str(PtElapsed)]);
            
            
            
            fprintf(fid, 'algorithm %s mean error %6.2f median error %6.2f variance error %6.2f number inliers %d number iterations %d and time elapsed is %6.2f \n',AlgNames{k},current_errors_samp_mean{i}(k,j),current_errors_samp_mean{i}(k,j),current_errors_samp_mean{i}(k,j),current_errors_inlier{i}(k,j) ,current_errors_iterations{i}(k,j) ,PtElapsed  );
            
        end
        
        tElapsed=toc(tStart);
        disp(['iteration ' num2str(currIteration) ' took ' num2str(tElapsed) ' seconds' ' and total time spent in algs is ' num2str(totalAgltime) ' time remaining: ' ' out of ' num2str(tElapsed*(numTotalIterations-currIteration))]);
        fprintf(dispfid,['\niteration ' num2str(currIteration) ' took ' num2str(tElapsed) ' seconds' ' and total time spent in algs is ' num2str(totalAgltime) ' time remaining: ' ' out of ' num2str(tElapsed*(numTotalIterations-currIteration))]);
    end
    
    disp('______________________________________________________');
    fprintf(dispfid,['\n______________________________________________________']);
    
    fprintf(fidgraph, '%6.2f , ' ,t(1,i));
    %now calculate the stat for the current run
    for k=1:numalgs
        means_mean(k,i)=mean(current_errors_samp_mean{i}(k,:));
        medians_mean(k,i)=median(current_errors_samp_mean{i}(k,:));
        variances_mean(k,i)=var(current_errors_samp_mean{i}(k,:));
        
        means_med(k,i)=mean(current_errors_samp_median{i}(k,:));
        medians_med(k,i)=median(current_errors_samp_median{i}(k,:));
        variances_med(k,i)=var(current_errors_samp_median{i}(k,:));
        
        means_var(k,i)=mean(current_errors_samp_var{i}(k,:));
        means_inliers(k,i)=mean(current_errors_inlier{i}(k,:));
        means_iterations(k,i)=mean(current_errors_iterations{i}(k,:));
        
        fprintf(fidgraph, ' %6.2f , %6.2f  , %6.2f ,  %6.2f , %6.2f  ,  %6.2f , %6.2f  ,  %6.2f , %6.2f ,' ,means_mean(k,i),medians_mean(k,i),  variances_mean(k,i),means_med(k,i),medians_med(k,i),variances_med(k,i), means_var(k,i),  means_inliers(k,i),means_iterations(k,i));
    end
    
    fprintf(fidgraph, ' \n');
    fclose(comletesolsf);
    
end

% for focal length
data={current_errors_samp_mean, current_errors_samp_median,current_errors_samp_var,current_errors_inlier,current_errors_iterations};
dataNames={'means', 'variances', 'medians','inliers','iterations'};
sizeDataCats=size(data,2);


figure;
hold;
for k=1:numalgs
    
    plot(t,means_mean(k,:),styles{k});
end
xlabel(['x noise ratio']);       %  add axis labels and plot title
ylabel('y mean of mean errors');
title([ ' plot of mean of mean errors versus error in focal length estimation']);
legend(AlgNames);

%  saveas(gcf,['param' paramcheck '_' dataNames{i} nowtime '.eps']);
saveas(gcf,[curdirname '/param_focal_e_' dataNames{i} nowtime '.jpg']);
saveas(gcf,[curdirname '/param_focal_e_' dataNames{i} nowtime '.fig']);
saveas(gcf,[curdirname '/param_focal_e_' dataNames{i} nowtime '.eps'],'epsc');
hold







fclose(fid);
fclose(fidgraph);
fclose(dispfid);
save( [curdirname '/variables_GP' nowtime '.mat']);

tElapsedprogram=toc(tStartprogram);
disp([' program took ' num2str(tElapsedprogram) ' seconds']);
copyfile('*.m',[curdirname '\matlabfiles']);
end

%make a generic plotting script that plots the variance and mean and median
%of every measure

% write a proper mapsac and ransac function that uses a function to
% calculate the score and also doesnt normalize beforehand, also i should
% probably write a mlesac fucntion