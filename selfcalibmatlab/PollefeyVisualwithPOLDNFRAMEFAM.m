function [ x ] = PollefeyVisualwithPOLDTWOFRAMEFAM( P,w,h)
%code conforms to the marr prize paper but with full parametrization of the
%Q so we solve for the 10 variables
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here
%http://www.robots.ox.ac.uk/~vgg/hzbook/code/
%http://www.csse.uwa.edu.au/~pk/research/matlabfns/
% data:   http://www.robots.ox.ac.uk/~vgg/data/data-mview.html
f1=0;
f2=0;
x=[0 0];

%remove this
typicalF=w+h;

[m n ]=size(P);
numFrames=n;


P_in=P;



if(m>1)
    disp([ 'the size of the input cell is wrong']);
end


%now form A

[A,b]  = formAunknownF( P_in );

[QZ]=QsfromAb(A,b);



S=findSolsfromQ(QZ);

if(size(QZ,2)>1)
    Q1=QZ{1,1};
    Q2=QZ{1,2};
else
    Q1=QZ{1,1};
    Q2=zeros(4,4);
end

for i=1:size(S,1)
    QS{1,i}=normalizeSetRank(Q1+ S(i,1)*Q2);
    
end
%S

%%%%%%%%%%%%%%%%%%%%%


MS=chooseFinalQ(QS);


if(size(MS,2)==0)
    M=eye(4,4);
    
else
    
    M=MS{1,1};
    
end

K1=findKfromPQ(eye(3),P_in{1,1},M);
K2=findKfromPQ(eye(3),P_in{1,2},M);

f1=K1(1,1)*typicalF;
f2=K2(1,1)*typicalF;



x=[f1   f2];
K1
end





