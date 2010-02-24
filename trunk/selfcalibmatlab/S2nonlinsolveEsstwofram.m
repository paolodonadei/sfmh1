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



numtries=150;


ffinals=zeros(numtries,sizeFs);
xfinals=zeros(numtries,sizeFs);
yfinals=zeros(numtries,sizeFs);
arfinals=zeros(numtries,sizeFs);
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

    [focs, xcentrs, ycentrs, ars,scrs, bestFfinal, bestXfinal, bestYfinal,bestAR] =  findBestsolsrepeatmore(numtries,{TF{q}}, w,h,ones(1,1),finit,w/2,h/2,fvari, xvari,yvari,1);

    %bestFfinal, bestXfinal, bestYfinal,bestAR

    ffinals(:,q)=focs;
    xfinals(:,q)= xcentrs;
    yfinals(:,q)=ycentrs;
    arfinals(:,q)=ars;
    scorearray(:,q)=scrs;

end

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%



%%%%%%%%%%5
numclusts=15;
X=[ reshape(ffinals,sizeFs*numtries,1)  reshape(arfinals,sizeFs*numtries,1)  ];

if(plotting==1)

    Cl = repmat(1:sizeFs,numtries,1);

    scatter3(ffinals(:),xfinals(:),yfinals(:),10,Cl(:),'filled');
    title('graph of focal lengths and optical axis, color coded based on individual F votes');
    zlabel('y coordinates of optical centers');
    ylabel('x coordinates of optical centers');
    xlabel('value of focal length ');
    figure

    scatter(ffinals(:),arfinals(:),10,Cl(:),'filled');
    title('graph of focal lengths and aspect ratios, color coded based on individual F votes');
    ylabel('aspect ratio');
    xlabel('value of focal length ');
    figure
end




XComplete=[ reshape(ffinals,sizeFs*numtries,1)  reshape(xfinals,sizeFs*numtries,1) reshape(yfinals,sizeFs*numtries,1) reshape(arfinals,sizeFs*numtries,1)  ];
[idx,ctrs,sumd] = kmeans(XComplete,numclusts,'Replicates',5,'emptyaction','drop','display','off');






%%%%%%%%%%%
%creating an index of the memberships
mcount=1;

for q=1:sizeFs
    for i=1:numtries
        if(validSLFSolution(XComplete(mcount,1),XComplete(mcount,2),XComplete(mcount,3),XComplete(mcount,4),w,h)~=1)
            idx_membership(mcount)=0;
        else
            idx_membership(mcount)=q;
        end

        mcount=mcount+1;
    end
end

classscores=zeros(numclusts,sizeFs);

%this is a dummy variable helping to debug this thing, the columns contain
%some attributes of the classes , i wont need this in the production
%version
objscores=zeros(numclusts,3);


for j=1:(sizeFs*numtries)
    if(idx_membership(j)~=0)
        classscores(idx(j),idx_membership(j))=classscores(idx(j),idx_membership(j))+1;
        objscores(idx(j),1)=objscores(idx(j),1)+ computerEssentialErrorSVDNFramesWeighted(XComplete(j,:),TF);
        objscores(idx(j),2)=objscores(idx(j),2)+1;
    end
end

for i=1:numclusts
    if(objscores(i,2)==0)
        objscores(i,3)=10;
    else
        objscores(i,3)=objscores(i,1)/objscores(i,2);
    end

end
objscores(:,4)=sumd;
for i=1:numclusts
    if(objscores(i,2)==0)
        objscores(i,4)=1000000;
    else
        objscores(i,4)=objscores(i,4)/objscores(i,2);
    end
end



%objscore column
%1: robust score     2: number of elements per cluster    3: value of
%objective function     4:average value of the distance of points to the
%centroid




classscores_rbst=log((1+classscores.^2));

objscores(:,1)=sum(classscores_rbst,2);

objscoresN(:,1)=normalizeVector(objscores(:,1));
objscoresN(:,2)=normalizeVector(objscores(:,2));
objscoresN(:,3)=normalizeVector(-objscores(:,3));
objscoresN(:,4)=normalizeVector(-objscores(:,4));


[C,I] = max(sum(classscores_rbst,2));



maxnumclusts=C;
bestf  = ctrs(I,1);
bestx  = ctrs(I,2);
besty  = ctrs(I,3);
bestar = ctrs(I,4);
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

    hist(reshape(arfinals,sizeFs*numtries,1),numtries/2);
    title(['AR of camera center']);
    %

    fff=zeros(sizeFs*numtries,3);
    for k=1:numclusts
        scolors{k}=rand(1,3);
    end

    %     for z=1:numclusts
    %         figure
    %         for k=1:sizeFs*numtries
    %             if(idx(k,1)~=z)
    %                 fff(k,:)=  scolors{idx(k,1)};
    %             else
    %                 fff(k,:)=  [0 0 0];
    %             end
    %         end
    %
    %
    %
    %         scatter3(X(:,1),X(:,2),X(:,3),10,fff,'filled');
    %         title(['new scatter plot of all the results found using my method, cluster ' num2str(z) ' in black']);
    %         zlabel('x coordinates of optical centers');
    %         ylabel('y coordinates of optical centers');
    %         xlabel('value of focal length ');
    %     end

    figure
    for k=1:sizeFs*numtries
        if(idx(k,1)~=I)
            fff(k,:)=  scolors{idx(k,1)};
        else
            fff(k,:)=  [0 0 0];
        end
    end



    scatter(XComplete(:,1),XComplete(:,4),10,fff,'filled');
    title(['new scatter plot of all the results found using my method, winning cluster in black']);
    zlabel('focal length');
    ylabel('aspect ratio');

    figure

    scatter3(XComplete(:,1),XComplete(:,2),XComplete(:,3),10,fff,'filled');
    title(['new scatter plot of all the results found using my method, winning cluster, ' num2str(I) ' in black']);
    zlabel('x coordinates of optical centers');
    ylabel('y coordinates of optical centers');
    xlabel('value of focal length ');



end



%toc
if(validSLFSolution(bestf,bestx,besty,bestar,w,h)~=1)
    bestf=w+h;
    bestx=w/2;
    besty=h/2;
end


fcl=[ bestf bestf*bestar];
centerloc=[bestx  besty];

%bestar

end


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

function retVal = validSLFSolution(f,x,y,ar,w,h)

retVal=1;

if(ar>1.4 || ar<0.6)
    retVal=0;
end


if~(f>200 && f<1600 && imag(f)==0 && isnan(f)==0)
    retVal=0;
end

if~(x<w && x>0 && imag(x)==0 && isnan(x)==0)
    retVal=0;
end

if~(y<w && y>0 && imag(y)==0 && isnan(y)==0)
    retVal=0;
end

% if(retVal==0)
%     f,x,y,ar
% end


end

function nrmVect = normalizeVector(v)

[m,n]=size(v);

if(n>1)
    disp('bad size for vector');
end


mn=min(v);
mx=max(v);
range=mx-mn;

v=v-mn;
v=v./range;
nrmVect=v;
end

