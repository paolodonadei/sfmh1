close all
clear all
clc




seqname='merton1';


width=1024;
height=768;

dirname='C:\Documents and Settings\hrast019\Desktop\sfmh1\temporary\';



realFold=[dirname seqname 'sfmh\'];
FFiles=dir([realFold '*FMotion*']);
[m,n]=size(FFiles);




for i=1:m
    
   F{1,i}=load( [realFold FFiles(i,1).name]);
    
end


originaldirname='C:\Documents and Settings\hrast019\Desktop\data\euclidean\'
originalFold=[originaldirname seqname '\'];
PFiles=dir([originalFold '*.P']);
[m,n]=size(PFiles);

for i=1:m
    
   P{1,i}=load( [originalFold PFiles(i,1).name]);
   [ks{1,i}, R, t] = vgg_KR_from_P(P{1,i});
    
end


[t] = runall(F,ks,width,height)



