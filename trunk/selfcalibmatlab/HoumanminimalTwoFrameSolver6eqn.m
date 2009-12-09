function [ x] = HoumanminimalTwoFrameSolver6eqn( F,w,h )

%normFORP , if this is set to 1, we normalize F and then we get the Ps in
%the canonic position, if its zero we normalize the Ps in which case they
%wont be in the canonical position, read the code to see how this works


%create the guess Ks


ux2=w/2;
ux1=w/2;

vy1=h/2;
vy2=h/2;
f1guess=w+h;
f2guess=w+h;

r=0;
count=0;
absdiff= 500000000;
diffprev=5000000000;
x=[0   0];
f1=0;
f2=0;
K1=eye(3,3);
K2=eye(3,3);
Q=eye(4,4);
M=eye(4,4);
l=[0; 0 ; 0 ] ; %plane at infinity
while absdiff>0.000005 && count<400 &&  diffprev>absdiff
    

    
    count=count+1;
    
    %    disp(['count is ' num2str(count)]);
    
    clear G A P P_in U S V c d Q1 Q2  QZ S QS MS  ;
    G = normalizeFSturmTwoFrame( F,ux1,vy1,ux2,vy2,f1guess,f2guess );
    
    
    P_in=PsfromF( G );
    
    
    n=P_in{1,2};
    
    
    %with 4 equations only, q(3,3)=c1
    %A = [-n(2,1) ^ 2 - n(2,2) ^ 2 - n(2,3) ^ 2 + n(1,3) ^ 2 + n(1,1) ^ 2 + n(1,2) ^ 2 2 * n(1,1) * n(1,4) - 2 * n(2,4) * n(2,1) 2 * n(1,4) * n(1,2) - 2 * n(2,4) * n(2,2) -2 * n(2,3) * n(2,4) + 2 * n(1,3) * n(1,4) n(1,4) ^ 2 - n(2,4) ^ 2; n(2,1) * n(1,1) + n(2,3) * n(1,3) + n(2,2) * n(1,2) n(2,1) * n(1,4) + n(2,4) * n(1,1) n(2,4) * n(1,2) + n(2,2) * n(1,4) n(2,4) * n(1,3) + n(2,3) * n(1,4) n(2,4) * n(1,4); n(3,1) * n(2,1) + n(3,3) * n(2,3) + n(3,2) * n(2,2) n(3,1) * n(2,4) + n(3,4) * n(2,1) n(3,4) * n(2,2) + n(3,2) * n(2,4) n(3,4) * n(2,3) + n(3,3) * n(2,4) n(3,4) * n(2,4); n(3,1) * n(1,1) + n(3,3) * n(1,3) + n(3,2) * n(1,2) n(3,1) * n(1,4) + n(3,4) * n(1,1) n(3,4) * n(1,2) + n(3,2) * n(1,4) n(3,4) * n(1,3) + n(3,3) * n(1,4) n(3,4) * n(1,4);];;
    
    %with 6 equations
    alpha=1/(abs(r*10)+1);
    
    A = [-(n(2,1) ^ 2) - (n(2,2) ^ 2) - (n(2,3) ^ 2) + (n(1,3) ^ 2) + (n(1,1) ^ 2) + (n(1,2) ^ 2) 2 * n(1,1) * n(1,4) - 2 * n(2,4) * n(2,1) 2 * n(1,4) * n(1,2) - 2 * n(2,4) * n(2,2) -2 * n(2,3) * n(2,4) + 2 * n(1,3) * n(1,4) n(1,4) ^ 2 - n(2,4) ^ 2; alpha * (-n(3,1) ^ 2 - n(3,2) ^ 2 - n(3,3) ^ 2 + n(2,3) ^ 2 + n(2,1) ^ 2 + n(2,2) ^ 2) alpha * (-2 * n(3,1) * n(3,4) + 2 * n(2,4) * n(2,1)) alpha * (-2 * n(3,2) * n(3,4) + 2 * n(2,4) * n(2,2)) alpha * (2 * n(2,3) * n(2,4) - 2 * n(3,3) * n(3,4)) alpha * (n(2,4) ^ 2 - n(3,4) ^ 2); alpha * (-n(1,1) ^ 2 - n(1,2) ^ 2 - n(1,3) ^ 2 + n(3,3) ^ 2 + n(3,1) ^ 2 + n(3,2) ^ 2) alpha * (-2 * n(1,1) * n(1,4) + 2 * n(3,1) * n(3,4)) alpha * (-2 * n(1,4) * n(1,2) + 2 * n(3,2) * n(3,4)) alpha * (2 * n(3,3) * n(3,4) - 2 * n(1,3) * n(1,4)) alpha * (n(3,4) ^ 2 - n(1,4) ^ 2); n(2,1) * n(1,1) + n(2,3) * n(1,3) + n(2,2) * n(1,2) n(2,1) * n(1,4) + n(2,4) * n(1,1) n(2,4) * n(1,2) + n(2,2) * n(1,4) n(2,4) * n(1,3) + n(2,3) * n(1,4) n(2,4) * n(1,4); n(3,1) * n(2,1) + n(3,3) * n(2,3) + n(3,2) * n(2,2) n(3,1) * n(2,4) + n(3,4) * n(2,1) n(3,4) * n(2,2) + n(3,2) * n(2,4) n(3,4) * n(2,3) + n(3,3) * n(2,4) n(3,4) * n(2,4); n(3,1) * n(1,1) + n(3,3) * n(1,3) + n(3,2) * n(1,2) n(3,1) * n(1,4) + n(3,4) * n(1,1) n(3,4) * n(1,2) + n(3,2) * n(1,4) n(3,4) * n(1,3) + n(3,3) * n(1,4) n(3,4) * n(1,4);];
    
    
    
    %   disp(['rank is ' num2str(rank(A))]);
    
    
    [U,S,V] = svd(A);
    r=S(4,4);
    
    
    
    
    Q1=zeros(4,4);
    Q2=zeros(4,4);
    
    %
    %no b
    c=V(:,5);
    d=V(:,4);
    
    
    
    % Q(3,3) is c1
    Q1 = [c(1,1) 0 0 c(2,1); 0 c(1,1) 0 c(3,1); 0 0 c(1,1) c(4,1); c(2,1) c(3,1) c(4,1) c(5,1);];
    Q2 = [d(1,1) 0 0 d(2,1); 0 d(1,1) 0 d(3,1); 0 0 d(1,1) d(4,1); d(2,1) d(3,1) d(4,1) d(5,1);];
    
    
    %full Q
    QZ={Q1 Q2};
    
    
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
    %SL
    
    %%%%%%%%%%%%%%%%%%%%%
    
    MS=chooseFinalQ(QS);
    
    if(size(MS,2)==0)
        % i think i ought to add gaussian noise just to the first camera
        % maybeor the secondd
        f1guess=f1guess+0.1*randn();
        %f2guess=f1guess+0.2*randn();
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
