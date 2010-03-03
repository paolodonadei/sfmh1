function [fcl, centerloc] = S2nonlinsolveEssRansac(TF,w,h)
%this function , given a camera center and a focal length and a fundamental
%matrix computes the error with respect to a fundamental matrix
%tic
if (nargin == 1)
    w=512;
    h=512;
end

% robust score function
%threshold=1.96*MADN;
%threshold=0.05; % dont knwo about this
threshold=findSVDthreshold(TF,w,h); % dont knwo about this

fcl=[0 0];

xcen=0;
ycen =0;
centerloc=[xcen ycen];

[m,sizeFs]=size(TF);

numtries=5;

solutions=cell(numtries,sizeFs);

x = PeterSturmSelfRobust( TF,w,h );



if(x(1,1)>200 && x(1,1)<1600)
    finit=x(1,1);
else
    finit=w+h;
end

xvari=w*0.050;
yvari=h*0.050;


for q=1:sizeFs


    clear focs xcentrs ycentrs scrs bestFfinal bestXfinal bestYfinal;
    [focs, xcentrs, ycentrs, cars, scrs, bestF, bestX, bestY,bestAR] = findBestsolsrepeatmore(numtries, {TF{q}}, w,h,[1],finit,w/2,h/2,30,xvari,yvari,1);



    for i=1:numtries
        solutions{i,q}=[focs(i,1) xcentrs(i,1) ycentrs(i,1) focs(i,1)*cars(i,1)];
    end

end

rawscores=zeros(sizeFs,sizeFs);
rawscoresmean=zeros(sizeFs,sizeFs);
scoresfinal=zeros(sizeFs,sizeFs);

for q=1:sizeFs
    for m=1:sizeFs
        clear tempVect;
        for i=1:numtries
            tempVect(i)=  computerEssentialErrorSVD( solutions{i,q},TF{m});
        end

        rawscores(m,q)=median(tempVect(i));
        rawscoresmean(m,q)=mean(tempVect(i));

    end
end


for q=1:sizeFs

    for m=1:sizeFs
        if(rawscores(q,m)<threshold)
            scoresfinal(q,m)=rawscores(q,m);
        else
            scoresfinal(q,m)=threshold;
        end
    end
end


[m,idx]=min(sum(scoresfinal));


count=1;

FFinal{count}=TF{idx};
WEIGHTS(count,1)=rawscores(idx,idx);
 %disp(['using frame ' num2str(idx)]);

for q=1:sizeFs
    if(rawscores(q,idx)<threshold && q~=idx)
        
  %        disp(['using frame ' num2str(q)]);
        count=count+1;
        WEIGHTS(count,1)=rawscores(q,idx);
        FFinal{count}=TF{q};
    end
end


clear focs xcentrs ycentrs scrs bestFfinal bestXfinal bestYfinal;


[focs, xcentrs, ycentrs,cars, scrs, bestF, bestX, bestY, bestAR] = findBestsolsrepeatmore(10, FFinal, w,h);


fcl=[ bestF bestF*bestAR];
centerloc=[bestX bestY];


end

