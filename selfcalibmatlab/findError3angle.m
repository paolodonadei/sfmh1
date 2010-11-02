function [x] = findError3angle(ij, jk, ik)

%normalize
ij=ij/norm(ij);
jk=jk/norm(jk);
ik=ik/norm(ik);

a_ij_ik=acos(dot(ij,ik));
a_ji_jk=acos(dot(-ij,jk));
a_ki_kj=acos(dot(-ik,-jk));

total=(180-(((a_ij_ik+ a_ji_jk+ a_ki_kj)*180)/pi))/180;
[a_ij_ik  a_ji_jk a_ki_kj]
x=total;

end