function [qo] = quatconj(q)

qo=q;

qo(2)=-qo(2);
qo(3)=-qo(3);
qo(4)=-qo(4);
end