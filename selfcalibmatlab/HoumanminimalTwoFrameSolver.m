function [ x, K1,K2,Q ] = HoumanminimalTwoFrameSolver( F,f1guess,f2guess,ux1,vy1,ux2, vy2 )

%normFORP , if this is set to 1, we normalize F and then we get the Ps in
%the canonic position, if its zero we normalize the Ps in which case they
%wont be in the canonical position, read the code to see how this works


%create the guess Ks
count=0;

while 1
count=count+1;

disp(['count is ' num2str(count)]);

    clear G A P P_in U S V c d Q1 Q2  QZ S QS MS M Q K1 K2;
G = normalizeFSturmTwoFrame( F,ux1,vy1,ux2,vy2,f1guess,f2guess );


P=PsfromF( G );
P_in=P;

[ K_norm1, K_norm1_inverse ] = findNormalizingK(ux1,vy1,f1guess);
[ K_norm2, K_norm2_inverse ] = findNormalizingK(ux2,vy2,f2guess);


n=P_in{1,2};

b=zeros(4,1);

%with 4 equations only, q(3,3)=c1
A = [-n(2,1) ^ 2 - n(2,2) ^ 2 - n(2,3) ^ 2 + n(1,3) ^ 2 + n(1,1) ^ 2 + n(1,2) ^ 2 2 * n(1,1) * n(1,4) - 2 * n(2,4) * n(2,1) 2 * n(1,4) * n(1,2) - 2 * n(2,4) * n(2,2) -2 * n(2,3) * n(2,4) + 2 * n(1,3) * n(1,4) n(1,4) ^ 2 - n(2,4) ^ 2; n(2,1) * n(1,1) + n(2,3) * n(1,3) + n(2,2) * n(1,2) n(2,1) * n(1,4) + n(2,4) * n(1,1) n(2,4) * n(1,2) + n(2,2) * n(1,4) n(2,4) * n(1,3) + n(2,3) * n(1,4) n(2,4) * n(1,4); n(3,1) * n(2,1) + n(3,3) * n(2,3) + n(3,2) * n(2,2) n(3,1) * n(2,4) + n(3,4) * n(2,1) n(3,4) * n(2,2) + n(3,2) * n(2,4) n(3,4) * n(2,3) + n(3,3) * n(2,4) n(3,4) * n(2,4); n(3,1) * n(1,1) + n(3,3) * n(1,3) + n(3,2) * n(1,2) n(3,1) * n(1,4) + n(3,4) * n(1,1) n(3,4) * n(1,2) + n(3,2) * n(1,4) n(3,4) * n(1,3) + n(3,3) * n(1,4) n(3,4) * n(1,4);];;








disp(['rank is ' num2str(rank(A))]);


[U,S,V] = svd(A);


S(4,4)


Q1=zeros(4,4);
Q2=zeros(4,4);

% 
%no b
c=V(:,5);
d=V(:,4);



% Q(3,3) is c1
Q1 = [c(1,1) 0 0 c(2,1); 0 c(1,1) 0 c(3,1); 0 0 c(1,1) c(4,1); c(2,1) c(3,1) c(4,1) c(5,1);];
Q2 = [d(1,1) 0 0 d(2,1); 0 d(1,1) 0 d(3,1); 0 0 d(1,1) d(4,1); d(2,1) d(3,1) d(4,1) d(5,1);];


%full Q
QZ={Q1 Q2};


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
    break;
end

for k=1:size(MS,2)
    
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
    
    
end

f1guess=f2;
f2guess=f2;
% ux1=K1(1,3) ;
% vy1=K1(2,3) ;
% ux2= K1(1,3);
% vy2=K1(2,3) ;
  x=[f1   f2]
end



end
