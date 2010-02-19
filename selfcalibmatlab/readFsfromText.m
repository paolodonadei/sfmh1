function [F]=readFsfromText(path)




if(path(size(path,2))~='\' && path(size(path,2))~='/' )
    path=[path '/'] ;
end

% get the Ps
psdir=dir([path '*FMotion.txt']);
[m,n]=size(psdir);


count=0;
for i=1:m
    count=count+1;
    F{1,count}=load([path psdir(i,1).name]);
   F{1,count}=F{1,count}/F{1,count}(3,3);
    
    clear D;
end




end