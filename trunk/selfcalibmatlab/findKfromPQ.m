function [ K ] = findKfromPQ(K_norm,P,Q)

w=P*Q*(P');
w=w/w(3,3); %should i do this here?
 %w
%d= eig(w)

try
k=chol(w);
catch ME
    disp(['we couldnt get the chol decomposition of w']);
    w
    Q
    k=eye(3,3);
end

k=k/k(3,3);
K=K_norm*k;


end
