% close all;
% clear all;
% clc
seqname='merton1';

A=dlmread(['C:\Documents and Settings\hrast019\Desktop\sfmh1\temporary\' seqname 'sfmh\structure.txt']);

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
scatter3(A(:,1),A(:,2),A(:,3),5)