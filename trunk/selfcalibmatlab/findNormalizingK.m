function [ K_norm, K_norm_inverse ] = findNormalizingK(w,h)


%create the normalizing matrix
K_norm=zeros(3,3);
K_norm(1,1)=  w+h;
K_norm(2,2)=  w+h;
K_norm(3,3)=  1;
K_norm(1,3)=  w/2;
K_norm(2,3)= h/2;

K_norm_inverse=inv(K_norm);


end