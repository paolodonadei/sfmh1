function [ MF,corrs,ks,height,width ] = generateFImages( seqname,noiselevel, numCorruptFs )
tic
% a lot of the things here are taken from peter kovesi
%http://www.csse.uwa.edu.au/~pk/research/matlabfns/Robust/example/index.html

matchtech=2; % 1 for haris with correlation, 2 for sift
thresh = 500;   % Harris corner threshold
nonmaxrad = 3;  % Non-maximal suppression radius
w = 7;    % Window size for correlation matching
t = .001;  % Distance threshold for deciding outliers
showgraphs=1;
countF=0;



maxfunds=11;


dirname='C:\Documents and Settings\hrast019\Desktop\data\euclidean\';
%dirname='/home/houman/work/test_data/';
realFold=[dirname seqname '/'];
imgformats={'pgm','jpg','png','tif','ppm'};
numimages=0;
outdirname=['projFolderre' num2str(sum(round(100*clock))) '/'];
mkdir(outdirname);

formatnum=1;
fid2 = fopen([outdirname 'F_index.txt'], 'w');



while(numimages==0)
    
    IMFiles=dir([realFold '*.' imgformats{1,formatnum}]);
    numimages=size(IMFiles,1);
    formatnum=formatnum+1;
end

for i=1:numimages
    
    IMS{1,i}=imread(  [realFold  '' IMFiles(i,1).name ]);
    
end

[height,width,ch]=size(IMS{1,1});

[m,numIs]=size(IMS);



PFiles=dir([realFold '*.P']);
[m,n]=size(PFiles);

if(m~=0)
    
    for i=1:m
        
        P{1,i}=load( [realFold PFiles(i,1).name]);
        [ks{1,i}, rrrrr, tttt] = vgg_KR_from_P(P{1,i});
        
    end
    
else
    
    ks{1,1}=zeros(3,3);
end



for i=1:numIs
    for j=1:i
        if(i~=j)
            
            
            countF=countF+1;
            disp(['fundamental matrix number ' num2str(countF) ' between frame ' num2str(i) ' with ' num2str(j)]);
            clear cim1 r1 c1 cim2 r2 c2 m1 m2 F   inliers im1 im2 m1 m2 x1 x2 I1 I2 frames1 descriptors1 frames2 descriptors2 xin1 xin2;
            
            [vm1,vn1,vj1]=size(IMS{1,i});
            [vm2,vn2,vj2]=size(IMS{1,j});
            
            
            if(vj1>1 )
                im1 = 	rgb2gray(IMS{1,i});
            else
                im1 = 	(IMS{1,i});
            end
            if(vj2>1 )
                im2 = 	rgb2gray(IMS{1,j});
            else
                im2 = 	(IMS{1,j});
            end
            
            if(matchtech==1)
                [cim1, r1, c1] = harris(im1, 1, thresh, 3);
                [cim2, r2, c2] = harris(im2, 1, thresh, 3);
                [m1,m2] = matchbycorrelation(im1, [r1';c1'], im2, [r2';c2'], w);
                x1 = [m1(2,:); m1(1,:); ones(1,length(m1))];
                x2 = [m2(2,:); m2(1,:); ones(1,length(m1))];
            else
                I1 = double(im1) ;
                I2 = double(im2) ;
                
                I1=I1/256;
                I2=I2/256;
                
                
                [frames1,descriptors1] = sift(I1, 'Verbosity', 0) ;
                
                [frames2,descriptors2] = sift(I2, 'Verbosity', 0) ;
                matches=siftmatch( descriptors1, descriptors2 ) ;
                
                [sm,sn]=size(matches);
                disp(['number of matches was: ' num2str(sn)]);
                
                x1=zeros(3,sn);
                x2=zeros(3,sn);
                
                
                
                for p=1:sn
                    x1(1,p)= frames1(1,matches(1,p)) ;
                    x1(2,p)= frames1(2,matches(1,p)) ;
                    x1(3,p)=1  ;
                    m1(1,p)= x1(2,p) ;
                    m1(2,p)= x1(1,p) ;
                    
                    x2(1,p)=  frames2(1,matches(2,p));
                    x2(2,p)=  frames2(2,matches(2,p));
                    x2(3,p)= 1 ;
                    m2(1,p)= x2(2,p) ;
                    m2(2,p)= x2(1,p) ;
                end
                
                for p=1:sn
                    r1(p,1)= x1(2,p)  ;
                    c1(p,1)= x1(1,p)  ;
                    
                    r2(p,1)= x2(2,p)  ;
                    c2(p,1)= x2(1,p)  ;
                end
                
                
                %         plotmatches(I1,I2,frames1(1:2,:),frames2(1:2,:),matches) ;
                %         drawnow ;
            end
            
            
            [F, inliers] = ransacfitfundmatrix(x1, x2, t);
            MF{countF}=F;
            GG=F;
            fname=['F' num2str(i-1) '_' num2str(j-1) '.txt'];
            save([outdirname  fname], 'GG','-ascii', '-double');
            fprintf(fid2, ' %d\t%d\t%s\n' ,i-1,j-1,fname);
      
            [im,in]=size(inliers);
            
            disp(['number of inliers is: ' num2str(in)]);
            
            for k=1:in
                xin1(:,k)= x1(:,inliers(1,k));
                xin2(:,k)= x2(:,inliers(1,k));
                
            end
            corrs{1,countF}=xin1;
            corrs{2,countF}=xin2;
            
            
            l2 = F*x1;    % Epipolar lines in image2
            l1 = F'*x2;   % Epipolar lines in image1
            
            if(showgraphs==1)
                
                show(im1,1), hold on, plot(c1,r1,'r+');
                saveas(gcf,[outdirname seqname  '_features_im' num2str(i) '.png']);
                saveas(gcf,[outdirname seqname  '_features_im' num2str(i) '.eps'],'epsc');
                
                show(im2,2), hold on, plot(c2,r2,'r+');
                saveas(gcf,[outdirname seqname  '_features_im' num2str(j) '.png']);
                saveas(gcf,[outdirname seqname  '_features_im' num2str(j) '.eps'],'epsc');
                
                % Display putative matches
                show(im1,3), set(3,'name','Putative matches'), hold on
                for n = 1:length(m1);
                    line([m1(2,n) m2(2,n)], [m1(1,n) m2(1,n)])
                end
                saveas(gcf,[outdirname seqname  '_putative_' num2str(i) 'to_' num2str(j) '.png']);
                saveas(gcf,[outdirname seqname  '_putative_' num2str(i) 'to_' num2str(j) '.eps'],'epsc');
                
                % Display both images overlayed with inlying matched feature points
                show(double(im1)+double(im2),4), set(4,'name','Inlying matches'), hold on
                plot(m1(2,inliers),m1(1,inliers),'r+');
                plot(m2(2,inliers),m2(1,inliers),'g+');
                
                for n = inliers
                    line([m1(2,n) m2(2,n)], [m1(1,n) m2(1,n)],'color',[0 0 1])
                end
                saveas(gcf,[outdirname seqname  '_inlier_' num2str(i) 'to_' num2str(j) '.png']);
                saveas(gcf,[outdirname seqname  '_inlier_' num2str(i) 'to_' num2str(j) '.eps'],'epsc');
                
                %                 for n = inliers
                %                     figure(1), clf, show(im1,1), hold on, plot(x1(1,n),x1(2,n),'r+');
                %                     hline(l1(:,n));
                %                     figure(2), clf, show(im2,2), hold on, plot(x2(1,n),x2(2,n),'r+');
                %                     hline(l2(:,n));
                %                     fprintf('hit any key to see next point\r'); pause
                %                 end
                
                
                
                close all;
                
                
                
                
                
                
            end
        end
    end
    
    toc
    
    
    
    
end

fclose(fid2);

[stat, mess, id]=rmdir('currentProjreal','s');
copyfile(outdirname,'currentProjreal');


end