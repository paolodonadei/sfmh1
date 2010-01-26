function [fcl, centerloc] = S2nonlinsolveEsstwofram(TF,w,h)
%this function , given a camera center and a focal length and a fundamental
%matrix computes the error with respect to a fundamental matrix
%tic

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
numtries=200;
ffinals=zeros(numtries,sizeFs);
xfinals=zeros(numtries,sizeFs);
yfinals=zeros(numtries,sizeFs);
scorearray=zeros(numtries,sizeFs);

sturmfailed=0;

for q=1:sizeFs

    x = PeterSturmSelf( TF{q},w,h)

    if(x(1,1)>200 && x(1,1)<1600)
        finit=x(1,1);
    else
        sturmfailed=1;
    end


    xinit=w/2;
    yinit=h/2;


    fvari=70;
    xvari=70;
    yvari=70;



    bestf=0;
    bestx=0;
    besty=0;
    bestscore=1000000000000;
    curscore=0;
    f = @(x)computerEssentialErrorSVD(x,TF{q});
%    optionsfsolve  =optimset('Display','off','Jacobian','off','NonlEqnAlgorithm','lm','TolFun',1e-6,'TolX',1e-6); 
    optionsfsolve    =optimset('Display','off','Jacobian','off','Algorithm','levenberg-marquardt','TolFun',1e-6,'TolX',1e-6); 



    for i=1:numtries
        if(sturmfailed==0)
            x0=[ (randn()*fvari)+finit  (randn()*xvari)+xinit  (randn()*yvari)+yinit ];
        else
            x0=[ (rand()*(maxfocal))  (randn()*xvari)+xinit  (randn()*yvari)+yinit ];
        end



        [x,fval,exitflag,output]  = fsolve(f ,x0,optionsfsolve);

        if(x(1)<0 || x(1)>maxfocal || x(2)<0 || x(2)>w || x(3)<0 || x(3)>h)
            ffinals(i,q)=0;
            xfinals(i,q)=0;
            yfinals(i,q)=0;
            x
        else
            ffinals(i,q)=x(1);
            xfinals(i,q)=x(2);
            yfinals(i,q)=x(3);
        end;

        curscore=sum(abs(fval));

        [svScore, detScore, EssScore, EssScoreIA ]= EvalErrorParams1(TF{q},x(1),x(1),x(2),x(3),x(2),x(3) );
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

numclusts=10;
X=[ reshape(ffinals,sizeFs*numtries,1)  reshape(xfinals,sizeFs*numtries,1)   reshape(yfinals,sizeFs*numtries,1)];
[idx,ctrs] = kmeans(X,numclusts,'Replicates',5);

scatter3(X(:,1),X(:,2),X(:,3),100,idx,'filled');
title(['scatter plot of all the results found using my method']);
zlabel('x coordinates of optical centers');
ylabel('y coordinates of optical centers');
xlabel('value of focal length ');

maxnumclusts=0;
maxnumclustsF=0;
maxnumclustsX=0;
maxnumclustsY=0;

for i=1:numclusts
    cursize=sum(idx==i);

    if(cursize>maxnumclusts)
        maxnumclusts=cursize;
        maxnumclustsF=ctrs(i,1);
        maxnumclustsX=ctrs(i,2);
        maxnumclustsY=ctrs(i,3);
    end
end


figure

%

hist(reshape(ffinals,sizeFs*numtries,1),numtries/2);
title(['focal length']);
figure

hist(reshape(xfinals,sizeFs*numtries,1),numtries/2);
title(['xcomponent of camera center']);
figure

hist(reshape(yfinals,sizeFs*numtries,1),numtries/2);
title(['ycomponent of camera center']);
%

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

