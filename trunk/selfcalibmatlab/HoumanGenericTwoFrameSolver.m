function [ x, K1,K2,Q ] = HoumanGenericTwoFrameSolver( F,f1guess,f2guess,ux1,vy1,ux2, vy2,functionFormA,normF )

%normFORP , if this is set to 1, we normalize F and then we get the Ps in
%the canonic position, if its zero we normalize the Ps in which case they
%wont be in the canonical position, read the code to see how this works


%create the guess Ks




if(normF==1)
    F = normalizeFSturmTwoFrame( F,ux1,vy1,ux2,vy2,f1guess,f2guess );
end

P=PsfromF( F );
P_in=P;

[ K_norm1, K_norm1_inverse ] = findNormalizingK(ux1,vy1,f1guess);
[ K_norm2, K_norm2_inverse ] = findNormalizingK(ux2,vy2,f2guess);


if(normF~=1)
    
    
    P_in{1,1}=K_norm1_inverse*P_in{1,1};
    P_in{1,2}=K_norm2_inverse*P_in{1,2};
end





[ A,b ]=functionFormA(P_in);


[QZ]=QsfromAb(A,b);



SL=findSolsfromQ(QZ);

if(size(QZ,2)>1)
    Q1=QZ{1,1};
    Q2=QZ{1,2};
else
    Q1=QZ{1,1};
    Q2=zeros(4,4);
end

for i=1:size(SL,1)
    QS{1,i}=normalizeSetRank(Q1+ SL(i,1)*Q2);
end

%SL

%%%%%%%%%%%%%%%%%%%%%

MS=chooseFinalQ(QS);

if(size(MS,2)==0)
    error('no M found');
end


k=1

M=MS{1,k};

w1=P_in{1,1}*M*(P_in{1,1}');
w2=P_in{1,2}*M*(P_in{1,2}');
w1=w1/w1(3,3);
w2=w2/w2(3,3);

K1=findKfromPQ(K_norm1,P_in{1,1},M);
K2=findKfromPQ(K_norm2,P_in{1,2},M);

f1=K1(1,1);
f2=K2(1,1);


Q=M;
x=[f1   f2];
K1
K2



end
