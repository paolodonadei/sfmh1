function  [ K1 K2] = estimateUXfromFandVY( F,x,ux1,ux2,vy1,vy2 )


K1=eye(3,3);
K2=eye(3,3);


f1=0;
f2=0;


P=PsfromF( F );
[m n ]=size(P);
numFrames=n;


P_in=P;



if(m>1)
    disp([ 'the size of the input cell is wrong']);
end


%create the normalizing matrix

%create the normalizing matrix for 1st camera
K_norm1=zeros(3,3);
K_norm1(1,1)=  x(1,1);
K_norm1(2,2)=  x(1,2);
K_norm1(3,3)=  1;
K_norm1(1,3)=  ux1;
K_norm1(2,3)= vy1;



K_norm2=K_norm1;
K_norm2(2,3)=vy2;
K_norm2(1,3)=ux2;



K_norm_inv1=inv(K_norm1);
K_norm_inv2=inv(K_norm2);



% K_norm=eye(3,3);
% K_norm_inv=K_norm;

%perform normalization

P_in{1,1}=K_norm_inv1*P_in{1,1};
P_in{1,2}=K_norm_inv2*P_in{1,2};
%   P_in{1,i}=P_in{1,i}/norm(P_in{1,i},'fro');



%now form A

[A,b]  = formAunknownUX( P_in );


[Q1, Q2]=QsfromAb(A,b);



S=findSolsfromQ(Q1,Q2);

for i=1:size(S,1)
  QS{1,i}=normalizeSetRank(Q1+ S(1)*Q2);  
end


%%%%%%%%%%%%%%%%%%%%%



M=chooseFinalQ(QS);
      
K1=findKfromPQ(K_norm1,P_in{1,1},M);
K2=findKfromPQ(K_norm2,P_in{1,2},M);
      
x=[f1   f2];










end