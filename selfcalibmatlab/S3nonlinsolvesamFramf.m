function [K] = S3nonlinsolvesamFramf(TF,corrs,w,h,ks)
%this shows random sampling consensus
%this function , given a camera center and a focal length and a fundamental
%matrix computes the error with respect to a fundamental matrix
%tic
if (nargin == 2)
    w=512;
    h=512;
end
maxf=2000;
minf=200;
maxs=8;
mins=-8;
maxXdeviationpercentage=0.20;
maxYdeviationpercentage=0.20;

framesconstant=0;
numparams=1;
[s1,s2]=size(TF);

numFrames=s1;


unknownFrames=numFrames;

if(framesconstant==1)
    unknownFrames=1;
else
    unknownFrames=numFrames;
end


m=unknownFrames*numparams;
n=m;

Weights=cell(numFrames,numFrames);
K=cell(numFrames,1);
upK=cell(numFrames,1);
lowK=cell(numFrames,1);




for i=1:numFrames
    x = PeterSturmSelfRobust( TF(i,:),w,h );
  x=1000; 
  tempK=[ x(1,1) 0 w/2 ; 0 x(1,1) h/2 ; 0 0 1];
    tempupK=[ maxf maxs (w/2)+(maxXdeviationpercentage*w) ; 0 maxf (h/2)+(maxYdeviationpercentage*h) ; 0 0 1];
    templowK=[ minf mins (w/2)-(maxXdeviationpercentage*w) ; 0 minf (h/2)-(maxYdeviationpercentage*h) ; 0 0 1];
    K{i,1}=tempK;
    upK{i,1}= tempupK;
    lowK{i,1}=templowK;
end

%[pp] = convertMatstoLin(K,framesconstant, numparams,numFrames,w,h);
[pp] = convertMatstoLin(ks,framesconstant, numparams,numFrames,w,h);
[ub] = convertMatstoLin(upK,framesconstant, numparams,numFrames,w,h);
[lb] = convertMatstoLin(lowK,framesconstant, numparams,numFrames,w,h);

iterMax= 1000;
x=zeros(m,1);

options=[1E-03, 1E-18, 1E-28, 1E-28, 1E-06];

s=cell(8,1);
s{1,1}= TF;
s{2,1}= framesconstant;
s{3,1}=numparams;
s{4,1}=numFrames;
s{5,1}=w;
s{6,1}=h;
s{7,1}=Weights;
s{8,1}=corrs;
%[ret, popt, info, covar]=levmar('S3Objective', pp, x, iterMax, options,'bc', lb, ub, s);

[ret, popt, info, covar]=levmar('S3ObjectiveRotationDist', pp, x, iterMax, options,'bc', lb, ub, s);

clear K
[K] = convertLinetoMats( popt,framesconstant, numparams,numFrames,w,h);


for i=1:numFrames
    disp(['frame ' num2str(i) ' is: ']);
    K{i}
    
end


end





