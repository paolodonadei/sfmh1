function [fcl, centerloc] = S2nonlinsolveEsstwofram(TF,w,h,corrs)
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
arfinals=zeros(numtries,sizeFs);
scorearray=zeros(numtries,sizeFs);


xvari=w*0.050;
yvari=h*0.050;



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
    
    [focs, xcentrs, ycentrs, ars,scrs, bestFfinal, bestXfinal, bestYfinal,bestAR] =  findBestsolsrepeatmore(numtries,{TF{q}}, w,h,ones(1,1),finit,w/2,h/2,fvari, xvari,yvari,1,0.02);
    
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
numclusts=10;
X=[ reshape(ffinals,sizeFs*numtries,1)  reshape(arfinals,sizeFs*numtries,1)  ];

XComplete=[ reshape(ffinals,sizeFs*numtries,1)  reshape(xfinals,sizeFs*numtries,1) reshape(yfinals,sizeFs*numtries,1) reshape(arfinals,sizeFs*numtries,1)  ];
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
    
    
    
    scatter(XComplete(:,1),XComplete(:,4),10,fff2,'filled');
    title(['scatter plot of all the solution points']);
    zlabel('focal length');
    ylabel('aspect ratio');
    
    figure
    
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

% this is the part that prunes bad solutions, i will leave it out
% for q=1:sizeFs
%     for i=1:numtries
%         if(validSLFSolutionStrict(XComplete(mcount,:),w,h)~=1)
%             deletionVector(ccounter,1)=mcount;
%             ccounter=ccounter+1;
%
%         end
%
%         mcount=mcount+1;
%     end
% end
%
%
%
% for k=size(deletionVector,1):-1:1
%
%     idx_membership(deletionVector(k,1),:)=[];
%     XComplete(deletionVector(k,1),:)=[];
%
% end



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

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

function retVal = validSLFSolutionStrict( xa,w,h)
f=xa(1,1);
x=xa(1,2);
y=xa(1,3);
ar=xa(1,4);


retVal=1;

if(ar>1.3 || ar<0.7)
    retVal=0;
end

oaxis_x_l=(w/2)-(w*0.1);
oaxis_x_h=(w/2)+(w*0.1);


oaxis_y_l=(h/2)-(h*0.1);
oaxis_y_h=(h/2)+(h*0.1);


if~(f>200 && f<1600 && imag(f)==0 && isnan(f)==0)
    retVal=0;
end

if~(x<oaxis_x_h && x>oaxis_x_l && imag(x)==0 && isnan(x)==0)
    retVal=0;
end

if~(y<oaxis_y_h && y>oaxis_y_l && imag(y)==0 && isnan(y)==0)
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

