function [ F, ks,ps , badpoints,corrs, FFormatted, corrsFormatted ] = generateF( fdiff, skew, ar,centerdeviation,silent,number,noiselevel,numCorruptFs )

writefiles=1;
numCorrs=200;
FFormatted=cell(number,number);
corrsFormatted=cell(number,number);
badpoints=zeros(numCorrs,((number*(number-1))/2) );



[ ps ,myks ] = generatePs( fdiff, skew, ar,centerdeviation, number,1,silent,0 );


if(writefiles==1)
    dirname=['projFolder' num2str(sum(round(100*clock)))];
    mkdir(dirname);
    for i=1:number
        K=myks{1,i};
        save([dirname '/K_' num2str(i) '.txt'], 'K','-ascii', '-double');
    end
    fid = fopen([dirname '/F_index.txt'], 'w');
end


%
%
%
% if (nargin <7)
%
%     P1=ps{1,1};
%     P2=ps{1,2};
%
%
%     if(number==1)
%         disp(['error']);
%         F=zeros(3);
%     elseif(number==2)
%         F = vgg_F_from_P(P1, P2);
%     else
%
%         count=0;
%         for i=1:number
%             for j=1:i
%                 if(i~=j)
%                     count=count+1;
%                     F{count}=vgg_F_from_P(ps{1,i},ps{1,j});
%                 end
%             end
%         end
%
%
%     end
%
% else

if(noiselevel>1)
    disp('noise level is hjigher than 1');
end
if(numCorruptFs>((number*(number-1))/2))
    disp('demanded more corrupt Fs than there are Fs');
end




count=0;
for i=1:number
    for j=1:i
        if(i~=j)
            count=count+1;
            
            
            clear x1 x2 X;
            x1=zeros(3, numCorrs);
            x2=zeros(3, numCorrs);
            X=zeros(4, numCorrs);
            for q=1:numCorrs
                X(1,q)=rand()*1000 -500;
                X(2,q)=rand()*1000 -500;
                X(3,q)=rand()*1000 -500;
                X(4,q)=1;
                
                x1(:,q)=ps{1,i}*X(:,q);
                x2(:,q)=ps{1,j}*X(:,q);
                
                x2(1,q)=x2(1,q)/x2(3,q);
                x2(2,q)=x2(2,q)/x2(3,q);
                x2(3,q)=1;
                
                
                x1(1,q)=x1(1,q)/x1(3,q);
                x1(2,q)=x1(2,q)/x1(3,q);
                x1(3,q)=1;
            end
            if(count<=numCorruptFs)
                maxx=max([max(x1(1,:)) max(x2(1,:))]);
                maxy=max([max(x1(2,:)) max(x2(2,:))]);
                
                minx=min([min(x1(1,:)) min(x2(1,:))]);
                miny=min([min(x1(2,:)) min(x2(2,:))]);
                
                xrange=maxx-minx;
                yrange=maxy-miny;
                %now making the outliers
                for q=1:numCorrs*noiselevel
                    
                    
                    
                    %the reason why we have to do this is because our
                    %image features are not confined to the image size,
                    %so if we just made outliers within the image its
                    %not going to work since the outliers will not be
                    %the same magnitude as inliers
                    noisex=(rand()*xrange)+minx;
                    noisey=(rand()*yrange)+miny;
                    
                    if(rand()<0.5)
                        x2(1,q)=noisex; % outlier generation, this is whack and important
                        x2(2,q)=noisey;
                        
                    else
                        x1(1,q)=noisex; % outlier generation, this is whack and important
                        x1(2,q)=noisey;
                    end
                    
                    
                    
                    
                    
                end
            end
            
            
            
            corrs{1,count}=x1;
            corrs{2,count}=x2;
            
           corrsFormatted{i,j}=[x1 ; x2]  ;
           corrsFormatted{j,i}=[x2 ; x1]  ;
              
            [F{count}, e1, e2] = fundmatrix(x1, x2);
            
            FFormatted{i,j}=fundmatrix(x1, x2);
            FFormatted{j,i}=fundmatrix(x1, x2)';
                
            if(writefiles==1)
                GG=F{count};
                fname=['F' num2str(i-1) '_' num2str(j-1) '.txt'];
                save([dirname '/' fname], 'GG','-ascii', '-double');
                fprintf(fid, ' %d\t%d\t%s\n' ,i-1,j-1,fname);
            end
            
        end
        
        
        
    end
end







ks=myks;


if(writefiles==1)
    fclose(fid);
end

[stat, mess, id]=rmdir('currentProj','s');
copyfile(dirname,'currentProj');






end