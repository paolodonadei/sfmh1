function [ G,K_norm1, K_norm2] = normalizeFSturmTwoFrame( F,ux1,vy1,ux2,vy2,F1guess,F2guess )


K_norm1=zeros(3,3);
K_norm1(1,1)=  F1guess;
K_norm1(2,2)=  F1guess;
K_norm1(3,3)=  1;
K_norm1(1,3)= ux1;
K_norm1(2,3)= vy1;


K_norm2=zeros(3,3);
K_norm2(1,1)=  F2guess;
K_norm2(2,2)=  F2guess;
K_norm2(3,3)=  1;
K_norm2(1,3)= ux2;
K_norm2(2,3)= vy2;



G=(K_norm2')*F*K_norm1;
%the reason why i did it in this order can be found on page 244, not sure
%if this order is correct

G=G/G(3,3);



end



