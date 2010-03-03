function [ t,means_F,medians_F,  variances_F ,means_XY,medians_XY,  variances_XY ] =     generateSelfPlot(numPs, paramcheck,repeat,pfdiff,pskew,par,pcenterdev,noiselevel, numbadFs, startp, endp )

%paramcheck would be the parmameter we are varying in characters, so
%'c' for camera center
%repeat is for how many times we will try this
%rest is the constant camera params

%change this
width=512;
height =512;


if (nargin == 9)
    startp=0;
    endp=1;
end


savefoldername='datafilesd';
direxists=1;
dircount=0;

while(direxists==1)

    dircount=dircount+1;

    curdirname=[savefoldername '/subdir' num2str(dircount)];

    if(exist(curdirname,'dir')==0)
        mkdir(curdirname);
        direxists=0;
    end

end

if(paramcheck=='b' )
    numPoints=(numPs*(numPs-1))/2;
else
    %this is an important parameter, change it maybe
    numPoints=10;
end




styles={'-.or' ,'-.xg', '-.+b', '-.*y', '-.vr' ,'-..c'};
t=zeros(1,numPoints);
label='empty';
nowtime=num2str(sum(round(100*clock)));
%Algs
fid = fopen([curdirname '/exp' nowtime '.txt'], 'w');
fidgraph = fopen([curdirname '/graphdata' nowtime '.txt'], 'w');
dispfid = fopen([curdirname '/dispcommands' nowtime '.txt'], 'w');

AlgNames={ 'Un-robust','Case Deletion', 'M-estimator', 'RANSAC','TWOFRAM'};
AlgFuncs={ @S2nonlinsolveEssNfram ,@S2nonlinsolveEssNframdiagnostics, @S2nonlinsolveEssNframestimator ,  @S2nonlinsolveEssRansac,@S2nonlinsolveEsstwofram};


numalgs=size(AlgFuncs,2);

%outputs
means_F=zeros(numalgs,numPoints);
medians_F=zeros(numalgs,numPoints);
variances_F=zeros(numalgs,numPoints);

means_XY=zeros(numalgs,numPoints);
medians_XY=zeros(numalgs,numPoints);
variances_XY=zeros(numalgs,numPoints);

numBadPoints=zeros(numalgs,numPoints);

%arg paramters

n=ones(1,numPoints)*noiselevel;
fdiff=ones(1,numPoints)*pfdiff;
skew=ones(1,numPoints)*pskew;
aspect=ones(1,numPoints)*par;
centerdev=(ones(1,numPoints)*pcenterdev); %+(randn(1,numPoints)*5); %gaussian noise, what what

b=ones(1,numPoints)*numbadFs;

%depending on what we are varying we are gonna change the parameters
if(paramcheck=='n')
    step=0.9/numPoints;
    n=0:step:0.9;  %continue from here and find out why your method sucks

    t=n(1,1:numPoints);
    label='noise-level';
end

if(paramcheck=='b')
    step=((numPs*(numPs-1))/2)/numPoints;
    b=floor(0:step:((numPs*(numPs-1))/2));  %continue from here and find out why your method sucks
    t=b(1,1:numPoints);

    label='number-bad-F';
end


if(paramcheck=='s')

    skew=0:0.1:10;
    t=skew(1,1:numPoints);
    label='skew';
end

if(paramcheck=='a')
    aspect=0.8:0.02:1.2;
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

%allSolutions=cell(numPoints,repeat,numalgs);


fprintf(fidgraph, ' , ');
for k=1:numalgs
    fprintf(fidgraph, [  AlgNames{1,k} '_meanF , ' AlgNames{1,k} '_medianF , ' AlgNames{1,k} '_meanXY , ' AlgNames{1,k} '_medianXY , ' AlgNames{1,k} '_bnadPTS , ']);
end
fprintf(fidgraph, '\n');


startloc=(startp*numPoints)+1;
endloc=(endp*numPoints);

for i=startloc:endloc

    clear  current_errors_F current_errors_XY current_BADPTS ;
    current_errors_F=zeros(numalgs,repeat);
    current_errors_XY=zeros(numalgs,repeat);
    current_BADPTS=zeros(numalgs,repeat);

    for j=1:repeat
        tStart=tic;
        currIteration=currIteration+1;

        clear F ks totalAgltime;
        totalAgltime=0;

        % [ F, ks ] = generateF( fdiff(1,i), skew(1,i), aspect(1,i),centerdev(1,i),1,numPs,n(1,i),b(1,i)   );
        %     [ F, ks ] =  generateFangl( fdiff(1,i), skew(1,i), aspect(1,i),centerdev(1,i),1,numPs,n(1,i),b(1,i)   );
        %   [corrs, IMS, P,ks, F] = readCorrsOxford('/home/houman/work/test_data/wadhamcollege', n(1,i), b(1,i));

        [corrs, IMS, P,ks, F] = readCorrsOxford('C:\Documents and Settings\hrast019\Desktop\data\euclidean\build1', n(1,i), b(1,i));
        width=1024; height=768;

        disp(['****iteration ' num2str(currIteration) ' out of ' num2str(numTotalIterations) '   AND calling generateF( ' num2str(fdiff(1,i)) ' , ' num2str(skew(1,i)) ' , '  num2str(aspect(1,i)) ' , ' num2str(centerdev(1,i)) ' , 1 , ' num2str(numPs) ' , ' num2str(n(1,i)) ' , ' num2str(b(1,i)) ')'] );
        fprintf(dispfid,['\n****iteration ' num2str(currIteration) ' out of ' num2str(numTotalIterations) '   AND calling generateF( ' num2str(fdiff(1,i)) ' , ' num2str(skew(1,i)) ' , '  num2str(aspect(1,i)) ' , ' num2str(centerdev(1,i)) ' , 1 , ' num2str(numPs) ' , ' num2str(n(1,i)) ' , ' num2str(b(1,i)) ')']);
        for k=1:numalgs

            tic;
            [answerf, loca]=AlgFuncs{k}(F,width,height); %assuming camera size is 512x512
            PtElapsed=toc;
            totalAgltime=totalAgltime+PtElapsed;

            [current_errors_F(k,j), current_errors_XY(k,j)  ] = calcSelfCalibError(answerf, loca,ks);


            disp(['algorithm: ' AlgNames{k} ' had error in F ' num2str(current_errors_F(k,j)) ' and error xy: ' num2str(current_errors_XY(k,j)) ' time: ' num2str(PtElapsed)]);
            fprintf(dispfid,['\nalgorithm: ' AlgNames{k} ' had error in F ' num2str(current_errors_F(k,j)) ' and error xy: ' num2str(current_errors_XY(k,j)) ' time: ' num2str(PtElapsed)]);
            if(abs(current_errors_F(k,j))>50)
                current_BADPTS(k,j)=current_BADPTS(k,j)+1;
            end


            %  allSolutions{i,j,k}=[answerf loca current_errors_F(k,j) current_errors_XY(k,j)];
            fprintf(fid, 'algorithm %s correct answers: %6.2f and %6.2f obtained answers %6.2f and %6.2f error: %6.2f AND true X=%6.2f and true Y=%6.2f and estimated X=%6.2f and true Y=%6.2f with error %6.2f and time %6.2f\n',AlgNames{k},ks{1}(1,1),ks{2}(1,1),answerf(1,1),answerf(1,2),current_errors_F(k,j),ks{1}(1,3),ks{1}(2,3),loca(1,1),loca(1,2),current_errors_XY(k,j),PtElapsed  );

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
        means_F(k,i)=mean(current_errors_F(k,:));
        medians_F(k,i)=median(current_errors_F(k,:));
        variances_F(k,i)=var(current_errors_F(k,:));

        means_XY(k,i)=mean(current_errors_XY(k,:));
        medians_XY(k,i)=median(current_errors_XY(k,:));
        variances_XY(k,i)=var(current_errors_XY(k,:));

        numBadPoints(k,i)=mean(current_BADPTS(k,:));

        fprintf(fidgraph, ' %6.2f , %6.2f  , %6.2f ,  %6.2f , %6.2f ,' ,means_F(k,i), medians_F(k,i), means_XY(k,i),medians_XY(k,i),numBadPoints(k,i));
    end

    fprintf(fidgraph, ' \n');


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

    %  saveas(gcf,['param' paramcheck '_' dataNames{i} nowtime '.eps']);
    saveas(gcf,[curdirname '/param_focal_' paramcheck '_' dataNames{i} nowtime '.jpg']);
    saveas(gcf,[curdirname '/param_focal_' paramcheck '_' dataNames{i} nowtime '.fig']);
    saveas(gcf,[curdirname '/param_focal_' paramcheck '_' dataNames{i} nowtime '.eps'],'epsc');
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
    saveas(gcf,[curdirname '/param' paramcheck '_' dataNames{i} nowtime '.fig']);
    saveas(gcf,[curdirname '/param_center_' paramcheck '_' dataNames{i} nowtime '.jpg']);
    saveas(gcf,[curdirname '/param_center_' paramcheck '_' dataNames{i} nowtime '.eps'],'epsc');


    hold

end



%%%%%%%%%%%%%%%%%%%%%%%% num bad points

figure;
hold;
for k=1:numalgs

    plot(t,numBadPoints(k,:),styles{k});
end
xlabel(['x (' label ')']);       %  add axis labels and plot title
ylabel('number of bad solutions');
title(['plot of bad points versus parameter ']);
legend(AlgNames);
hold
%%%%%%%%%%%%%%%%%%%%%%%%%%%%


saveas(gcf,[curdirname '/BADPOINTS_' paramcheck '_'  nowtime '.fig']);
saveas(gcf,[curdirname '/BADPOINTS_' paramcheck '_'  nowtime '.jpg']);
saveas(gcf,[curdirname '/BADPOINTS_' paramcheck '_'  nowtime '.eps'],'epsc');

fclose(fid);
fclose(fidgraph);
fclose(dispfid);
save( [curdirname '/variables_GP' nowtime '.mat'])

end