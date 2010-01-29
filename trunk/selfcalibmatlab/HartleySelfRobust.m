function [ v ,  centerloc] = HartleySelfRobust(ps,w,h,pnum)
if (nargin == 1)
    w=512;
    h=512;
    pnum=1;
end
if (nargin == 3)
    pnum=1;
end


[m,n]=size(ps);
number=n;

if(pnum>number)
    disp(['p number is larger than the largest p number, this is incorrect']);
end

P1=ps{pnum};


count=0;
for i=1:number
    if(i~=pnum)
        count=count+1;
        TF{count}=vgg_F_from_P(P1,ps{1,i});
    end
end
count=0;


centerloc=[256 256 256 256];
typicalF=5000;


[m,n]=size(TF);

numFs=n;

solvector=zeros(numFs,2);

for i=1:numFs
    clear G S U V a b f1 f2 cg0 cg1 x;
    G=normalizeFSturm(TF{1,i},w,h,typicalF);

    [U,S,V] = svd(G);

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


    if(cg0>0)

        f1=sqrt(cg0);
        f1=f1*typicalF;

    end



    if(cg1>0)

        f2=sqrt(cg1);
        f2=f2*typicalF;

    end;

    x=[f2 f1];

    solvector(i,1)= x(1,1) ;
    solvector(i,2)= x(1,2) ;
end


vm=median(solvector);
v=vm(1,1);
end





