function [ Fgt,k1,k2,corrs,corrsclean, inlierOutlier, I1, I2,  R1, t1,R2,t2 ] = generateCorrsforF(numcorrs, outlierratio, stderror, type, seqname )
%this function generates a virtual or a real pair of images with the given
%amount of outliers and inliers with the given amount of gaussian noise
%added to everything and also the seqname is either the sequence name or
%the directory where the images reside
writefiles=0;


dirnames{1,1}='/home/houman/work/test_data/';
dirnames{2,1}='C:\Documents and Settings\hrast019\Desktop\data\euclidean\';
dirnames{3,1}='E:\Documents and Settings\houman\Desktop\data\euclidean\';
dirnames{4,1}='C:\mystf\data\euclidean\';

[m,n]=size(dirnames);
path='empty';

if nargin < 5
    if(type=='o' || type=='f')
        seqname='merton1';
    end
end

if(type=='f')
    for i=1:m
        if(exist([dirnames{i,1} seqname],'dir')~=0)
            path=[dirnames{i,1} seqname '/'];
            break;
        end
    end
    if(strcmp(path,'empty')==1)
        display('path not found in matlab');
    end
end



dirindex=0;
if(writefiles==1)
    while(exist(['corrsfolder' num2str(dirindex)],'dir')~=0)
        dirindex=dirindex+1;
    end
    
    dirname=['corrsfolder' num2str(dirindex) '/'];
    mkdir(dirname);
end



%for synthetic data
if(type=='s')
    oldFolder = cd('../utils/CRC2D/');
    partonecom='./crc2Dto3D ';
    
    if(isunix~=0)
        partonecom='./crc2Dto3D ';
    else
        partonecom='crc3D2D.exe ';
    end
    
    partTwo=[' -N ' num2str(numcorrs) ' -E ' num2str(outlierratio) ' -v ' num2str(stderror) ' -T 3 -q 1'];
    comma=[partonecom partTwo];
    while(exist(['matches.csv'],'file')==0)
        [status currdir] = system(comma );
    end
    
    M = dlmread('matches.csv',',',1,0);
    MCL = dlmread('matchesclean.csv',',',1,0);
    delete('matches.csv');
    delete('matchesclean.csv');
    %    I1=imread('correspondences_drprevimg.png');
    %    I2=imread('correspondences_drnextimg.png');
    %    I1motion=imread('correspondences_dr.png');
    I1=uint8(zeros(512,512,3));
    I2=uint8(zeros(512,512,3));
    
    
    
    k1=load('intrinsic_synth_data.txt');
    k2=load('intrinsic_synth_data2.txt');
    Fgt=load('trueFmatrix_synth_data.txt');
    t1=zeros(3,1);
    R1=eye(3,3);
    R2=load('rotation_synth_data.txt');
    t2=load('trans_synth_data.txt');
    x1=M(:,2:3)';
    x2=M(:,4:5)';
    
      x1c=MCL(:,2:3)';
    x2c=MCL(:,4:5)';
    
    inlierOutlier=M(:,6)';
      inlierOutlier=  1-inlierOutlier;
    corrs=[x1 ; x2];
     corrsclean=[x1c ; x2c];
    cd(oldFolder);
end

%for sift
if(type=='f')
    
    % so here we assume seqname is the directory name and we read the first
    % two images
    
    if(exist(path,'dir')==0)
        display(['directory can not be found']);
    end
    
    imdirs=dir([path '*.jpg']);
    [m,n]=size(imdirs);
    if(m==0)
        imdirs=dir([path '*.pgm']);
        [m,n]=size(imdirs);
    elseif(m==0)
        imdirs=dir([path '*.png']);
        [m,n]=size(imdirs);
    elseif(m==0)
        imdirs=dir([path '*.tif']);
        [m,n]=size(imdirs);
    elseif(m==0)
        display(['NO IMAGES WERE FOUND']);
    end
    
    
    
    fname1=imdirs(1,1).name;
    fname2=imdirs(2,1).name;
    
    
    
    
    [Fgt, corrs, I1, I2 ] = SIFTmatchpair([path fname1],[path fname2] );
    R1=eye(3);
    R2=eye(3);
     k1=eye(3);
    k2=eye(3);
    t1=zeros(3,1);
    t2=zeros(3,1);
    corrsclean=corrs;
    inlierOutlier=ones(size(corrs,2),1);
end


%for oxford
if(type=='o')
    [cocorrs, IMS, P,K, F, E, FFormatted, corrsFormatted,EFormatted,FFormattedGT,inlierOutlier,corrsFormattedclean] = readCorrsOxford(seqname, outlierratio, 1,2,stderror);

    corrs(1:2,:)=corrsFormatted{1,2}(1:2,:);
    corrs(3:4,:)=corrsFormatted{1,2}(4:5,:);
    [k1, R1, t1] = vgg_KR_from_P(P{1});
    [k2, R2, t2] = vgg_KR_from_P(P{2});
    I1=double(double(IMS{1})/255);
    I2=double(double(IMS{2})/255);
    Fgt=FFormattedGT{1,2};
    inlierOutlier=1-inlierOutlier;
   corrsclean(1:2,:)=corrsFormattedclean{1,2}(1:2,:);
     corrsclean(3:4,:)=corrsFormattedclean{1,2}(4:5,:);
end

%gui showing the F
%fig=vgg_gui_F(I1, I2,Fgt');

if(writefiles==1)
    % showing left and right features
    show(I1,1), hold on, plot(corrs(1,:),corrs(2,:),'g+');
    saveas(gcf,[dirname 'output1'],'png');
    save2pdf([dirname 'image1withfeat.pdf']);
    
    show(I2,2), hold on, plot(corrs(3,:),corrs(4,:),'g+');
    saveas(gcf,[dirname 'output2'],'png');
    save2pdf([dirname 'image2withfeat.pdf']);
    
    %save left and write images
    imwrite(I1,[dirname 'image1.png']);
    imwrite(I2,[dirname 'image2.png']);
    
    
    %showing features matches
    % Display putative matches inb one image
    
    show(I1,3), set(3,'name','Putative matches'), hold on
    for n = 1:size(corrs,2)
        if(inlierOutlier(n)==1)
            line([corrs(1,n) corrs(3,n)], [corrs(2,n) corrs(4,n)],'color',[0 0 1]);
        else
            line([corrs(1,n) corrs(3,n)], [corrs(2,n) corrs(4,n)],'color',[1 0 0]);
        end
    end
    
    saveas(gcf,[dirname 'matchesoneimage'],'png');
    save2pdf([dirname 'matchesoneimage.pdf']);
    figure
    % now display them side by side
    
    [M1,N1,K1]=size(I1) ;
    [M2,N2,K2]=size(I2) ;
    
    N3=N1+N2 ;
    M3=max(M1,M2) ;
    oj=N1 ;
    oi=0 ;
    
    I=zeros(M3,N3,K1) ;
    
    I(1:M1,1:N1,:) = I1 ;
    I(oi+(1:M2),oj+(1:N2),:) = I2 ;
    
    
    axes('Position', [0 0 1 1]) ;
    imagesc(I) ; colormap gray ; hold on ; axis image ; axis off ;
    
    
    
    for n = 1:size(corrs,2)
        if(inlierOutlier(n)==1)
            line([corrs(1,n) corrs(3,n)+oj ], [corrs(2,n) corrs(4,n)],'color',[0 1 0]);
        else
            line([corrs(1,n) corrs(3,n)+oj ], [corrs(2,n) corrs(4,n)],'color',[1 0 0]);
        end
    end
    
    %?????    set(gcf,'Marker','.','Color','b') ;
    
    saveas(gcf,[dirname 'matchesside'],'png');
    save2pdf([dirname 'matchesside.pdf']);
    
    % now save all the files
    save([dirname 'Fgt.txt'], 'Fgt','-ascii', '-double');
    save([dirname 'k1.txt'], 'k1','-ascii', '-double');
    save([dirname 'k2.txt'], 'k2','-ascii', '-double');
    save([dirname 'corrs.txt'], 'corrs','-ascii', '-double');
    save([dirname 'inlierOutlier.txt'], 'inlierOutlier','-ascii', '-double');
    save([dirname 'R1.txt'], 'R1','-ascii', '-double');
    save([dirname 'R2.txt'], 'R2','-ascii', '-double');
    
    save([dirname 't1.txt'], 't1','-ascii', '-double');
    save([dirname 't2.txt'], 't2','-ascii', '-double');
    
    [stat, mess, id]=rmdir('currentProjcorrs','s');
    copyfile(dirname,'currentProjcorrs');
end

end

%1) only thing i have not done here is make sure the number of corrrspondences
%is the same as that requested for the cases of sift and oxford
% next step is to write a function to calculate epipolar error

%2)  other thing to do is to move this synthetic correlation creation to
% matlab , its pretty time consuming now