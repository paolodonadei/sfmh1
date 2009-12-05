function [ K_norm, K_norm_inverse ] = findNormalizingK(ux,vy,fl)


%create the normalizing matrix
K_norm=zeros(3,3);
K_norm(1,1)= fl;  
K_norm(2,2)=  fl;
K_norm(3,3)=  1;
K_norm(1,3)=  ux;
K_norm(2,3)= vy;

K_norm_inverse=inv(K_norm);


end