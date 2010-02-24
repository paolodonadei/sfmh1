function [fcl, centerloc] = S2nonlinsolveEssNfram(TF,w,h)
%this function , given a camera center and a focal length and a series of fundamental
%matrices computes the error with respect to a fundamental matrix
%tic
if (nargin == 1)
    w=512;
    h=512;
end
%TF=TF*10000;
plotting=0;
numtries=10;
 clear focs xcentrs ycentrs scrs bestFfinal bestXfinal bestYfinal;
 [focs, xcentrs, ycentrs, ars,scrs, bestFfinal, bestXfinal, bestYfinal,bestARfinal] =  findBestsolsrepeatmore(numtries, TF, w,h);
scorearray=scrs;
ffinals=focs;
xfinals=xcentrs;
yfinals=ycentrs;

bestf=bestFfinal;
bestx= bestXfinal;
besty= bestYfinal;
bestar=bestARfinal;
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
if(plotting==1)
    sizearray=scorearray;
    scoremedian=median(scorearray);
    for i=1:numtries
        if(sizearray(i,1)>scoremedian)
            sizearray(i,1)=1;
        else
            sizearray(i,1)=ceil(10+(sizearray(i,1)*(-9/scoremedian)));
        end
    end


    scatter(ffinals, scorearray);
    title(['focal lengths versus energy']);
    xlabel('focal length');
    ylabel('value of the energy function ');

    figure;

    scatter(ffinals, log(scorearray));
    title(['log scale focal lengths versus energy']);
    xlabel('focal length');
    ylabel('value of the energy function ');

    figure;


    scatter3(xfinals,yfinals,ffinals, 3 , 'r')
    title(['all the local minima points scatter plot']);
    xlabel('x coordinates of optical centers');
    ylabel('y coordinates of optical centers');
    zlabel('value of focal length ');
    figure

    plot3(xfinals,yfinals,ffinals,'r.','MarkerSize',12)
    title(['all the local minima points']);
    xlabel('x coordinates of optical centers');
    ylabel('y coordinates of optical centers');
    zlabel('value of focal length ');
    figure

    %
    % numclusts=10;
    % X=[ ffinals  xfinals   yfinals];
    % [idx,ctrs] = kmeans(X,numclusts,'Replicates',5);
    %
    % scatter3(X(:,1),X(:,2),X(:,3),100,idx,'filled');
    % title(['scatter plot of all the results found using my method and those found by sweeping the peter sturm algorithm']);
    % hold
    % count=0;
    % for i=230:350
    % for j=230:350
    %     count=count+1;
    % [ x, centerloc ] = PeterSturmSelfmoreparams( TF, i, j );
    % XF(count,1)=x(1,1);
    % XF(count,2)=i;
    % XF(count,3)=j;
    % end
    % end
    % scatter3(XF(:,1),XF(:,2),XF(:,3),100,'r','filled');
    % hold
    %
    %
    % maxnumclusts=0;
    % maxnumclustsF=0;
    % maxnumclustsX=0;
    % maxnumclustsY=0;
    %
    % for i=1:numclusts
    %     cursize=sum(idx==i);
    %
    %     if(cursize>maxnumclusts)
    %         maxnumclusts=cursize;
    %         maxnumclustsF=ctrs(i,1);
    %         maxnumclustsX=ctrs(i,2);
    %         maxnumclustsY=ctrs(i,3);
    %     end
    % end
    %
    %
    % figure
    %
    %
    hist(ffinals,numtries/2);
    title(['focal length']);
    figure

    hist(xfinals,numtries/2);
    title(['xcomponent of camera center']);
    figure

    hist(yfinals,numtries/2);
    title(['ycomponent of camera center']);



end
%
% display(['mean of f was ' num2str(mean(ffinals)) ' median of f was ' num2str(median(ffinals)) ' variance of f was ' num2str(var(ffinals))]);
% display(['mean of x was ' num2str(mean(xfinals)) ' median of x was ' num2str(median(xfinals)) ' variance of x was ' num2str(var(xfinals))]);
% display(['mean of y was ' num2str(mean(yfinals)) ' median of y was ' num2str(median(yfinals)) ' variance of y was ' num2str(var(yfinals))]);
% display(['best f is ' num2str(bestf) ' and best x is ' num2str(bestx) ' and best y is ' num2str(besty)]);
% display([' the highest cluster number was ' num2str(maxnumclusts) ' whose f is  ' num2str(maxnumclustsF) ' X is  ' num2str(maxnumclustsX) ' Y is  ' num2str( maxnumclustsY)  ]);



%toc



fcl=[ bestf bestf*bestar];
centerloc=[bestx besty];



end

