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
    P_in{1,i}=K_norm_inv*P_in{1,i};
    %   P_in{1,i}=P_in{1,i}/norm(P_in{1,i},'fro');
end


%now form A

[A,b]  = formAunknownF( P_in );


[Q1, Q2]=QsfromAb(A,b);

S=findSolsfromQ(Q1,Q2);



for i=1:size(S,1)
  QS{1,i}=normalizeSetRank(Q1+ S(1)*Q2);  
end


%%%%%%%%%%%%%%%%%%%%%


M=chooseFinalQ(QS);

K1=findKfromPQ(K_norm,P_in{1,1},M);
K2=findKfromPQ(K_norm,P_in{1,2},M);

f1=K1(1,1);
f2=K2(1,1);

M

x=[f1   f2];

end





