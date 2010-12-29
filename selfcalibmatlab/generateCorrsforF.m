function [ Fgt,k1,k2,corrs,inlierOutlier, I1, I2,  R1, t1,R2,t2 ] = generateCorrsforF(numcorrs, outlierratio, stderror, type, seqname )
%this function generates a virtual or a real pair of images with the given
%amount of outliers and inliers with the given amount of gaussian noise
%added to everything and also the seqname is either the sequence name or
%the directory where the images reside
writefiles=1;

if nargin < 5
    if(type=='o')
        seqname='merton1';
        
    else
        if(isunix()==0)
            seqname='C:\Documents and Settings\hrast019\Desktop\data\euclidean\merton1';
        else
            seqname='/home/houman/Desktop/data';
        end
        
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
    
    
    
    
    [Fgt, corrs, I1, I2 ] = SIFTmatchpair(fname1,fname2 );
    R1=eye(3);
    R2=eye(3);
    t1=zeros(3,1);
    t2=zeros(3,1);
    
    inlierOutlier=ones(size(corrs,2),1);
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

%gui showing the F
%fig=vgg_gui_F(I1, I2,Fgt);

if(writefiles==1)
    % showing left and right features
    show(I1), hold on, plot(corrs(1,:),corrs(2,:),'g+');
    saveas(gcf,[dirname 'output1'],'png');
    save2pdf([dirname 'image1withfeat.pdf']);
    figure
    show(I2), hold on, plot(corrs(3,:),corrs(4,:),'g+');
    saveas(gcf,[dirname 'output2'],'png');
    save2pdf([dirname 'image2withfeat.pdf']);
    figure
    %save left and write images
    imwrite(I1,[dirname 'image1.png']);
    imwrite(I2,[dirname 'image2.png']);
    
    
    %showing features matches
    % Display putative matches inb one image
    
    show(im1), set('name','Putative matches'), hold on
    for n = 1:size(corrs,2)
        if(inlierOutlier(n)==1)
            line([corrs(1,n) corrs(2,n)], [corrs(3,n) corrs(4,n)],'color',[0 0 1]);
        else
            line([corrs(1,n) corrs(2,n)], [corrs(3,n) corrs(4,n)],'color',[1 0 0]);
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
    
    K = size(matches, 2) ;
    nans = NaN * ones(1,K) ;
    
    x = [ P1(1,matches(1,:)) ; P2(1,matches(2,:))+oj ; nans ] ;
    y = [ P1(2,matches(1,:)) ; P2(2,matches(2,:))+oi ; nans ] ;
    
    
    for n = 1:size(corrs,2)
        if(inlierOutlier(n)==1)
            line([corrs(1,n) corrs(2,n)], [corrs(3,n)+oj corrs(4,n)],'color',[0 0 1]);
        else
            line([corrs(1,n) corrs(2,n)], [corrs(3,n)+oj corrs(4,n)],'color',[1 0 0]);
        end
    end
    
    set(h,'Marker','.','Color','b') ;
    
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

