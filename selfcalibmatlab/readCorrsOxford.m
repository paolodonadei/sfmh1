function [corrs, IMS, P,K, F, E, FFormatted, corrsFormatted,EFormatted,FFormattedGT,statusOutliers] = readCorrsOxford(seq_name, noiselevel, numbadf,numreadFrames,gaussianerrorstd)
IMS={}
dirnames{1,1}='/home/houman/work/test_data/';
dirnames{2,1}='C:\Documents and Settings\hrast019\Desktop\data\euclidean\';
writefiles=1;
[m,n]=size(dirnames);

path='empty';
for i=1:m
    if(exist([dirnames{i,1} seq_name],'dir')~=0)
        path=[dirnames{i,1} seq_name];
    end
end

if(strcmp(path,'empty')==1)
    disp('directory not found');
    return;
end
if(nargin<5)
    gaussianerrorstd=0;
end

if(path(size(path,2))~='\' && path(size(path,2))~='/' )
    path=[path '/'] ;
end

if(noiselevel>1)
    disp('noise level is higher than one');
    noiselevel
    
end

% get the Ps
psdir=dir([path '*.P']);

if (nargin ==3)
    [m,n]=size(psdir);
elseif (nargin ==4)
    m=numreadFrames;
else
    display('something is wrong, incorrect arg number');
end

FFormatted=cell(m,m);
EFormatted=cell(m,m);
corrsFormatted=cell(m,m);


count=0;
for i=1:m
    count=count+1;
    P{1,count}=load([path psdir(i,1).name]);
    [D, R, t] = vgg_KR_from_P(P{1,count});
    K{1,count}=abs(D);
    clear D;
end




imdirs=dir([path '*.jpg']);




if (nargin ==3)
    [m,n]=size(imdirs);
elseif (nargin ==4)
    m=numreadFrames;
else
    display('something is wrong, incorrect arg number');
end


count=0;
for i=1:m
    count=count+1;
    IMS{1,count}=imread([path imdirs(i,1).name]);
end
 [imheight,imwidth,imchannels]=IMS{1,1}; % get the size of the first image



cornerdirs=dir([path '*.corners']);
[m,n]=size(cornerdirs);

count=0;
for i=1:m
    count=count+1;
    corners{1,count}= dlmread([path cornerdirs(i,1).name]);
end

textstr='';
for k=1:count
    textstr= strcat(textstr,' %s ');
end

fid = fopen([path 'nview-corners']);
nviewfile = textscan(fid, textstr);
fclose(fid);



if (nargin ==3)
    [m,numcolum]=size(nviewfile);
elseif (nargin ==4)
    numcolum=numreadFrames;
else
    display('something is wrong, incorrect arg number');
end



[numrows,n]=size(nviewfile{1,1});

for j=1:numcolum
    for i=1:numrows
        curElement=nviewfile{1,j}(i,1);
        if(strcmp('*', curElement)==1)
            nviewnums(i,j)=-1;
        else
            %            curElement{1}
            nviewnums(i,j)=str2num(curElement{1});
        end
    end
end


if(writefiles==1)
    dirname=['projFolder' num2str(sum(round(100*clock)))];
    mkdir(dirname);
    for i=1:numcolum
        Kb= K{1,i};
        save([dirname '/K_' num2str(i) '.txt'], 'Kb','-ascii', '-double');
    end
    fid = fopen([dirname '/F_index.txt'], 'w');
end


corcount=1;
for i=1:numcolum
    for j=1:i
        if(i~=j)
            count=0;
            
            clear x1 x2;
            
            for k=1:numrows
                if(nviewnums(k,i)~=-1 && nviewnums(k,j)~=-1)
                    count=count+1;
                    
              
                    
                    x1(1,count)=corners{1,i}(nviewnums(k,i)+1,1);
                    x1(2,count)=corners{1,i}(nviewnums(k,i)+1,2);
                    x1(3,count)=1;
                    
                    x2(1,count)=corners{1,j}(nviewnums(k,j)+1,1);
                    x2(2,count)=corners{1,j}(nviewnums(k,j)+1,2);
                    x2(3,count)=1;
                    
                   
                     
                     % i did not check to see if the noisy coordinates dont
                     % exceed the image boundaries, but this check might be
                     % important zzz
                end
            end
            
            FFormattedGT{i,j}=fundmatrix(x1, x2);
            FFormattedGT{j,i}=fundmatrix(x1, x2)';
            
            %add gaussian noise
            count=0;
                      for k=1:numrows
                if(nviewnums(k,i)~=-1 && nviewnums(k,j)~=-1)
                    count=count+1;
                    
                    X1Gaussian= randn()*gaussianerrorstd ;
                    Y1Gaussian=randn()*gaussianerrorstd  ;
                    X2Gaussian= randn()*gaussianerrorstd ;
                    Y2Gaussian= randn()*gaussianerrorstd ;
                    
                 
                    
                    %adding gaussian noise
                    x1(1,count)=  x1(1,count)+  X1Gaussian;
                    x1(2,count)= x1(2,count)+  Y1Gaussian;
                     x2(1,count)= x2(1,count)+  X2Gaussian;
                     x1(2,count)= x1(2,count)+Y2Gaussian;
                     
                     % i did not check to see if the noisy coordinates dont
                     % exceed the image boundaries, but this check might be
                     % important zzz
                end
                      end
            
                      
                      
                      
                      
                      
            
            if(corcount<=numbadf )
                %  disp([' going to have ' num2str(count*noiselevel) ' outliers']);
                numOutliers=int16(count*noiselevel);
                clear statusOutliers;
                statusOutliers=zeros(coun,1);
                for mm=1: numOutliers
                    indexOut=int16(rand()*count);
                    while(statusOutliers(indexOut,1)~=0)
                          indexOut=int16(rand()*count);
                    end
                    statusOutliers(indexOut,1)=0;
                end
                
                if(sum( statusOutliers)~=numOutliers)
                    display('the number of found outliers is not same as requested');
                end
                
                for q=1:count
           
                    if(statusOutliers(q,1)~=0)
                    noise1=(rand()*imwidth);
                    noise2=(rand()*imheight);
                    
                    if(rand()<0.5)
                        x2(1,q)=noise1; % outlier generation, this is whack and important
                        x2(2,q)=noise2;
                        
                    else
                        x1(1,q)=noise1; % outlier generation, this is whack and important
                        x1(2,q)=noise2;
                    end
                    
                    end
                end
            end
            
            
            %             for n = 1:(count-1)
            %                 figure(1), clf, show(IMS{1,i},1), hold on, plot(x1(1,n),x1(2,n),'r+');
            %
            %                 figure(2), clf, show(IMS{1,j},2), hold on, plot(x2(1,n),x2(2,n),'r+');
            %
            %                 fprintf('hit any key to see next point\r'); pause
            %             end
            
            
            corrs{1,corcount}=x1;
            corrs{2,corcount}=x2;
            
            [F{corcount}, e1, e2] = fundmatrix(x1, x2);
            
            corrsFormatted{i,j}=[x1 ; x2]  ;
            corrsFormatted{j,i}=[x2 ; x1]  ;
            
            
            FFormatted{i,j}=fundmatrix(x1, x2);
            FFormatted{j,i}=fundmatrix(x1, x2)';
            
            
            E{corcount}=(( K{1,j})')*F{corcount}*(( K{1,i}));
            E{corcount}=E{corcount}/E{corcount}(3,3);
            
            EFormatted{i,j}=(K{j}')*FFormatted{i,j}*(K{i});
            EFormatted{j,i}=(K{i}')*FFormatted{j,i}*(K{j});
            
            
            fprintf(' right i=%d left j=%d and index=%d \n',i,j,corcount);
            %             E{corcount}
          
            
            if(writefiles==1)
                GG=F{corcount};
                fname=['F' num2str(i-1) '_' num2str(j-1) '.txt'];
                save([dirname '/' fname], 'GG','-ascii', '-double');
                fprintf(fid, ' %d\t%d\t%s\n' ,i-1,j-1,fname);
            end
              corcount=corcount+1;
        end
    end
end


if(writefiles==1)
    fclose(fid);
    [stat, mess, id]=rmdir('currentProj','s');
    copyfile(dirname,'currentProj');
  
    
end





end
