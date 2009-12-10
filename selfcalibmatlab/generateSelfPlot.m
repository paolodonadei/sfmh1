function [ t,means,medians,  variances  ] = generateSelfPlot(paramcheck,repeat,pfdiff,pskew,par,pcenterdev )

fid = fopen('exp.txt', 'w');


numPoints=30;
styles={'-.or' ,'-.xg', '-.+b', '-.*y', '-.vr' ,'-..c'};
t=zeros(1,numPoints);
label='empty';
nowtime=num2str(sum(round(100*clock)));
%Algs

AlgNames={ 'Houman2Fr','Houman2FrPINF','Pollefey2Frame'};
AlgFuncs={ @HoumanminimalTwoFrameSolver ,@HoumanminimalTwoFrameSolverpinf,@PollefeyVisualwithPOLDTWOFRAMEFAM};


numalgs=size(AlgFuncs,2);

%outputs
means=zeros(numalgs,numPoints);
medians=zeros(numalgs,numPoints);
variances=zeros(numalgs,numPoints);


%arg paramters
fdiff=ones(1,numPoints)*pfdiff;
skew=ones(1,numPoints)*pskew;
aspect=ones(1,numPoints)*par;
centerdev=ones(1,numPoints)*pcenterdev;


%depending on what we are varying we are gonna change the parameters
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
    centerdev=0:2:200;
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
    
    current_errors=zeros(numalgs,repeat);
    
    for j=1:repeat
        currIteration=currIteration+1;
        [ F, ks ] = generateF( fdiff(1,i), skew(1,i), aspect(1,i),centerdev(1,i),1 );
        
        disp(['****iteration ' num2str(currIteration) ' out of ' num2str(numTotalIterations) '   AND calling generateF( ' num2str(fdiff(1,i)) ' , ' num2str(skew(1,i)) ' , '  num2str(aspect(1,i)) ' , ' num2str(centerdev(1,i)) ')'] );
        
        for k=1:numalgs
            answer=AlgFuncs{k}(F,512,512);
            current_errors(k,j)=calcSelfCalibError(answer,ks);
            disp(['algorithm: ' AlgNames{k} ' had error ' num2str(current_errors(k,j))]);
            fprintf(fid, 'algorithm %s correct answers: %6.2f and %6.2f obtained answers %6.2f and %6.2f error: %6.2f\n',AlgNames{k},ks{1}(1,1),ks{2}(1,1),answer(1,1),answer(1,2),current_errors(k,j)  );

        end
        
    end
    %now calculate the stat for the current run
    for k=1:numalgs
        means(k,i)=mean(current_errors(k,:));
        medians(k,i)=median(current_errors(k,:));
        variances(k,i)=var(current_errors(k,:));
        
    end
    
    
    
    
end

data={means, medians, variances};
dataNames={'means', 'medians', 'variances'};
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
    saveas(gcf,['param_' paramcheck '_' dataNames{i} nowtime '.jpg']);
    hold
    
end


fclose(fid);

end