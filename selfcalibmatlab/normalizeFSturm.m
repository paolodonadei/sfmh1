function [ G ] = normalizeFSturm( F,w,h,typicalF )
K=[1 0 0 ; 0 1 0 ; w/2 h/2 1];
KT=K';

G1=K*F*KT;

FF=[ typicalF 0 0 ; 0 typicalF 0 ; 0 0 1];

G2=FF*G1*FF;
G=G2/G2(3,3);

% K_norm=zeros(3,3);
% K_norm(1,1)=  typicalF;
% K_norm(2,2)=  typicalF;
% K_norm(3,3)=  1;
% K_norm(1,3)=  w/2;
% K_norm(2,3)= h/2;
% F
% K_norm_inverse=inv(K_norm)
% 
% G=(K_norm_inverse')*F*K_norm_inverse;
% G=G/G(3,3);
% G


end



