function [fcl, centerloc] = S2nonlinsolveEsstwofram(TF,w,h)
%this function , given a camera center and a focal length and a fundamental
%matrix computes the error with respect to a fundamental matrix
%tic
if (nargin == 1)
    w=512;
    h=512;
end
%TF=TF*10000;
plotting=0;
maxfocal=2000;
fcl=[0 0];

xcen=0;
ycen =0;
centerloc=[xcen ycen];

[m,sizeFs]=size(TF);



%f = @(x)computerEssentialErrorSquared(x,TF); %squared
%f = @(x)computerEssentialError(x,TF);
numtries=50;
ffinals=zeros(numtries,sizeFs);
xfinals=zeros(numtries,sizeFs);
yfinals=zeros(numtries,sizeFs);
scorearray=zeros(numtries,sizeFs);


xvari=50;
yvari=50;

maxfocal=2000;
minfocal=200;
x = PeterSturmSelfRobust( TF,w,h );
finit=x(1,1);
if(finit>minfocal && finit<maxfocal && imag(finit)==0)
    finit=finit;
    fvari=150;
else
    sturmfailed=1;
    fvari=(maxfocal-minfocal);
end
xinit=w/2;
yinit=h/2;

for q=1:sizeFs

    clear focs xcentrs ycentrs scrs bestFfinal bestXfinal bestYfinal;

    [focs, xcentrs, ycentrs, scrs, bestFfinal, bestXfinal, bestYfinal] = findBestsolsrepeat(numtries,{TF{q}}, w,h,ones(1,1),finit,w/2,h/2,fvari, xvari,yvari);



    ffinals(:,q)=focs;
    xfinals(:,q)= xcentrs;
    yfinals(:,q)=ycentrs;
    scorearray(:,q)=scrs;

end

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
if(plotting==1)

    for q=1:sizeFs
        sizearray=scorearray;
        scoremedian(q)=median(scorearray(:,q));
        for i=1:numtries
            if(sizearray(i,q)>scoremedian(q))
                sizearray(i,q)=1;
            else
                sizearray(i,q)=ceil(10+(sizearray(i,q)*(-9/scoremedian(q))));
            end
        end
    end


    figure
    hold
    title(['focal lengths versus energy']);
    xlabel('focal length');
    ylabel('value of the energy function ');

    for q=1:sizeFs
        scatter(ffinals(:,q), scorearray(:,q));
    end
    hold

    %%%%%%%%%%%%%%%%%%%%%
    styles={'r' ,'g', 'b', 'y', 'r' ,'c'};
    figure
    hold
    title(['all the local minima points scatter plot']);
    xlabel('x coordinates of optical centers');
    ylabel('y coordinates of optical centers');
    zlabel('value of focal length ');

    for q=1:sizeFs

        scatter3(xfinals(:,q),yfinals(:,q),ffinals(:,q), 3 ,styles{mod(q,6)+1})
    end
    hold

    %%%%%%%%%%%%%%%%%%%%%

    figure

end




%%%%%%%%%%5
numclusts=10;
X=[ reshape(ffinals,sizeFs*numtries,1)  reshape(xfinals,sizeFs*numtries,1)   reshape(yfinals,sizeFs*numtries,1)];
[idx,ctrs] = kmeans(X,numclusts,'Replicates',5,'EmptyAction','drop','display','off');

if(plotting==1)
    scatter3(X(:,1),X(:,2),X(:,3),100,idx,'filled');
    title(['scatter plot of all the results found using my method']);
    zlabel('x coordinates of optical centers');
    ylabel('y coordinates of optical centers');
    xlabel('value of focal length ');
end

maxnumclusts=0;
maxnumclustsF=0;
maxnumclustsX=0;
maxnumclustsY=0;


%%%%%%%%%%%
%creating an index of the memberships
mcount=1;
for q=1:sizeFs
    for i=1:numtries
        % some ad hoc limits
        if(ffinals(i,q)<50 || ffinals(i,q)>maxfocal || xfinals(i,q)<10 || xfinals(i,q)>=w || yfinals(i,q)<10 || yfinals(i,q)>h)
            idx_membership(mcount)=0;
        else
            idx_membership(mcount)=q;
        end

        mcount=mcount+1;
    end
end


classscores=zeros(numclusts,sizeFs);

for j=1:(sizeFs*numtries)
    if(idx_membership(j)~=0)
        classscores(idx(j),idx_membership(j))=classscores(idx(j),idx_membership(j))+1;
    end
end

%  uncomment these bro, and your class scores thing is not working
%ctrs

%classscores

for i=1:numclusts
    for q=1:sizeFs
        %         if( classscores(i,q)>ceil(numtries/numclusts))
        %             newval=ceil(numtries/numclusts)+log(classscores(i,q)-ceil(numtries/numclusts)+1);
        %             classscores(i,q)=newval;
        %         end

        %         if( classscores(i,q)>0)
        %             classscores(i,q)=log( classscores(i,q));
        %         end
        classscores(i,q)=log((1+classscores(i,q)^2));

    end
end

%classscores


for i=1:numclusts





    %im capping here, but maybe a smooth functio would be a better idea,
    %basically the idea here is that we prevent one F from overtaking the
    %cluster scores, cuz this way one bad F that gets stuck in a local
    %minima will generate teh maximum consensus, but hopefully there is a
    %more elegant way of doing this, like a smooth decaying exponential or
    %something





    % cursize=sum(idx==i);
    cursize=sum( classscores(i,:));

    if(cursize>maxnumclusts)
        maxnumclusts=cursize;
        maxnumclustsF=ctrs(i,1);
        maxnumclustsX=ctrs(i,2);
        maxnumclustsY=ctrs(i,3);
    end
end



%
if(plotting==1)

    figure
    hist(reshape(ffinals,sizeFs*numtries,1),numtries/2);
    title(['focal length']);
    figure

    hist(reshape(xfinals,sizeFs*numtries,1),numtries/2);
    title(['xcomponent of camera center']);
    figure

    hist(reshape(yfinals,sizeFs*numtries,1),numtries/2);
    title(['ycomponent of camera center']);
    %
end

bestf=maxnumclustsF;
bestx=maxnumclustsX;
besty=maxnumclustsY;

%display([' the highest cluster number was ' num2str(maxnumclusts) ' whose f is  ' num2str(maxnumclustsF) ' X is  ' num2str(maxnumclustsX) ' Y is  ' num2str( maxnumclustsY)  ]);


%toc
if(bestf>200 && bestf<1600 && imag(bestf)==0 && isnan(bestf)==0)
    bestf=bestf;
else
    bestf=w+h;
end

if(bestx<w && bestx>0 && imag(bestx)==0 && isnan(bestx)==0)
    bestx=bestx;
else
    bestx=w/2;
end

if(besty<w && besty>0 && imag(besty)==0 && isnan(besty)==0)
    besty=besty;
else
    besty=h/2;
end

fcl=[ bestf bestf];
centerloc=[bestx  besty];


end

