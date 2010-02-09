function [fcl, centerloc] =S2nonlinsolveEssNframestimator(TF,w,h)
%this function , given a camera center and a focal length and a series of fundamental
%matrices computes the error with respect to a fundamental matrix
%tic
if (nargin == 1)
    w=512;
    h=512;
end
%TF=TF*10000;
plotting=0;
fcl=[0 0];
maxfocal=2000;


xcen=0;
ycen =0;
centerloc=[xcen ycen];
[m,numFs]=size(TF);


WEIGHTS=ones(numFs,1);

count=0;


for i=1:5


    [focs, xcentrs, ycentrs, scrs, bestF, bestX, bestY] = findBestsolsrepeat(10, TF, w,h,WEIGHTS );

    x=[bestF bestX  bestY  ];
    erFs=zeros(numFs,1);
    for j=1:numFs
        erFs(j,1) = computerEssentialErrorSVD(x,TF{j});
       
    end

    curMedian=median(erFs);
    MADN=median(abs(erFs-curMedian))/0.6745;

    threshold=0.1; % i dont know if this is the best thing to do

    for j=1:numFs
        %         if(erFs(j,1) <threshold)
        %             WEIGHTS(j,1)=1;
        %         elseif(erFs(j,1) >=threshold && erFs(j,1) <(3*threshold))
        %             WEIGHTS(j,1)=1/erFs(j,1) ;
        %         else
        %             WEIGHTS(j,1)=0;
        %         end
        if(erFs(j,1) <threshold)
            WEIGHTS(j,1)=(1-((erFs(j,1)/threshold)^2))^2;
        elseif(erFs(j,1) >=threshold )
            WEIGHTS(j,1)=0 ;
        else
            disp(['what happened']);
        end
    end
    % DISTS
    WEIGHTS;
    %     %   put the robust weighing here
    x
end

fcl=[ bestF bestF];
xcen=bestX;
ycen=bestY;
centerloc=[xcen ycen];

end

