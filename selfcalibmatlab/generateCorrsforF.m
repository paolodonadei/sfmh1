function [ Fgt,k1,k2,corrs,inlierOutlier, I1, I2, I1motion, R1, t1,R2,t2 ] = generateCorrsforF(numcorrs, outlierratio, stderror, type, seqname )
%this function generates a virtual or a real pair of images with the given
%amount of outliers and inliers with the given amount of gaussian noise
%added to everything and also the seqname is either the sequence name or
%the directory where the images reside
if nargin < 5
    if(type=='o')
    seqname='merton1';
    
else
  if(isunix()==0)
      seqname='C:\Documents and Settings\hrast019\Desktop\data\euclidean\merton1';
  else
      
  end
  left off here
    end
    
    
end


%for synthetic data
if(type=='s')
    oldFolder = cd('../utils/CRC2D/');
    partonecom='./crc2Dto3D ';
    partTwo=[' -N ' num2str(numcorrs) ' -E ' num2str(outlierratio) ' -v ' num2str(stderror) ' -T 3 -q 1'];
    comma=[partonecom partTwo];
    [status currdir] = system(comma );
    M = dlmread('matches.csv',',',1);
    
    %    I1=imread('correspondences_drprevimg.png');
    %    I2=imread('correspondences_drnextimg.png');
    %    I1motion=imread('correspondences_dr.png');
    I1=zeros(512,512);
    I2=zeros(512,512);
    
    
    
    k1=load('intrinsic_synth_data.txt');
    k2=load('intrinsic_synth_data2.txt');
    Fgt=load('trueFmatrix_synth_data.txt');
    t1=zeros(3,1);
    R1=eye(3,3);
    R2=load('rotation_synth_data.txt');
    t2=load('trans_synth_data.txt');
    x1=M(:,2:3)';
    x2=M(:,4:5)';
    inlierOutlier=M(:,6)';
    corrs=[x1 ; x2];
    cd(oldFolder);
end

%for sift
if(type=='f')
    
    % so here we assume seqname is the directory name and we read the first
    % two images
    
    if(exist(seq_name,'dir')==0)
        display(['directory can not be found']);
    end
    
    imdirs=dir([seq_name '*.jpg']);
    [m,n]=size(imdirs);    
    if(m==0)
        imdirs=dir([seq_name '*.pgm']);
        [m,n]=size(imdirs);
    elseif(m==0)
        imdirs=dir([seq_name '*.png']);
        [m,n]=size(imdirs);
    elseif(m==0)
        imdirs=dir([seq_name '*.tif']);
        [m,n]=size(imdirs);
    elseif(m==0)
        display(['NO IMAGES WERE FOUND']);
    end
   


    fname1=imdirs(1,1).name;
    fname2=imdirs(2,1).name;
    
    
    
    
    [ corrs, I1, I2 ] = SIFTmatchpair(fname1,fname2 );
    R1=eye(3);
    R2=eye(3);
    t1=zeros(3,1);
    t2=zeros(3,1);
    
    inlierOutlier=zeros(size(corrs,2),1);
end


%for oxford
if(type=='o')
    [corrs, IMS, P,K, F, E, FFormatted, corrsFormatted,EFormatted,FFormattedGT,inlierOutlier] = readCorrsOxford(seqname, outlierratio, 1,2,stderror);
    corrs=corrsFormatted{1,2};
    [k1, R1, t1] = vgg_KR_from_P(P{1});
    [k2, R2, t2] = vgg_KR_from_P(P{2});
    I1=IMS{1};
    I2=IMS{2};
    Fgt=FFormattedGT{1,2};
end





end