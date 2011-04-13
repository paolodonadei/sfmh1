function [ t,means_mean ] =     generateFestPlot(type, variance,repeat, startp, endp )

tt=1.96*1.96;
seqname='merton1';
numN=200;
numPoints=10;


tStartprogram=tic;

if (nargin == 3)
    startp=0;
    endp=1;
end


savefoldername='festdatafiles';
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


styles={'-.or' ,'-.xg', '-.+b', '-.*y', '-.vr' ,'-..c','g--'};
t=zeros(1,numPoints);
label='empty';
nowtime=num2str(sum(round(100*clock)));
%Algs
fid = fopen([curdirname '/algresults.txt'], 'w');
fidgraph = fopen([curdirname '/graphdata.csv'], 'w');
dispfid = fopen([curdirname '/dispcommands' nowtime '.txt'], 'w');


% AlgNames={ 'RANSAC','Residuals','cookFixed','cookUpdate','Liang','CookUpdateLikelihood'};
% AlgFuncs={1,3,4,5,6,7};

AlgNames={ 'MSAC','cookUpdate','compete9','compete10','compete11'};
AlgFuncs={2,5,9,10,11};

numalgs=size(AlgFuncs,2);

if(size(AlgFuncs,2)~=size(AlgFuncs,2))
    
    display('you for got to add correct params, exit now');
    return;
end


%outputs


means_mean=zeros(numalgs,numPoints);
medians_mean=zeros(numalgs,numPoints);
variances_mean=zeros(numalgs,numPoints);

means_med=zeros(numalgs,numPoints);
medians_med=zeros(numalgs,numPoints);
variances_med=zeros(numalgs,numPoints);

means_var=zeros(numalgs,numPoints);
means_inliers=zeros(numalgs,numPoints);
means_iterations=zeros(numalgs,numPoints);

%arg paramters




step=(  endp-startp)/numPoints;
n=startp:step:endp;

t=n(1,1:numPoints);





numTotalIterations=numPoints*repeat;
currIteration=0;

%allSolutions=cell(numPoints,repeat,numalgs);


fprintf(fidgraph, ' error Ratio , ');
for k=1:numalgs
    fprintf(fidgraph, [  AlgNames{1,k} '_meanOFmeanE , ' AlgNames{1,k} '_medianOFmeanE , '  AlgNames{1,k} '_varianceOFmean , '  AlgNames{1,k} '_meanOFmedianE , ' AlgNames{1,k} '_medianOFmedianE , '  AlgNames{1,k} '_varianceOFmedian , '   AlgNames{1,k} '_meanOFvariance , ' AlgNames{1,k} '_iterations , ' AlgNames{1,k} '_inliers , ']);
end
fprintf(fidgraph, '\n');


current_errors_samp_mean=cell(numPoints);
current_errors_samp_var=cell(numPoints);
current_errors_samp_median=cell(numPoints);
current_errors_inlier=cell(numPoints);
current_errors_iterations=cell(numPoints);


for i=1:numPoints
    comletesolsf= fopen([curdirname '/sols_t_' num2str(i) '_.txt'], 'w');
    fprintf(comletesolsf, ' repeat , ');
    for k=1:numalgs
        fprintf(comletesolsf, [  AlgNames{1,k} '_sampsonerrorMean , ' AlgNames{1,k} '_sampsonerrorVariance , '  AlgNames{1,k} '_sampsonerrormedian , '  AlgNames{1,k} '_iterations , ' ]);
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
        
        
        [ Fgt,k1,k2,corrs,corrsclean, inlierOutlier, I1, I2,  R1, t1,R2,t2 ] = generateCorrsforF(numN, n(1,i), variance, type, seqname );
        
        [mm,nn]=size(corrs);
        
        
        
        disp(['****iteration ' num2str(currIteration) ' out of ' num2str(numTotalIterations) '   AND calling generateCorrsforF( ' num2str((numN)) ' , ' num2str( n(1,i)) ' , '  num2str( variance) ' , '  seqname ')'] );
        fprintf(dispfid,['****iteration ' num2str(currIteration) ' out of ' num2str(numTotalIterations) '   AND calling generateCorrsforF( ' num2str((numN)) ' , ' num2str( n(1,i)) ' , '  num2str( variance) ' , '  seqname ') \n']);
        for k=1:numalgs
            
            tic;
            clear F iterations pvizout
            
            [F, current_errors_iterations{i}(k,j),pvizout]= fundmatrixrobustgeneral(corrs,AlgFuncs{k});
            PtElapsed=toc;
            totalAgltime=totalAgltime+PtElapsed;
            
            [bestInliers, bestF, d, current_errors_samp_mean{i}(k,j),current_errors_samp_var{i}(k,j),current_errors_samp_median{i}(k,j),current_errors_inlier{i}(k,j)] = sampsonF(F, corrsclean, tt);
            
            fprintf(comletesolsf, [  num2str(current_errors_samp_mean{i}(k,j)) ' , ' num2str(current_errors_samp_var{i}(k,j)) ' , ' num2str(current_errors_samp_median{i}(k,j)) ' , '  num2str(current_errors_inlier{i}(k,j))  ' , ']);
            
            disp(['algorithm: ' AlgNames{k} ' had mean error ' num2str(current_errors_samp_mean{i}(k,j)) ' and error variance: ' num2str(current_errors_samp_var{i}(k,j))  ' inliers: '  num2str(current_errors_inlier{i}(k,j)) ' time: ' num2str(PtElapsed)]);
            fprintf(dispfid,['\nalgorithm: ' AlgNames{k} ' had mean error ' num2str(current_errors_samp_mean{i}(k,j)) ' and error variance: ' num2str(current_errors_samp_var{i}(k,j)) ' and error median: ' num2str(current_errors_samp_median{i}(k,j)) ' iterations: '  num2str(current_errors_iterations{i}(k,j)) ' inliers: '  num2str(current_errors_inlier{i}(k,j)) ' time: ' num2str(PtElapsed)]);
            
            
            
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
        
        fprintf(fidgraph, ' %6.2f , %6.2f  , %6.2f ,  %6.2f , %6.2f  ,  %6.2f , %6.2f  ,  %6.2f  ,   %6.2f  ,' ,means_mean(k,i),medians_mean(k,i),  variances_mean(k,i),means_med(k,i),medians_med(k,i),variances_med(k,i), means_var(k,i),means_iterations(k,i),means_inliers(k,i));
    end
    
    fprintf(fidgraph, ' \n');
    fclose(comletesolsf);
    
end

% for focal length
data={means_mean,  medians_mean,variances_mean,means_inliers,means_iterations };
dataNames={'means of errors', 'medians of errors', 'variance of errors','inliers','iterations'};
sizeDataCats=size(data,2);

for i=1:sizeDataCats
    % the mean stuff
    figure;
    hold;
    for k=1:numalgs
        
        plot(t,data{i}(k,:),styles{k});
    end
    xlabel(['x outlier ratio']);       %  add axis labels and plot title
    ylabel(['y ' dataNames{i}]);
    title([ ' plot of ' dataNames{i} label ' versus error ratio']);
    legend(AlgNames);
    saveas(gcf,[curdirname '/param'   dataNames{i} nowtime '.fig']);
    saveas(gcf,[curdirname '/param_center_'   dataNames{i} nowtime '.jpg']);
    saveas(gcf,[curdirname '/param_center_' dataNames{i} nowtime '.eps'],'epsc');
    
    
    hold
    
end




for k=1:numalgs
   clear   cvsMatrix
    for j=1:repeat
        
        for i=1:numPoints
            cvsMatrix(j+1,i+1)=current_errors_samp_mean{i}(k,j);
            cvsMatrix(1,i+1)=n(1,i);
              cvsMatrix(j+1,1)=j;
        end
    end
    dlmwrite([curdirname '/datasamplers_'  AlgNames{1,k}  '.csv'], cvsMatrix);
    
end



fclose(fid);
fclose(fidgraph);
fclose(dispfid);
save( [curdirname '/variables_GP' nowtime '.mat']);

tElapsedprogram=toc(tStartprogram);
disp([' program took ' num2str(tElapsedprogram) ' seconds']);
copyfile('*.m',[curdirname '\matlabfiles']);
copyfile('robustfiles',[curdirname '\matlabfiles\robustfiles']);
copyfile('fundMatrix',[curdirname '\matlabfiles\fundMatrix']);
%copyfile(curdirname,['H:\matlabs\' curdirname ]);
end

%make a generic plotting script that plots the variance and mean and median
%of every measure

% write a proper mapsac and ransac function that uses a function to
% calculate the score and also doesnt normalize beforehand, also i should
% probably write a mlesac fucntion