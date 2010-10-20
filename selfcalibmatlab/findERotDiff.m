function [RP12,RP23,RP13, q12, q23, q13 ]= findERotDiff(R1,R2,R3)

RP12=R2*inv(R1);
RP23=R3*inv(R2);
RP13=R3*inv(R1);

[q12] = rotmat2quat(RP12);
[q23] = rotmat2quat(RP23);
[q13] = rotmat2quat(RP13);

RRes= inv(RP13)*RP23*RP12


QRes=  quatmultiply(quatconj(q13),quatmultiply(q23,q12 ) )


end