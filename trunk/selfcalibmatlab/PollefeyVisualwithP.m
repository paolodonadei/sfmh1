function [ output_args ] = PollefeyVisualwithP( P,w,h )
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here
%http://www.robots.ox.ac.uk/~vgg/hzbook/code/
%http://www.csse.uwa.edu.au/~pk/research/matlabfns/
% data:   http://www.robots.ox.ac.uk/~vgg/data/data-mview.html

[m n ]=size(P);
numFrames=n;
numRowsperFrame=6; %this is how many equatiosn we get per frame, or how many linear constraints per picture
%how many times repeat the estimation
numIterations =5;


%copy the Ps to new P
P_in=P;

disp([ 'the number of frames is ' num2str(numFrames)]);

if(m>1)
    disp([ 'the size of the input cell is wrong']);
end

[ K_norm, K_norm_inv ] = findNormalizingK(w,h);


%perform normalization
for i=1:numFrames
    P_in{1,i}=K_norm_inv*P_in{1,i};
  %  P_in{1,i}=P_in{1,i}/norm(P_in{1,i},'fro');
end


%now form A


A=zeros(numFrames*numRowsperFrame,10);

for i=1:numFrames
    m=   P_in{1,i};% to make it simpler we assign it

    A(1+((i-1)*numRowsperFrame) , 1) =  (Pow(m(1 , 1), 2.0) - Pow(m(3 , 1), 2.0));
    A(1+((i-1)*numRowsperFrame), 2)  =  (-2.0 * m(3 , 1) * m(3 , 2) + 2.0 * m(1 , 1) * m(1 , 2));
    A(1+((i-1)*numRowsperFrame) , 3) =  (2.0 * m(1 , 1) * m(1 , 3) - 2.0 * m(3 , 1) * m(3 , 3));
    A(1+((i-1)*numRowsperFrame), 4)  =  (2.0 * m(1 , 1) * m(1 , 4) - 2.0 * m(3 , 4) * m(3 , 1));
    A(1+((i-1)*numRowsperFrame) , 5) =  (Pow(m(1 , 2), 2.0) - Pow(m(3 , 2), 2.0));
    A(1+((i-1)*numRowsperFrame) , 6) =  (2.0 * m(1 , 2) * m(1 , 3) - 2.0 * m(3 , 3) * m(3 , 2));
    A(1+((i-1)*numRowsperFrame) , 7) =  (2.0 * m(1 , 4) * m(1 , 2) - 2.0 * m(3 , 4) * m(3 , 2));
    A(1+((i-1)*numRowsperFrame) , 8) =  (-Pow(m(3 , 3), 2.0) + Pow(m(1 , 3), 2.0));
    A(1+((i-1)*numRowsperFrame) , 9) =  (-2.0 * m(3 , 3) * m(3 , 4) + 2.0 * m(1 , 3) * m(1 , 4));
    A(1+((i-1)*numRowsperFrame) , 10)= (Pow(m(1 , 4), 2.0) - Pow(m(3 , 4), 2.0));
    A(2+((i-1)*numRowsperFrame) , 1) =  (Pow(m(2 , 1), 2.0) - Pow(m(3 , 1), 2.0));
    A(2+((i-1)*numRowsperFrame) , 2) =  (-2.0 * m(3 , 1) * m(3 , 2) + 2.0 * m(2 , 1) * m(2 , 2));
    A(2+((i-1)*numRowsperFrame) , 3) =  (2.0 * m(2 , 1) * m(2 , 3) - 2.0 * m(3 , 1) * m(3 , 3));
    A(2+((i-1)*numRowsperFrame) , 4) =  (2.0 * m(2 , 1) * m(2 , 4) - 2.0 * m(3 , 4) * m(3 , 1));
    A(2+((i-1)*numRowsperFrame), 5)  =  (Pow(m(2 , 2), 2.0) - Pow(m(3 , 2), 2.0));
    A(2+((i-1)*numRowsperFrame) , 6) =  (2.0 * m(2 , 2) * m(2 , 3) - 2.0 * m(3 , 3) * m(3 , 2));
    A(2+((i-1)*numRowsperFrame) , 7) =  (2.0 * m(2 , 4) * m(2 , 2) - 2.0 * m(3 , 4) * m(3 , 2));
    A(2+((i-1)*numRowsperFrame) , 8) =  (-Pow(m(3 , 3), 2.0) + Pow(m(2 , 3), 2.0));
    A(2+((i-1)*numRowsperFrame) , 9) =  (-2.0 * m(3 , 3) * m(3 , 4) + 2.0 * m(2 , 3) * m(2 , 4));
    A(2+((i-1)*numRowsperFrame) , 10)= (Pow(m(2 , 4), 2.0) - Pow(m(3 , 4), 2.0));
    A(3+((i-1)*numRowsperFrame), 1)  =  (Pow(m(1 , 1), 2.0) - Pow(m(2 , 1), 2.0));
    A(3+((i-1)*numRowsperFrame) , 2) =  (-2.0 * m(2 , 1) * m(2 , 2) + 2.0 * m(1 , 1) * m(1 , 2));
    A(3+((i-1)*numRowsperFrame), 3)  =  (2.0 * m(1 , 1) * m(1 , 3) - 2.0 * m(2 , 1) * m(2 , 3));
    A(3+((i-1)*numRowsperFrame) , 4) =  (2.0 * m(1 , 1) * m(1 , 4) - 2.0 * m(2 , 1) * m(2 , 4));
    A(3+((i-1)*numRowsperFrame) , 5) =  (Pow(m(1 , 2), 2.0) - Pow(m(2 , 2), 2.0));
    A(3+((i-1)*numRowsperFrame) , 6) =  (2.0 * m(1 , 2) * m(1 , 3) - 2.0 * m(2 , 2) * m(2 , 3));
    A(3+((i-1)*numRowsperFrame), 7)  =  (2.0 * m(1 , 4) * m(1 , 2) - 2.0 * m(2 , 4) * m(2 , 2));
    A(3+((i-1)*numRowsperFrame) , 8) =  (-Pow(m(2 , 3), 2.0) + Pow(m(1 , 3), 2.0));
    A(3+((i-1)*numRowsperFrame) , 9) =  (-2.0 * m(2 , 3) * m(2 , 4) + 2.0 * m(1 , 3) * m(1 , 4));
    A(3+((i-1)*numRowsperFrame) , 10)= (Pow(m(1 , 4), 2.0) - Pow(m(2 , 4), 2.0));
    A(4+((i-1)*numRowsperFrame) , 1) =  m(2 , 1) * m(1 , 1);
    A(4+((i-1)*numRowsperFrame), 2)  =  (m(2 , 2) * m(1 , 1) + m(2 , 1) * m(1 , 2));
    A(4+((i-1)*numRowsperFrame) , 3) =  (m(2 , 1) * m(1 , 3) + m(2 , 3) * m(1 , 1));
    A(4+((i-1)*numRowsperFrame), 4)  =  (m(2 , 1) * m(1 , 4) + m(2 , 4) * m(1 , 1));
    A(4+((i-1)*numRowsperFrame) , 5) =   m(2 , 2) * m(1 , 2);
    A(4+((i-1)*numRowsperFrame) , 6) =  (m(2 , 3) * m(1 , 2) + m(2 , 2) * m(1 , 3));
    A(4+((i-1)*numRowsperFrame) , 7) =  (m(2 , 4) * m(1 , 2) + m(2 , 2) * m(1 , 4));
    A(4+((i-1)*numRowsperFrame) , 8) =   m(2 , 3) * m(1 , 3);
    A(4+((i-1)*numRowsperFrame) , 9) =  (m(2 , 4) * m(1 , 3) + m(2 , 3) * m(1 , 4));
    A(4+((i-1)*numRowsperFrame), 10) =  m(2 , 4) * m(1 , 4);
    A(5+((i-1)*numRowsperFrame), 1)  =   m(3 , 1) * m(2 , 1);
    A(5+((i-1)*numRowsperFrame), 2)  =  (m(3 , 2) * m(2 , 1) + m(3 , 1) * m(2 , 2));
    A(5+((i-1)*numRowsperFrame) , 3) =  (m(3 , 1) * m(2 , 3) + m(3 , 3) * m(2 , 1));
    A(5+((i-1)*numRowsperFrame) , 4) =  (m(3 , 1) * m(2 , 4) + m(3 , 4) * m(2 , 1));
    A(5+((i-1)*numRowsperFrame) , 5) =  m(3 , 2) * m(2 , 2);
    A(5+((i-1)*numRowsperFrame) , 6) =  (m(3 , 3) * m(2 , 2) + m(3 , 2) * m(2 , 3));
    A(5+((i-1)*numRowsperFrame), 7)  =  (m(3 , 4) * m(2 , 2) + m(3 , 2) * m(2 , 4));
    A(5+((i-1)*numRowsperFrame), 8)  =  m(3 , 3) * m(2 , 3);
    A(5+((i-1)*numRowsperFrame) , 9) =  (m(3 , 4) * m(2 , 3) + m(3 , 3) * m(2 , 4));
    A(5+((i-1)*numRowsperFrame) , 10)=  m(3 , 4) * m(2 , 4);
    A(6+((i-1)*numRowsperFrame) , 1) =   m(3 , 1) * m(1 , 1);
    A(6+((i-1)*numRowsperFrame) , 2) =  (m(3 , 2) * m(1 , 1) + m(3 , 1) * m(1 , 2));
    A(6+((i-1)*numRowsperFrame), 3)  =  (m(3 , 1) * m(1 , 3) + m(3 , 3) * m(1 , 1));
    A(6+((i-1)*numRowsperFrame), 4)  =  (m(3 , 1) * m(1 , 4) + m(3 , 4) * m(1 , 1));
    A(6+((i-1)*numRowsperFrame) , 5) =   m(3 , 2) * m(1 , 2);
    A(6 +((i-1)*numRowsperFrame), 6) =  (m(3 , 3) * m(1 , 2) + m(3 , 2) * m(1 , 3));
    A(6+((i-1)*numRowsperFrame) , 7) =  (m(3 , 4) * m(1 , 2) + m(3 , 2) * m(1 , 4));
    A(6+((i-1)*numRowsperFrame) , 8) =   m(3 , 3) * m(1 , 3);
    A(6+((i-1)*numRowsperFrame), 9)  =  (m(3 , 4) * m(1 , 3) + m(3 , 3) * m(1 , 4));
    A(6+((i-1)*numRowsperFrame) , 10)=  m(3 , 4) * m(1 , 4);
end



omega=1;




%use this for recalculating the omega, the paper doesnt exactly show which
%P has to be used for recalculating this, or whether or not this is a
%normalized P, i used frame 1 P and it is normalized
P_F1=P_in{1,1};

for j=1:numIterations

    AC=zeros(numFrames*numRowsperFrame,10);
    AC=A;


    for i=1:numFrames
        AC(1+((i-1)*numRowsperFrame),:)=AC(1+((i-1)*numRowsperFrame),:)*(1/(9*omega));
        AC(2+((i-1)*numRowsperFrame),:)=AC(2+((i-1)*numRowsperFrame),:)*(1/(9*omega));
        AC(3+((i-1)*numRowsperFrame),:)=AC(3+((i-1)*numRowsperFrame),:)*(1/(0.2*omega));
        AC(4+((i-1)*numRowsperFrame),:)=AC(4+((i-1)*numRowsperFrame),:)*(1/(0.01*omega));%changed this, i think paper is wrong
        AC(5+((i-1)*numRowsperFrame),:)=AC(5+((i-1)*numRowsperFrame),:)*(1/(0.1*omega));
        AC(6+((i-1)*numRowsperFrame),:)=AC(6+((i-1)*numRowsperFrame),:)*(1/(0.1*omega));
    end

    
    [U,S,V] = svd(AC);
 
    disp([ 'last svd is ' num2str(S(10,10)) ' and the condition number is (it should be near 1) ' num2str(cond(AC)) ' and the rank is ' num2str(rank(AC))] );
    Q=findQfromV(V);


    %now recalculate omega according to polleyfey
    omega=P_F1(3,:)*Q*(P_F1(3,:)');
    disp(['at iteration ' num2str(j) ' omega is ' num2str(omega)]);

    for i=1:numFrames
        K=findKfromPQ(K_norm,P_in{1,i},Q);
        disp([' the focal length is ' num2str(K(1,1)) ' or ' num2str(K(2,2))]);
    end
end



output_args=1;



end





