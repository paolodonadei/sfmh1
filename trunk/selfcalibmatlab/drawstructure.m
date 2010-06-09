%close all;
% clear all;
% clc
seqname='merton1';

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
scatter3(A(:,1),A(:,2),A(:,3),5);

figure

F = TriScatteredInterp(A(:,1),A(:,2),A(:,3));
ti = -4:.25:4;
[qx,qy] = meshgrid(ti,ti);
qz = F(qx,qy);
mesh(qx,qy,qz);

