function [ x ] = PollefeyVisualwithPOLDTWOFRAMEFAMiter( F,w,h )
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


ux2=w/2;
ux1=w/2;

vy1=h/2;
vy2=h/2;
f1guess=w+h;
f2guess=w+h;

while 1
    clear G P P_in A b QZ S SL Q1 Q2 QS MS M K1 K2 w1 w2 ;
    G = normalizeFSturmTwoFrame( F,ux1,vy1,ux2,vy2,f1guess,f2guess );
    [ K_norm1, K_norm1_inverse ] = findNormalizingK(ux1,vy1,f1guess);
    [ K_norm2, K_norm2_inverse ] = findNormalizingK(ux2,vy2,f2guess);
    G
    P=PsfromF( G );
    [m n ]=size(P);
    numFrames=n;
    
    
    P_in=P;
    
    
    
    if(m>1)
        disp([ 'the size of the input cell is wrong']);
    end
    
    
    %now form A
    
    [A,b]  = formAunknownF( P_in );
    
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
   % S
    
    %%%%%%%%%%%%%%%%%%%%%
    
    
    MS=chooseFinalQ(QS);
    if(size(MS,2)==0)
        break;
    end
    k=1
    
    M=MS{1,k};
    
    w1=P_in{1,1}*M*(P_in{1,1}');
    w2=P_in{1,2}*M*(P_in{1,2}');
    w1=w1/w1(3,3);
    w2=w2/w2(3,3);
    
    K1=findKfromPQ(K_norm1,P_in{1,1},M);
    K2=findKfromPQ(K_norm2,P_in{1,2},M);
    
    f1=K1(1,1);
    f2=K2(1,1);
    
    
    
    
    
    ux1=K1(1,3);
    vy1=K1(2,3);
    f1guess=K1(1,1);
    
    ux2=K2(1,3);
    vy2=K2(2,3);
    f2guess=K2(1,1);
    
    
    
    x=[f1   f2]
    
    
end

end





