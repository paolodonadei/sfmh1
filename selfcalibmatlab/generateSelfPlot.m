function [ t,means_F,medians_F,  variances_F ,means_xy,medians_xy,  variances_xy ] =     generateSelfPlot(numFs, paramcheck,repeat,pfdiff,pskew,par,pcenterdev,noiselevel, numbadFs )

%paramcheck would be the parmameter we are varying in characters, so
%'c' for camera center
%repeat is for how many times we will try this
%rest is the constant camera params

fid = fopen('exp.txt', 'w');


numPoints=30;
styles={'-.or' ,'-.xg', '-.+b', '-.*y', '-.vr' ,'-..c'};
t=zeros(1,numPoints);
label='empty';
nowtime=num2str(sum(round(100*clock)));
%Algs

AlgNames={ 'Un-robust','Hough Transform','Case Deletion', 'M-estimator','Peter Sturm Median', 'RANSAC'};
AlgFuncs={ @S2nonlinsolveEssNfram ,@S2nonlinsolveEsstwofram,@S2nonlinsolveEssNframdiagnostics, @S2nonlinsolveEssNframestimator , @PeterSturmSelfRobust , @S2nonlinsolveEssRansac};


numalgs=size(AlgFuncs,2);

%outputs
means_F=zeros(numalgs,numPoints);
medians_F=zeros(numalgs,numPoints);
variances_F=zeros(numalgs,numPoints);

means_xy=zeros(numalgs,numPoints);
medians_xy=zeros(numalgs,numPoints);
variances_xy=zeros(numalgs,numPoints);

%arg paramters

noiselevels=ones(1,numPoints)*noiselevel;
fdiff=ones(1,numPoints)*pfdiff;
skew=ones(1,numPoints)*pskew;
aspect=ones(1,numPoints)*par;
centerdev=(ones(1,numPoints)*pcenterdev); %+(randn(1,numPoints)*5); %gaussian noise, what what
n=zeros(1,numPoints);
b=ones(1,numPoints)*numbadFs;

%depending on what we are varying we are gonna change the parameters
if(paramcheck=='n')
    step=1/numPoints;
    n=0:step:1;  %continue from here and find out why your method sucks
    n=n(1,1:numPoints);
    label='noise-level';
end

if(paramcheck=='b')
    step=numFs/numPoints;
    b=0:step:numFs;  %continue from here and find out why your method sucks
    b=floor(b(1,1:numPoints));
    label='number-bad-F';
end


if(paramcheck=='s')
    skew=0:0.1:10;
    t=skew(1,1:numPoints);
    label='skew';
end

if(paramcheck=='a')
    aspect=1:0.01:2;
    t=aspect(1,1:numPoints);
    label='aspect ratio';
end


if(paramcheck=='c')
    centerdev=0:3:300;
    t=centerdev(1,1:numPoints);
    label='deviation from camera center';
end


if(paramcheck=='f')
    fdiff=0:2:200;
    t=fdiff(1,1:numPoints);
    label='focal length change';
end


%loop for creating the data by calling the genF function togenerate random
%Fs and then using the self calib methods to find the focal lengths from
%these methods
numTotalIterations=numPoints*repeat;
currIteration=0;

for i=1:numPoints

    current_errors_F=zeros(numalgs,repeat);
    current_errors_XY=zeros(numalgs,repeat);

    for j=1:repeat
        currIteration=currIteration+1;
        [ F, ks ] = generateF( fdiff(1,i), skew(1,i), aspect(1,i),centerdev(1,i),1,numFs,n(1,i),b(1,i)   );

        disp(['****iteration ' num2str(currIteration) ' out of ' num2str(numTotalIterations) '   AND calling generateF( ' num2str(fdiff(1,i)) ' , ' num2str(skew(1,i)) ' , '  num2str(aspect(1,i)) ' , ' num2str(centerdev(1,i)) ' , 1 , ' num2str(numFs) ' , ' num2str(n(1,i)) ' , ' num2str(b(1,i)) ')'] );

        for k=1:numalgs
      
            [answerf, loca]=AlgFuncs{k}(F); %assuming camera size is 512x512
            current_errors_F(k,j)=calcSelfCalibError(answerf,ks);
            current_errors_XY(k,j)=sqrt(((loca(1,1)-ks{1}(1,3))^2)+((loca(1,2)-ks{1}(2,3))^2));
            disp(['algorithm: ' AlgNames{k} ' had error in F ' num2str(current_errors_F(k,j)) ' and error xy' num2str(current_errors_XY(k,j))]);

            fprintf(fid, 'algorithm %s correct answers: %6.2f and %6.2f obtained answers %6.2f and %6.2f error: %6.2f AND true X=%6.2f and true Y=%6.2f and estimated X=%6.2f and true Y=%6.2f with error %6.2f\n',AlgNames{k},ks{1}(1,1),ks{2}(1,1),answerf(1,1),answerf(1,2),current_errors_F(k,j),ks{1}(1,3),ks{1}(2,3),loca(1,1),loca(1,2),current_errors_XY(k,j)  );

        end

    end
    %now calculate the stat for the current run
    for k=1:numalgs
        means_F(k,i)=mean(current_errors_F(k,:));
        medians_F(k,i)=median(current_errors_F(k,:));
        variances_F(k,i)=var(current_errors_F(k,:));

        means_XY(k,i)=mean(current_errors_XY(k,:));
        medians_XY(k,i)=median(current_errors_XY(k,:));
        variances_XY(k,i)=var(current_errors_XY(k,:));
    end




end

% for focal length
data={means_F, medians_F, variances_F};
dataNames={'means_F', 'medians_F', 'variances_F'};
sizeDataCats=size(data,2);

for i=1:sizeDataCats
    % the mean stuff
    figure;
    hold;
    for k=1:numalgs

        plot(t,data{i}(k,:),styles{k});
    end
    xlabel(['x (' label ')']);       %  add axis labels and plot title
    ylabel('y (error in focal length in pixels)');
    title([dataNames{i} ' plot of ' label ' versus error in focal length estimation']);
    legend(AlgNames);
    %  saveas(gcf,['param' paramcheck '_' dataNames{i} nowtime '.fig']);
    %  saveas(gcf,['param' paramcheck '_' dataNames{i} nowtime '.eps']);
    saveas(gcf,['param_focal_' paramcheck '_' dataNames{i} nowtime '.jpg']);
    saveas(gcf,['param_focal_' paramcheck '_' dataNames{i} nowtime '.eps'],'epsc');
    hold

end

% for camera center
data={means_XY, medians_XY, variances_XY};
dataNames={'means_XY', 'medians_XY', 'variances_XY'};
sizeDataCats=size(data,2);

for i=1:sizeDataCats
    % the mean stuff
    figure;
    hold;
    for k=1:numalgs

        plot(t,data{i}(k,:),styles{k});
    end
    xlabel(['x (' label ')']);       %  add axis labels and plot title
    ylabel('y (error in camera center in pixels)');
    title([dataNames{i} ' plot of ' label ' versus error in camera center estimation']);
    legend(AlgNames);
    %  saveas(gcf,['param' paramcheck '_' dataNames{i} nowtime '.fig']);
    %  saveas(gcf,['param' paramcheck '_' dataNames{i} nowtime '.eps']);
    saveas(gcf,['param_center_' paramcheck '_' dataNames{i} nowtime '.jpg']);
    saveas(gcf,['param_center_' paramcheck '_' dataNames{i} nowtime '.eps'],'epsc');


    hold

end

save( ['variables_GP' nowtime '.mat'])

fclose(fid);

end