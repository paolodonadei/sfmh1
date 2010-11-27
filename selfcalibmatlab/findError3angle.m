function [x] = findError3angle(Pij, Pjk, Pik)

Pij=[Pij ; 0 0 0 1];
Pjk=[Pjk ; 0 0 0 1];
Pik=[Pik ; 0 0 0 1];

a_ijk=findError3anglemiddle( inv(Pij),Pjk);
a_jki=findError3anglemiddle(inv(Pjk),inv(Pik));
a_kij=findError3anglemiddle(Pik,Pij );

total=(180-(((a_ijk+ a_jki+ a_kij))))/180;
[a_ijk  a_jki  a_kij]
x=total;

end

% XYZ angle->find angle between vectors xy and yz where both originate from y. Y is
% placed at origin and x is defined in the coordinate axis of y and z is
% defined in the coordinate axis of Y 
function [a] = findError3anglemiddle(Pxy, Pxz)
Rxy = Pxy(1:3,1:3);
Rxz = Pxz(1:3,1:3);

% 
dxy = -inv(Rxy)*Pxy(1:3,4);
dxz = -inv(Rxz)*Pxz(1:3,4);
% 
% dxy = Pxy(1:3,4);
% dxz = Pxz(1:3,4);





% the two vectors whose angel has to be found are gonna be called 
%vec1 and vec2
vec1=dxy; % i made this negative to point from y to x
vec2=dxz;
% dyz=[dyz ; 1];
% vec2h=Pxy*dyz;
% vec2=vec2h(1:3)/vec2h(4);

vec1=vec1/norm(vec1);
vec2=vec2/norm(vec2);



a=acos(dot(vec1,vec2))*(180/pi);


end