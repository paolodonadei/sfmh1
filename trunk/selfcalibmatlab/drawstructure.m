%close all;
% clear all;
% clc
seqname='wadham';

A=dlmread(['C:\Documents and Settings\hrast019\Desktop\sfmh1\temporary\' seqname 'sfmh\structurelast.txt']);
%A=dlmread(['/home/houman/work/thesiscode/sfmh1/temporary/' seqname 'sfmh/structure1.txt']);

size(A)

[C,I] = max(A);
A(I(1),:)=[];

[C,I] = max(A);
A(I(2),:)=[];

[C,I] = max(A);
A(I(3),:)=[];

[C,I] = max(A);
A(I(1),:)=[];

[C,I] = max(A);
A(I(2),:)=[];

[C,I] = max(A);
A(I(3),:)=[];
[C,I] = max(A);
A(I(1),:)=[];

[C,I] = max(A);
A(I(2),:)=[];

[C,I] = max(A);
A(I(3),:)=[];
[C,I] = max(A);
A(I(1),:)=[];

[C,I] = max(A);
A(I(2),:)=[];

[C,I] = max(A);
A(I(3),:)=[];

[C,I] = max(A);
A(I(1),:)=[];

[C,I] = max(A);
A(I(2),:)=[];

[C,I] = max(A);
A(I(3),:)=[];
[C,I] = max(A);
A(I(1),:)=[];

[C,I] = max(A);
A(I(2),:)=[];

[C,I] = max(A);
A(I(3),:)=[];
figure
scatter3(A(:,1),A(:,2),A(:,3),5)