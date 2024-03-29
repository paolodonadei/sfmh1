function [ x ] = HoumanPollefeyVisualwithPOLDTWOFRAMEFAM( F,w,h )
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
G=normalizeFSturm(F,w,h,typicalF);

K_norm=eye(3,3);


P=PsfromF( G );
[m n ]=size(P);
numFrames=n;


P_in=P;



if(m>1)
    disp([ 'the size of the input cell is wrong']);
end


%now form A
alpha=0.1;
b
b = [0 0 0 0 0 0 -n(1,1) * n(1,4) * c(2) - n(1,4) * (n(1,1) * c(2) + n(1,2) * c(3) + n(1,3) * c(4) + n(1,4) * c(5)) - n(1,2) * n(1,4) * c(3) + n(2,4) * (n(2,1) * c(2) + n(2,2) * c(3) + n(2,3) * c(4) + n(2,4) * c(5)) - n(1,3) * n(1,4) * c(4) + n(2,3) * n(2,4) * c(4) + n(2,1) * n(2,4) * c(2) + n(2,2) * n(2,4) * c(3) -alpha * (n(2,1) * n(2,4) * c(2) + n(2,4) * (n(2,1) * c(2) + n(2,2) * c(3) + n(2,3) * c(4) + n(2,4) * c(5)) + n(2,2) * n(2,4) * c(3) - n(3,4) * (n(3,1) * c(2) + n(3,2) * c(3) + n(3,3) * c(4) + n(3,4) * c(5)) + n(2,3) * n(2,4) * c(4) - n(3,1) * n(3,4) * c(2) - n(3,2) * n(3,4) * c(3) - n(3,3) * n(3,4) * c(4)) -alpha * (n(3,1) * n(3,4) * c(2) + n(3,4) * (n(3,1) * c(2) + n(3,2) * c(3) + n(3,3) * c(4) + n(3,4) * c(5)) + n(3,2) * n(3,4) * c(3) - n(1,4) * (n(1,1) * c(2) + n(1,2) * c(3) + n(1,3) * c(4) + n(1,4) * c(5)) + n(3,3) * n(3,4) * c(4) - n(1,1) * n(1,4) * c(2) - n(1,2) * n(1,4) * c(3) - n(1,3) * n(1,4) * c(4)) -n(2,1) * n(1,4) * c(2) - n(2,4) * (n(1,1) * c(2) + n(1,2) * c(3) + n(1,3) * c(4) + n(1,4) * c(5)) - n(2,2) * n(1,4) * c(3) - n(2,3) * n(1,4) * c(4) -n(3,1) * n(2,4) * c(2) - n(3,4) * (n(2,1) * c(2) + n(2,2) * c(3) + n(2,3) * c(4) + n(2,4) * c(5)) - n(3,2) * n(2,4) * c(3) - n(3,3) * n(2,4) * c(4) -n(3,1) * n(1,4) * c(2) - n(3,4) * (n(1,1) * c(2) + n(1,2) * c(3) + n(1,3) * c(4) + n(1,4) * c(5)) - n(3,2) * n(1,4) * c(3) - n(3,3) * n(1,4) * c(4)];
A=[0 0; 1 -1; -1 1; 0 0; 0 0; 0 0; n(1,2) ^ 2 + n(1,1) ^ 2 - n(2,1) ^ 2 - n(2,2) ^ 2 n(1,3) ^ 2 - n(2,3) ^ 2; alpha * (n(2,1) ^ 2 + n(2,2) ^ 2 - n(3,2) ^ 2 - n(3,1) ^ 2) alpha * (n(2,3) ^ 2 - n(3,3) ^ 2); alpha * (n(3,1) ^ 2 + n(3,2) ^ 2 - n(1,2) ^ 2 - n(1,1) ^ 2) alpha * (n(3,3) ^ 2 - n(1,3) ^ 2); n(2,1) * n(1,1) + n(2,2) * n(1,2) n(2,3) * n(1,3); n(3,1) * n(2,1) + n(3,2) * n(2,2) n(3,3) * n(2,3); n(3,1) * n(1,1) + n(3,2) * n(1,2) n(3,3) * n(1,3);];


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

K1=findKfromPQ(K_norm,P_in{1,1},M);
K2=findKfromPQ(K_norm,P_in{1,2},M);

f1=K1(1,1)*typicalF;
f2=K2(1,1)*typicalF;



x=[f1   f2];



end





