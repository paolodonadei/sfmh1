function [ x ] = PollefeyVisualwithPOLDTWOFRAMEFAM( F,w,h )
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

P=PsfromF( F );
[m n ]=size(P);
numFrames=n;


P_in=P;



if(m>1)
    disp([ 'the size of the input cell is wrong']);
end


%create the normalizing matrix
[ K_norm, K_norm_inv ] = findNormalizingK(w,h);

% K_norm=eye(3,3);
% K_norm_inv=K_norm;

%perform normalization
for i=1:numFrames
    P_in{1,i}
  P_in{1,i}=K_norm_inv*P_in{1,i};
    P_in{1,i}
    %   P_in{1,i}=P_in{1,i}/norm(P_in{1,i},'fro');
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


%%%%%%%%%%%%%%%%%%%%%


M=chooseFinalQ(QS);

M

w1=P_in{1,1}*M*(P_in{1,1}');
w2=P_in{1,2}*M*(P_in{1,2}');
w1=w1/w1(3,3)
w2=w2/w2(3,3)

K1=findKfromPQ(K_norm,P_in{1,1},M);
K2=findKfromPQ(K_norm,P_in{1,2},M);

f1=K1(1,1);
f2=K2(1,1);



x=[f1   f2];

end





