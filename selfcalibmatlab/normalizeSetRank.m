function [ Q ] = normalizeSetRank(Qin )


%enforce rank 3 for Q by letting last sv to be zero

[UQ,SQ,VQ] = svd(Qin);
SQ(4,4)=0;
Q = UQ*SQ*(VQ');

% im gonna normalize Q so it wont make my omega oscilate
Q=Q/Q(3,3); % i think teh paper mentioned Q(3,3) is 1