function [ x, centerloc ] = PeterSturmSelf( F )
w=512;
h=512;
xc=256;
yc=256;
centerloc=[xc yc];
typicalF=5000;
G=normalizeFSturm(F,w,h,typicalF);
flagerr=0;

try
    [U,S,V] = svd(G);
catch
    disp(['we couldnt get the svd decomposition of w']);
    x=[0 0];
    S=zeros(3,3);
    U=zeros(3,3);
    V=zeros(3,3);
    flagerr=1;
end

if(flagerr==0)
    
    a=S(1,1);
    b=S(2,2);
    
    
    U31=U(3,1);
    U32=U(3,2);
    
    V31=V(3,1);
    V32=V(3,2);
    %quadratic:
    
    c3=a ^ 2 * (1 - U31 ^ 2) * (1 - V31 ^ 2) - b ^ 2 * (1 - U32 ^ 2) * (1 - V32 ^ 2);
    c2= a ^ 2 * (U31 ^ 2 + V31 ^ 2 - 2 * U31 ^ 2 * V31 ^ 2) - b ^ 2 * (U32 ^ 2 + V32 ^ 2 - 2 * U32 ^ 2 * V32 ^ 2);
    c1=a ^ 2 * U31 ^ 2 * V31 ^ 2 - b ^ 2 * U32 ^ 2 * V32 ^ 2;
    
    r=[c3 c2 c1];
    
    
    s = roots(r);
    
    r1=s(1,1);
    r2=s(2,1);
    
    
    f1=0;
    f2=0;
    
    if(r1>0)
        fq1=sqrt(r1)*typicalF;
        f1=fq1;
        f2=fq1;
        
    end
    
    
    if(r2>0)
        fq2=sqrt(r2)*typicalF;
        f1=fq2;
        f2=fq2;
    end
    
    %linear 1
    C1=(-U32 * V31 * (a * U31 * V31 + b * U32 * V32) / (a * U31 * U32 * (1 - V31 ^ 2) + b * V31 * V32 * (1 - U32 ^ 2)));
    
    if(C1>0)
        
        FL1 = sqrt(C1)*typicalF;
        
    end
    
    %linear 2
    C2=-U31 * V32 * (a * U31 * V31 + b * U32 * V32) / (a * V31 * V32 * (1 - U31 ^ 2) + b * U31 * U32 * (1 - V32 ^ 2));
    
    
    if(C2>0)
        
        FL2 = sqrt(C2)*typicalF;
        
    end
    
    x=[f1 f2];
    
end

end





