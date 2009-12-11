fzzunction [ x ] = HartleySelfHouman( F,w,h )


ux2=w/2;
ux1=w/2;

vy1=h/2;
vy2=h/2;
f1guess=5000;
f2guess=5000;

count=0;
S=eye(3,3);
S(1,1)=1000;

SOLD=eye(3,3);
SOLD(1,1)=10000;


fsol1=0;
fsol2=0;
%while abs(S(1,1)-S(2,2))> 0.0000005 && abs(S(1,1)-S(2,2))< abs(SOLD(1,1)-SOLD(2,2))
while count<10
count=count+1;
    G = normalizeFSturmTwoFrame( F,ux1,vy1,ux2,vy2,f1guess,f2guess );
    
    SOLD=S;
    [U,S,V] = svd(G);
    eig(G)
    S
    
    a=S(1,1);
    b=S(2,2);
    
    
    u31=U(3,1);
    u32=U(3,2);
    u21=U(2,1);
    u12=U(1,2);
    u11=U(1,1);
    u22=U(2,2);
    
    
    v31=V(3,1);
    v32=V(3,2);
    v21=V(2,1);
    v12=V(1,2);
    v11=V(1,1);
    v22=V(2,2);
    
    
    
    f1=0;
    f2=0;
    
    
    %quadratic:
    cg0 = -(a ^ 2 * v32 * v31 * u31 ^ 2 * v21 ^ 2 + a ^ 2 * v32 * v31 * u31 ^ 2 * v11 ^ 2 - a ^ 2 * v22 * v21 * u31 ^ 2 * v31 ^ 2 - a ^ 2 * v12 * v11 * u31 ^ 2 * v31 ^ 2 + u31 * a * v11 ^ 2 * b * v32 ^ 2 * u32 - u31 * a * v31 ^ 2 * b * v12 ^ 2 * u32 + u31 * a * v21 ^ 2 * b * v32 ^ 2 * u32 - u31 * a * v31 ^ 2 * b * v22 ^ 2 * u32 - b ^ 2 * v22 ^ 2 * u32 ^ 2 * v32 * v31 + b ^ 2 * v32 ^ 2 * u32 ^ 2 * v22 * v21 - b ^ 2 * v12 ^ 2 * u32 ^ 2 * v32 * v31 + b ^ 2 * v32 ^ 2 * u32 ^ 2 * v12 * v11) / (u11 ^ 2 * a ^ 2 * v32 * v31 * v21 ^ 2 + u11 ^ 2 * a ^ 2 * v32 * v31 * v11 ^ 2 - u11 ^ 2 * a ^ 2 * v22 * v21 * v31 ^ 2 - u11 ^ 2 * a ^ 2 * v12 * v11 * v31 ^ 2 + u11 * u12 * a * v11 ^ 2 * b * v32 ^ 2 - u11 * u12 * a * v31 ^ 2 * b * v12 ^ 2 + u11 * u12 * a * v21 ^ 2 * b * v32 ^ 2 - u11 * u12 * a * v31 ^ 2 * b * v22 ^ 2 + u12 ^ 2 * b ^ 2 * v12 * v11 * v32 ^ 2 - u12 ^ 2 * b ^ 2 * v32 * v31 * v12 ^ 2 - u12 ^ 2 * b ^ 2 * v32 * v31 * v22 ^ 2 + u12 ^ 2 * b ^ 2 * v22 * v21 * v32 ^ 2 - a * u22 * u21 * v31 ^ 2 * b * v12 ^ 2 + a * u22 * u21 * v11 ^ 2 * b * v32 ^ 2 + v32 * v31 * a ^ 2 * u21 ^ 2 * v11 ^ 2 + a * u22 * u21 * v21 ^ 2 * b * v32 ^ 2 + v32 * v31 * a ^ 2 * u21 ^ 2 * v21 ^ 2 - a * u22 * u21 * v31 ^ 2 * b * v22 ^ 2 - v22 * v21 * a ^ 2 * u21 ^ 2 * v31 ^ 2 - v12 * v11 * a ^ 2 * u21 ^ 2 * v31 ^ 2 - b ^ 2 * v32 * v31 * v12 ^ 2 * u22 ^ 2 + b ^ 2 * v12 * v11 * v32 ^ 2 * u22 ^ 2 + b ^ 2 * v22 * v21 * v32 ^ 2 * u22 ^ 2 - b ^ 2 * v32 * v31 * v22 ^ 2 * u22 ^ 2);
    cg1 = -(-a ^ 2 * u31 ^ 2 * u22 * u21 * v31 ^ 2 - a ^ 2 * u31 ^ 2 * u12 * u11 * v31 ^ 2 + b ^ 2 * u22 * u21 * v32 ^ 2 * u32 ^ 2 - b ^ 2 * u32 * u31 * v32 ^ 2 * u22 ^ 2 - a * u31 ^ 2 * b * u12 ^ 2 * v32 * v31 + b ^ 2 * u12 * u11 * v32 ^ 2 * u32 ^ 2 + u32 * a ^ 2 * u21 ^ 2 * u31 * v31 ^ 2 + u32 * a ^ 2 * u11 ^ 2 * u31 * v31 ^ 2 - a * u31 ^ 2 * b * u22 ^ 2 * v32 * v31 + a * u21 ^ 2 * b * u32 ^ 2 * v32 * v31 + a * u11 ^ 2 * b * u32 ^ 2 * v32 * v31 - b ^ 2 * u32 * u31 * v32 ^ 2 * u12 ^ 2) / (u32 * a ^ 2 * u11 ^ 2 * u31 * v11 ^ 2 - a ^ 2 * u31 ^ 2 * u22 * u21 * v11 ^ 2 - a ^ 2 * u31 ^ 2 * u12 * u11 * v11 ^ 2 + u32 * a ^ 2 * u21 ^ 2 * u31 * v11 ^ 2 - a * v12 * v11 * u31 ^ 2 * b * u12 ^ 2 + a * v12 * v11 * u11 ^ 2 * b * u32 ^ 2 + a * v12 * v11 * u21 ^ 2 * b * u32 ^ 2 - a * v12 * v11 * u31 ^ 2 * b * u22 ^ 2 - b ^ 2 * u32 * u31 * v12 ^ 2 * u22 ^ 2 + b ^ 2 * u12 * u11 * v12 ^ 2 * u32 ^ 2 + b ^ 2 * u22 * u21 * v12 ^ 2 * u32 ^ 2 - b ^ 2 * u32 * u31 * v12 ^ 2 * u12 ^ 2 - b ^ 2 * u32 * u31 * v22 ^ 2 * u12 ^ 2 - a * v22 * v21 * u31 ^ 2 * b * u12 ^ 2 - a * v22 * v21 * u31 ^ 2 * b * u22 ^ 2 + u32 * a ^ 2 * u21 ^ 2 * u31 * v21 ^ 2 - b ^ 2 * u32 * u31 * v22 ^ 2 * u22 ^ 2 + b ^ 2 * u12 * u11 * v22 ^ 2 * u32 ^ 2 + b ^ 2 * u22 * u21 * v22 ^ 2 * u32 ^ 2 + a * v22 * v21 * u21 ^ 2 * b * u32 ^ 2 - a ^ 2 * u31 ^ 2 * u22 * u21 * v21 ^ 2 + u32 * a ^ 2 * u11 ^ 2 * u31 * v21 ^ 2 - a ^ 2 * u31 ^ 2 * u12 * u11 * v21 ^ 2 + a * v22 * v21 * u11 ^ 2 * b * u32 ^ 2);
    
    
    fsol1=f1guess;
    fsol2=f2guess;
    if(cg0>0)
        
        f1=sqrt(cg0);
        f1=f1*f1guess;
        f1guess=f1;
        
    end
    
    
    
    if(cg1>0)
        
        f2=sqrt(cg1);
        f2=f2*f2guess;
        f2guess=f2;
    end;
    
    disp(['f1 is ' num2str(f1guess) ' and f2 is ' num2str(f2guess) ' for count ' num2str(count)]);
    abs(S(1,1)-S(2,2))
    
    
end

x=[fsol2 fsol1];

end





