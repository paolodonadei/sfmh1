function [ output_args ] = PollefeyVisual( F )
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here
%http://www.robots.ox.ac.uk/~vgg/hzbook/code/
%http://www.csse.uwa.edu.au/~pk/research/matlabfns/

P1=zeros(3,4);
P2=zeros(3,4);


%P1
P1(1,1)=1;
P1(2,2)=1;
P1(3,3)=1;


%P2
[UF,SF,VF] = svd(F)
ep=UF(:,3)
P2 = [-vgg_contreps(ep)*F ep];
x=P2(3,4);
P2=P2/x;


%check to see if teh found Ps are compatible with F
F_est = vgg_F_from_P(P1, P2);

F_est
F

R=sum(sum(abs(F_est-F)))

msg3=['the diff between your F and the real F is ' num2str(R)]

w=512;
h=512;

output_args  = PollefeyVisualwithP( {P1 P2},w,h );
end





