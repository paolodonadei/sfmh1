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
numFrames=2;

ux2=w/2;
ux1=w/2;

vy1=h/2;
vy2=h/2;
f1guess=w+h;
f2guess=w+h;
x=[0 0];
absdiff= 500000000;
diffprev=5000000000;
count=0;
M=eye(4,4);
while absdiff>0.000005 && count<400 &&  (diffprev-absdiff)>0
    
    count=count+1;
    clear G P P_in A b QZ S SL Q1 Q2 QS MS  K1 K2 w1 w2 ;
    G = normalizeFSturmTwoFrame( F,ux1,vy1,ux2,vy2,f1guess,f2guess );
    
    
    P_in=PsfromF( G );
    
    [A,b]  = formAunknownF( P_in );
    
    [Ua,Sa,Va] = svd(A);
    rank(A);
    Sa(8,8);
    
    [QZ]=QsfromAb(A,b);
    
    
    
    SL=findSolsfromQ(QZ);
    
    if(size(QZ,2)>1)
        Q1=QZ{1,1};
        Q2=QZ{1,2};
    else
        Q1=QZ{1,1};
        Q2=zeros(4,4);
    end
    
    for i=1:size(SL,1)
        QS{1,i}=normalizeSetRank(Q1+ SL(i,1)*Q2);
        
    end
    
    
    %%%%%%%%%%%%%%%%%%%%%
    
    MS=chooseFinalQ(QS);
    
    if(size(MS,2)==0)
        % i think i ought to add gaussian noise just to the first camera
        % maybeor the secondd
        f1guess=f1guess+0.2*randn();
        f2guess=f1guess+0.2*randn();
        %         ux1=ux1+2*randn() ;
        %         vy1=vy1 +2*randn();
        %         ux2= ux2+2*randn();
        %         vy2=vy2 +2*randn();
        
       
    else
        
        
        M=MS{1,1};
        
        
        [ K_norm2, K_norm2_inverse ] = findNormalizingK(ux2,vy2,f2guess);
        [ K_norm1, K_norm1_inverse ] = findNormalizingK(ux1,vy1,f1guess);
        
        K2=findKfromPQ(K_norm2,P_in{1,2},M);
        K1=findKfromPQ(K_norm1,P_in{1,1},M);
        
        % we just update camera 2
        f2=K2(1,1);
        f1=K1(1,1);
        
        %     ux2= K2(1,3); %this is not working well
        %     vy2=K2(2,3) ;
        
        
        diffprev=absdiff;
        fdiff=abs(f2guess-f2)+abs(f1guess-f1);
        absdiff=fdiff;
        
        %   disp(['difference is ' num2str(absdiff)]);
        
        
        f2guess=K2(1,1);
        f1guess=K1(1,1);
    end
    
    %swap
    F=F';
    
    %     uxtemp=ux1;
    %     ux1=ux2;
    %     ux2=uxtemp;
    %
    %     vytemp=vy1;
    %     vy1=vy2;
    %     vy2=vytemp;
    
    
    fguesstemp=f1guess;
    f1guess=f2guess;
    f2guess=fguesstemp;
    
    ftemp=f1;
    f1=f2;
    f2=ftemp;
    
    
    x=[f1   f2];
    Q=M;
    
end
x=[f1   f2];

end
