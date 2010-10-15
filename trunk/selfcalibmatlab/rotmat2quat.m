function [q] = rotmat2quat(R)


% find the maximum of the 4 quantities
tmp(1)=1.0 +  R(1,1) +  R(2,2) +  R(3,3);
tmp(2)=1.0 +  R(1,1) -  R(2,2) -  R(3,3);
tmp(3)=1.0 -  R(1,1) +  R(2,2) -  R(3,3);
tmp(4)=1.0 -  R(1,1) -  R(2,2) +  R(3,3);

mag=-1.0;
maxpos=0;
for i=1:4
    if(tmp(i)>mag)
        mag=tmp(i);
        maxpos=i;
    end
end




if( maxpos==1)
    q(1)=sqrt( tmp(1))*0.5;
    q(2)=( R(3,2) -  R(2,3))/(4.0* q(1));
    q(3)=( R(1,3) -  R(3,1))/(4.0* q(1));
    q(4)=( R(2,1) -  R(1,2))/(4.0* q(1));
elseif( maxpos==2)
    q(2)=sqrt( tmp(2))*0.5;
    q(1)=( R(3,2) -  R(2,3))/(4.0* q(2));
    q(3)=( R(2,1) +  R(1,2))/(4.0* q(2));
    q(4)=( R(1,3) +  R(3,1))/(4.0* q(2));
elseif( maxpos==3)
    q(3)=sqrt( tmp(3))*0.5;
    q(1)=( R(1,3) -  R(3,1))/(4.0* q(3));
    q(2)=( R(2,1) +  R(1,2))/(4.0* q(3));
    q(4)=( R(3,2) +  R(2,3))/(4.0* q(3));
elseif( maxpos==4)
    q(4)=sqrt( tmp(4))*0.5;
    q(1)=( R(2,1) -  R(1,2))/(4.0* q(4));
    q(2)=( R(1,3) +  R(3,1))/(4.0* q(4));
    q(3)=( R(3,2) +  R(2,3))/(4.0* q(4));
else % should not happen
    display(['internal error in rotmat2quat\n']);
end

% enforce unit length
mag= q(1)* q(1) +  q(2)* q(2) +  q(3)* q(3) +  q(4)* q(4);

if( abs(mag-1.0)<0.0001)
    q=q;
else
    
    mag=1.0/sqrt( mag);
    q(1)=q(1)* mag;
    q(2)=q(2)* mag;
    q(3)=q(3)* mag;
    q(4)=q(4)* mag;
end


end