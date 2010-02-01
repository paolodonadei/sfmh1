function [fcl, centerloc] = S2nonlinsolveoneframinNdiff(ps,w,h,pnum)
%this shows random sampling consensus
%this function , given a camera center and a focal length and a fundamental
%matrix computes the error with respect to a fundamental matrix
%tic
if (nargin == 1)
    w=512;
    h=512;
    pnum=1;
end
if (nargin == 3)
    pnum=1;
end


plotting=1;
%finding the f matrices
%always find the parameters of fgirst projection matrix

[m,n]=size(ps);
number=n;

if(pnum>number)
    disp(['p number is larger than the largest p number, this is incorrect']);
end

P1=ps{pnum};


count=0;
for i=1:number
    if(i~=pnum)
        count=count+1;
        TF{count}=vgg_F_from_P(P1,ps{1,i});
    end
end
count=0;



%TF=TF*10000;

maxfocal=2000;
fcl=[0 0];

xcen=0;
ycen =0;
centerloc=[xcen ycen];

[m,n]=size(TF);

sizeFs=n;

%f = @(x)computerEssentialErrorSquared(x,TF); %squared
%f = @(x)computerEssentialError(x,TF);
numtries=300;
ffinals=zeros(numtries,sizeFs);
xfinals=zeros(numtries,sizeFs);
yfinals=zeros(numtries,sizeFs);
scorearray=zeros(numtries,sizeFs);

hartleyfailed=0;

for q=1:sizeFs

    [ x ] = HartleySelf( TF{q},w,h );


    if(x(1,1)>200 && x(1,1)<1600 || x(1,2)>200 && x(1,2)<1600)
        finit1=x(1,1);
        finit2=x(1,2);
    else
        hartleyfailed=1;
    end


    xinit=w/2;
    yinit=h/2;


    fvari=110;
    xvari=70;
    yvari=70;



    bestf=0;
    bestx=0;
    besty=0;
    bestscore=1000000000000;
    curscore=0;
    f = @(x)computerEssentialErrorSVDDIFF(x,TF{q});
    optionsfsolve  =optimset('Display','off','Jacobian','off','NonlEqnAlgorithm','lm','TolFun',1e-6,'TolX',1e-6);
    % optionsfsolve    =optimset('Display','off','Jacobian','off','Algorithm','levenberg-marquardt','TolFun',1e-6,'TolX',1e-6);


%hartleyfailed=1;

    for i=1:numtries
        if(hartleyfailed==0)
            x0=[ (randn()*fvari)+finit1  (randn()*xvari)+xinit  (randn()*yvari)+yinit (randn()*fvari)+finit2  (randn()*xvari)+xinit  (randn()*yvari)+yinit ];
        else
            x0=[ (rand()*(maxfocal))  (randn()*xvari)+xinit  (randn()*yvari)+yinit (rand()*(maxfocal))  (randn()*xvari)+xinit  (randn()*yvari)+yinit ];

        end



        [x,fval,exitflag,output]  = fsolve(f ,x0,optionsfsolve);

        if(x(1)<0 || x(1)>maxfocal || x(2)<0 || x(2)>w || x(3)<0 || x(3)>h || x(4)<0 || x(4)>maxfocal || x(5)<0 || x(5)>w || x(6)<0 || x(6)>h)
            ffinals(i,q)=0;
            xfinals(i,q)=0;
            yfinals(i,q)=0;
            % x
        else
            ffinals(i,q)=x(1);
            xfinals(i,q)=x(2);
            yfinals(i,q)=x(3);

            ffinals_f2(i,q)=x(4);
            xfinals_f2(i,q)=x(5);
            yfinals_f2(i,q)=x(6);
        end

        curscore=sum(abs(fval));

        [svScore, detScore, EssScore, EssScoreIA ]= EvalErrorParams1(TF{q},x(1),x(4),x(2),x(3),x(5),x(6) );
        curscore=detScore;

        % disp(['iteration ' num2str(i) ' started from f= ' num2str(x0(1,1)) ' x= ' num2str(x0(1,2)) ' and y= ' num2str(x0(1,3))]);
        %disp(['fund matrix: ' num2str(q) ' iteration ' num2str(i) ' best f is ' num2str(x(1)) ' and best x = ' num2str(x(2)) ' and best y is ' num2str(x(3)) ' and score was ' num2str(curscore) ' det score was ' num2str(detScore) ' SV score was ' num2str(svScore) ' and ess score was ' num2str(EssScore) ' IA score is ' num2str( EssScoreIA)]);

        scorearray(i,q)=curscore;
        if(curscore<bestscore && imag(x(1))==0 && x(1)>200 && x(1)<1600 )
            bestscore=curscore;
            %  disp(['iteration ' num2str(i)]);
            bestf=x(1);
            bestx=x(2);
            besty=x(3);
            %   x,resnorm,fval,exitflag
            % disp(['fund matrix: ' num2str(q) '**BEST: iteration ' num2str(i) ' best f is ' num2str(x(1)) ' and best x = ' num2str(x(2)) ' and best y is ' num2str(x(3)) ' and score was ' num2str(curscore) ' det score was ' num2str(detScore) ' SV score was ' num2str(svScore) ' and ess score was ' num2str(EssScore) ' IA score is ' num2str( EssScoreIA)]);

        end
    end

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
[idx,ctrs] = kmeans(X,numclusts,'Replicates',5,'EmptyAction','drop');

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
for i=1:numclusts
    for q=1:sizeFs
        if( classscores(i,q)>ceil(numtries/numclusts))
            newval=ceil(numtries/numclusts)+log(classscores(i,q)-ceil(numtries/numclusts)+1);
            classscores(i,q)=newval;
        end

    end
end

for i=1:numclusts





    %im capping here, but maybe a smooth functio would be a better idea,
    %basically the idea here is that we prevent one F from overtaking the
    %cluster scores, cuz this way one bad F that gets stuck in a local
    %minima will generate teh maximum consensus, but hopefully there is a
    %more elegant way of doing this, like a smooth decaying exponential or
    %something





    % cursize=sum(idx==i);
    cursize=sum( classscores(i,:));

    if(cursize>maxnumclusts && ctrs(i,1) > 200 && ctrs(i,1) <maxfocal )
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

    figure
    hist(reshape(ffinals_f2,sizeFs*numtries,1),numtries/2);
    title(['focal length frame 2']);
    figure

    hist(reshape(xfinals_f2,sizeFs*numtries,1),numtries/2);
    title(['xcomponent of camera center frame 2']);
    figure

    hist(reshape(yfinals_f2,sizeFs*numtries,1),numtries/2);
    title(['ycomponent of camera center frame 2']);
    %
end

bestf=maxnumclustsF;
bestx=maxnumclustsX;
besty=maxnumclustsY;

display([' the highest cluster number was ' num2str(maxnumclusts) ' whose f is  ' num2str(maxnumclustsF) ' X is  ' num2str(maxnumclustsX) ' Y is  ' num2str( maxnumclustsY)  ]);


%toc
if(bestf>200 && bestf<1600 && imag(bestf)==0)
    bestf=bestf;
else
    bestf=w+h;
end
fcl=[ bestf bestf];
xcen=bestx;
ycen=besty;
centerloc=[xcen ycen];


end

