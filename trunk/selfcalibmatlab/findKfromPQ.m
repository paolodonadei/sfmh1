function [ K ] = findKfromPQ(K_norm,P,Q)

w=P*Q*(P');
w=w/w(3,3); %should i do this here?
 %w
%d= eig(w)

k=chol(w);

k=k/k(3,3);
K=K_norm*k;


end
