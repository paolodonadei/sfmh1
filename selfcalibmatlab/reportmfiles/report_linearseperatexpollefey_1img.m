function [ x ] = report_linearseperatexpollefey_1img( F,w,h,ks )
%code conforms to the marr prize paper but with full parametrization of the
%Q so we solve for the 10 variables
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here
%http://www.robots.ox.ac.uk/~vgg/hzbook/code/
%http://www.csse.uwa.edu.au/~pk/research/matlabfns/
% data:   http://www.robots.ox.ac.uk/~vgg/data/data-mview.html
f1=0;
f2=0;
x=[0 0];

%remove this
typicalF=ks{1}(1,1);
GG=normalizeFSturm(F,ks{1}(1,3)*2,ks{1}(2,3)*2,typicalF);
G = normalizeFSturmTwoFrame( F,ks{1}(1,3),ks{1}(2,3),ks{2}(1,3),ks{2}(2,3),ks{1}(1,1),ks{2}(1,1) );

K_norm=eye(3,3);


P=PsfromF( G );
[m n ]=size(P);
numFrames=n;


P_in=P;



if(m>1)
    disp([ 'the size of the input cell is wrong']);
end


%now form A

m=P_in{1};
n=P_in{2};

A = [(n(1,1) ^ 2) + (n(1,2) ^ 2) 2 * n(1,1) * n(1,4) 2 * n(1,4) * n(1,2) 2 * n(1,3) * n(1,4) n(1,4) ^ 2; n(2,1) ^ 2 + n(2,2) ^ 2 2 * n(2,4) * n(2,1) 2 * n(2,4) * n(2,2) 2 * n(2,3) * n(2,4) n(2,4) ^ 2; n(3,1) ^ 2 + n(3,2) ^ 2 2 * n(3,1) * n(3,4) 2 * n(3,2) * n(3,4) 2 * n(3,3) * n(3,4) n(3,4) ^ 2; n(2,1) * n(1,1) + n(2,2) * n(1,2) n(2,1) * n(1,4) + n(2,4) * n(1,1) n(2,4) * n(1,2) + n(2,2) * n(1,4) n(2,4) * n(1,3) + n(2,3) * n(1,4) n(2,4) * n(1,4); n(3,1) * n(2,1) + n(3,2) * n(2,2) n(3,1) * n(2,4) + n(3,4) * n(2,1) n(3,4) * n(2,2) + n(3,2) * n(2,4) n(3,4) * n(2,3) + n(3,3) * n(2,4) n(3,4) * n(2,4); n(3,1) * n(1,1) + n(3,2) * n(1,2) n(3,1) * n(1,4) + n(3,4) * n(1,1) n(3,4) * n(1,2) + n(3,2) * n(1,4) n(3,4) * n(1,3) + n(3,3) * n(1,4) n(3,4) * n(1,4); n(1,2) ^ 2 + n(1,1) ^ 2 - n(2,1) ^ 2 - n(2,2) ^ 2 2 * n(1,1) * n(1,4) - 2 * n(2,4) * n(2,1) 2 * n(1,4) * n(1,2) - 2 * n(2,4) * n(2,2) -2 * n(2,3) * n(2,4) + 2 * n(1,3) * n(1,4) n(1,4) ^ 2 - n(2,4) ^ 2; n(1,2) ^ 2 + n(1,1) ^ 2 - n(3,1) ^ 2 - n(3,2) ^ 2 2 * n(1,1) * n(1,4) - 2 * n(3,1) * n(3,4) 2 * n(1,4) * n(1,2) - 2 * n(3,2) * n(3,4) -2 * n(3,3) * n(3,4) + 2 * n(1,3) * n(1,4) n(1,4) ^ 2 - n(3,4) ^ 2; n(2,2) ^ 2 + n(2,1) ^ 2 - n(3,1) ^ 2 - n(3,2) ^ 2 -2 * n(3,1) * n(3,4) + 2 * n(2,4) * n(2,1) -2 * n(3,2) * n(3,4) + 2 * n(2,4) * n(2,2) 2 * n(2,3) * n(2,4) - 2 * n(3,3) * n(3,4) n(2,4) ^ 2 - n(3,4) ^ 2;];



b = [1 - n(1,3) ^ 2 1 - n(2,3) ^ 2 1 - n(3,3) ^ 2 -n(2,3) * n(1,3) -n(3,3) * n(2,3) -n(3,3) * n(1,3) n(2,3) ^ 2 - n(1,3) ^ 2 n(3,3) ^ 2 - n(1,3) ^ 2 -n(2,3) ^ 2 + n(3,3) ^ 2];
b=b';

c = regress(b,A);
%cr = robustfit(b,A);


Q = [c(1) 0 0 c(2); 0 c(1) 0 c(3); 0 0 1 c(4); c(2) c(3) c(4) c(5);];

% for i=1:size(b,1)
%     if(b(i,1)==1)
%         A(i,:)=zeros(1,size(A,2));
%         b(i,1)=0;
%     end
% end

[QZ]=QsfromAb(A,b);



S=findSolsfromQ(QZ);

if(size(QZ,2)>1)
    Q1=QZ{1,1};
    Q2=QZ{1,2};
else
    Q1=QZ{1,1};
    Q2=zeros(4,4);
end


for i=1:size(S,1)
    QS{1,i}=normalizeSetRank(Q1+ S(i,1)*Q2);
    
end
%S

%%%%%%%%%%%%%%%%%%%%%


MS=chooseFinalQ(QS);


if(size(MS,2)==0)
    M=eye(4,4);
    
else
    
    M=MS{1,1};
    
end

K1=findKfromPQ(K_norm,P_in{1,1},M);
K2=findKfromPQ(K_norm,P_in{1,2},M);

f1=K1(1,1)*typicalF;
f2=K2(1,1)*typicalF;



x=[f1   f2];



end





