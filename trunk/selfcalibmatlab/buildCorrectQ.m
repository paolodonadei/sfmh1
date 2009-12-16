function [ QC,linf ] = buildCorrectQ(ks,pes,pps,kinv )
%this guy here returns the plane at infinity and also the correct form of
%the absolute quadric, what it takes as input is the projective projection
%matrices and the euclidean projection matrices and also the intrinsic
%parameters
K1=kinv * ks{1};


P_M=pes{2};
P_P=pps{2};

e=kinv *P_M;
e=e/e(3,4);
p=P_P;
p=p/p(3,4);

k=K1;
a = (e(1, 1)-p(1, 1)*k(1, 1)-p(1, 2)*k(2, 1)-p(1, 3)*k(3, 1))/p(1, 4);
b = (e(1, 2)-p(1, 1)*k(1, 2)-p(1, 2)*k(2, 2)-p(1, 3)*k(3, 2))/p(1, 4);
c = (e(1, 3)-p(1, 1)*k(1, 3)-p(1, 2)*k(2, 3)-p(1, 3)*k(3, 3))/p(1, 4);




pev=[ a b c];


k1inv=inv(K1);

linf=-(pev*k1inv);
linf=linf';



QC=[ K1*(K1') -K1*(K1')*linf; -(linf')*K1*(K1') -(linf')*K1*(K1')*linf];


end

