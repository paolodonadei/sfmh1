function [ p] = planeInfFromQ( Q )

%calculate plane at infinity from Q

[U,S,V] = svd(Q);
p=V(:,4);


end

