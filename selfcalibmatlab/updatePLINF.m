function [ NP ] = updatePLINF(G, Q )

[UF,SF,VF] = svd(G);
ep=UF(:,3);


[UQ,SQ,VQ] = svd(Q);

L=VQ(:,4);

L=L/L(4,1);

PL=zeros(3,1);

PL(1,1)=L(1,1); %plane at infinity
PL(2,1)=L(2,1);
PL(3,1)=L(3,1);

ADM=ep*(PL');




P1=zeros(3,4);
P2=zeros(3,4);


%P1
P1(1,1)=1;
P1(2,2)=1;
P1(3,3)=1;


%P2

P2 = [((-vgg_contreps(ep)*G)+ADM) ep];
x=P2(3,4);
P2=P2/x;


%check to see if teh found Ps are compatible with F
F_est = vgg_F_from_P(P1, P2);



R=sum(sum(abs(F_est-G)));

if(R>2)
    
    msg3=['the diff between your F and the real F is ' num2str(R)]
    
end

NP= {P1 P2};
end