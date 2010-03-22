function [z]=sfmSiftRead(seqname)



%seqname='merton1';




dirname='C:\Documents and Settings\hrast019\Desktop\sfmh1\temporary\';



realFold=[dirname seqname 'sfmh\'];


% determinig image size
IMFiles=dir([realFold '*.pgm']);
I1=imread([realFold IMFiles(1,1).name]);
[height,width]=size(I1);
height
width


FFiles=dir([realFold '*FMotion*']);
[m,n]=size(FFiles);




for i=1:m
    clear x1 x2
    F{1,i}=load( [realFold FFiles(i,1).name]);
    claim = 'This is a good example.';
    new_name = strrep([realFold FFiles(i,1).name], 'FMotion', 'matches');
    corrsmatrix= dlmread(new_name,'\t', 2, 1);
    
    x1(1,:)=corrsmatrix(:,2)';
    x1(2,:)=corrsmatrix(:,3)';
    x1(3,:)=ones(1,size(x1,2));
    
    x2(1,:)=corrsmatrix(:,4)';
    x2(2,:)=corrsmatrix(:,5)';
    x2(3,:)=ones(1,size(x2,2));
    
    corrs{1,i}=x1;
    corrs{2,i}=x2;
end


originaldirname='C:\Documents and Settings\hrast019\Desktop\data\euclidean\';
originalFold=[originaldirname seqname '\'];
PFiles=dir([originalFold '*.P']);
[m,n]=size(PFiles);

if(m~=0)
    
    for i=1:m
        
        P{1,i}=load( [originalFold PFiles(i,1).name]);
        [ks{1,i}, R, t] = vgg_KR_from_P(P{1,i});
        
    end
    
else
    
    ks{1,1}=zeros(3,3);
end

[t] = runall(F,ks,width,height,corrs)


end
