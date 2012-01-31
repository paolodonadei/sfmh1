function [fcl, centerloc] = HoughSelfcalib(TF,w,h,corrs)
%this function , given a camera center and a focal length and a fundamental
%matrix computes the error with respect to a fundamental matrix
%tic
if (nargin == 1)
    w=512;
    h=512;
end
%TF=TF*10000;
plotting=1;
maxfocal=2000;
fcl=[0 0];

xcen=0;
ycen =0;
centerloc=[xcen ycen];

[m,sizeFs]=size(TF);



numtries=floor(1000/sizeFs);  % we want to have 400 points in the end


ffinals=zeros(numtries,sizeFs);
xfinals=zeros(numtries,sizeFs);
yfinals=zeros(numtries,sizeFs);
ffinalsinitial=zeros(numtries,sizeFs);
xfinalsinitial=zeros(numtries,sizeFs);
yfinalsinitial=zeros(numtries,sizeFs);
scorearray=zeros(numtries,sizeFs);


xvari=w*0.03;
yvari=h*0.03;



maxfocal=2000;
minfocal=200;

finit=w;

fvari=(maxfocal-minfocal);

xinit=w/2;
yinit=h/2;

for q=1:sizeFs
    
    clear focs xcentrs ycentrs scrs bestFfinal bestXfinal bestYfinal;
    
    [focs, xcentrs, ycentrs, scrs, bestFfinal, bestXfinal, bestYfinal,focsstart, xcentrsstart, ycentrsstart] =  nonlinearOptimizeselfcalib(numtries,{TF{q}}, w,h,ones(1,1),finit,w/2,h/2,fvari, xvari,yvari,1,0);
    
    %bestFfinal, bestXfinal, bestYfinal,bestAR
    
    ffinals(:,q)=focs;
    xfinals(:,q)= xcentrs;
    yfinals(:,q)=ycentrs;
    
       ffinalsinitial(:,q)=focs;
    xfinalsinitial(:,q)= xcentrs;
    yfinalsinitial(:,q)=ycentrs;
    
    scorearray(:,q)=scrs;
    
end

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%



%%%%%%%%%%5
numclusts=10;
X=[ reshape(ffinals,sizeFs*numtries,1)  reshape(xfinals,sizeFs*numtries,1)  ];

XComplete=[ reshape(ffinals,sizeFs*numtries,1)  reshape(xfinals,sizeFs*numtries,1) reshape(yfinals,sizeFs*numtries,1) ];
XCompleteinitial=[ reshape(ffinalsinitial,sizeFs*numtries,1)  reshape(xfinalsinitial,sizeFs*numtries,1) reshape(yfinalsinitial,sizeFs*numtries,1) ];
if(plotting==1)
    [newsizepts,n]=size(XComplete);
    fff=zeros(newsizepts,3);
    
    for k=1:sizeFs
        scolors{k}=rand(1,3);
    end
    
    %remove this later
    scolors{1}=[0.9 0.0 0.0];
    scolors{2}=[0.0 0.9 0.0];
    scolors{3}=[0.0 0.0 0.9];
    
    
    count=1;
    for k=1:sizeFs
        for m=1:numtries
            
            fff2(count,:)=  scolors{k};
            
            count=count+1;
        end
    end
    
    
    
    
    scatter3(XComplete(:,1),XComplete(:,2),XComplete(:,3),10,fff2,'filled');
    title(['scatter plot of all the solution points']);
    zlabel('x coordinates of optical centers');
    ylabel('y coordinates of optical centers');
    xlabel('value of focal length ');
end


%%%%%%%%%%%
%creating an index of the memberships
mcount=1;

for q=1:sizeFs
    for i=1:numtries
        idx_membership(mcount,1)=q;
        mcount=mcount+1;
    end
end

mcount=1;
ccounter=1;


[newsizepts,n]=size(XComplete);


[idx,ctrs,sumd] = kmeans(XComplete,numclusts,'Replicates',5,'emptyaction','drop','display','off');








classscores=zeros(numclusts,sizeFs);

%this is a dummy variable helping to debug this thing, the columns contain
%some attributes of the classes , i wont need this in the production
%version
objscores=zeros(numclusts,3);


for j=1:newsizepts
    if(idx_membership(j)~=0)
        classscores(idx(j),idx_membership(j))=classscores(idx(j),idx_membership(j))+1;
    end
end


numMaxima=2;
Iold=2;
I=1;
numvotes=0;
while(numMaxima~=1)
    Iold=I;
    numvotes=numvotes+1;
    clear    objscores C;
    objscores(:,1)= sum(classscores>numvotes,2)   ;
    [C,I] = max(objscores);
    numMaxima=sum(objscores==C,1) ;
end

if(numMaxima~=1)
    I=Iold;
end






maxnumclusts=C;
bestf  = ctrs(I,1);
bestx  = ctrs(I,2);
besty  = ctrs(I,3);
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
    

    
    fff=zeros(newsizepts,3);
    for k=1:numclusts
        scolors{k}=rand(1,3);
    end
    
    
    
    figure
    for k=1:newsizepts
        if(idx(k,1)~=I)
            fff(k,:)=  scolors{idx(k,1)};
        else
            fff(k,:)=  [0 0 0];
        end
    end
    
    
    
  
    
    scatter3(XComplete(:,1),XComplete(:,2),XComplete(:,3),10,fff,'filled');
    title(['new scatter plot of all the results found using my method, winning cluster, ' num2str(I) ' in black']);
    zlabel('x coordinates of optical centers');
    ylabel('y coordinates of optical centers');
    xlabel('value of focal length ');
    
    
    subplot(1,2,1)
    scatter3(XComplete(:,1),XComplete(:,2),XComplete(:,3),10,fff2,'filled');
    title(['scatter plot of all the solution points']);
    zlabel('x coordinates of optical centers');
    ylabel('y coordinates of optical centers');
    xlabel('value of focal length ');
    subplot(1,2,2)
    scatter3(XComplete(:,1),XComplete(:,2),XComplete(:,3),10,fff,'filled');
    title(['new scatter plot of all the results found using my method, winning cluster, ' num2str(I) ' in black']);
    zlabel('x coordinates of optical centers');
    ylabel('y coordinates of optical centers');
    xlabel('value of focal length ');
    
    figure
     
    subplot(1,2,1)
    scatter3(XCompleteinitial(:,1),XCompleteinitial(:,2),XCompleteinitial(:,3),10,fff2,'filled');
    title(['scatter plot of all the initial solution points']);
    zlabel('x coordinates of optical centers');
    ylabel('y coordinates of optical centers');
    xlabel('value of focal length ');
    subplot(1,2,2)
    scatter3(XCompleteinitial(:,1),XCompleteinitial(:,2),XCompleteinitial(:,3),10,fff,'filled');
    title(['new scatter plot of all the initial results found using my method, winning cluster, ' num2str(I) ' in black']);
    zlabel('x coordinates of optical centers');
    ylabel('y coordinates of optical centers');
    xlabel('value of focal length ');
end



%toc
if(validSLFSolutionz(bestf,bestx,besty,w,h)~=1)
    bestf=w+h;
    bestx=w/2;
    besty=h/2;
end


fcl=[ bestf bestf];
centerloc=[bestx  besty];

%bestar

end


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

function retVal = validSLFSolutionz(f,x,y,w,h)

retVal=1;



if~(f>200 && f<1600 && imag(f)==0 && isnan(f)==0)
    retVal=0;
end

if~(x<w && x>0 && imag(x)==0 && isnan(x)==0)
    retVal=0;
end

if~(y<h && y>0 && imag(y)==0 && isnan(y)==0)
    retVal=0;
end


end

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
