function [ Qout ] = findQfromV(V )

% take Q out of the svd
Q=zeros(4,4);

Q=myFormatvectoQ(V(:,10));

Qout=normalizeSetRank(Q);



end