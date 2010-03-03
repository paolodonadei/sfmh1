function [fcl, centerloc] =S2nonlinsolveEssNframestimator(TF,w,h)
%this function , given a camera center and a focal length and a series of fundamental
%matrices computes the error with respect to a fundamental matrix
%tic
%threshold=0.05; % i dont know if this is the best thing to do
threshold=findSVDthreshold(TF,w,h); 
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
maxiter=25;
maxgooditer=7;
allcounter=0;
goodcounter=0;

x=[ 0 0 0 0];
while(allcounter<maxiter && goodcounter<maxgooditer)
    allcounter=allcounter+1;

    clear focs xcentrs ycentrs scrs bestFfinal bestXfinal bestYfinal bestAR;
    [focs, xcentrs, ycentrs,  cars, scrs, bestF, bestX, bestY, bestAR] = findBestsolsrepeatmore(3, TF, w,h,WEIGHTS );





    x=[bestF bestX  bestY bestAR*bestF ];


    if(sum(x)<eps)
        x=[w w/2 h/2 w ];
    end


    erFs=zeros(numFs,1);
    for j=1:numFs
        erFs(j,1) = computerEssentialErrorSVD(x,TF{j});

    end

   

    %   disp(['good points']);
    goodcounter=goodcounter+1;
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
WEIGHTS
x

    % x
    % sum(erFs)


    if(sum(WEIGHTS<0))
        disp('negative element in the weights function');
        break;
    end

    if(sum(WEIGHTS)<eps)
        break;
    end


end

fcl=[ bestF bestF*bestAR];
centerloc=[bestX bestY];

end

