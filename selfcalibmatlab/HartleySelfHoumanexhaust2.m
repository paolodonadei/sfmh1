function [res , err ] = HartleySelfHoumanexhaust2( F,w,h, frangestart,frangeend,xystart, xyend )


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
curerrminer=10000;
currbestf1=1000;
currbestf2=1000;
currbestx=1000;
currbesty=1000;
x=[0 0];

K_norm1=zeros(3,3);
K_norm2=zeros(3,3);

err=zeros(xyend-xystart+1,frangeend-frangestart+1, frangeend-frangestart+1);
%ess=cell(xyend-xystart+1,frangeend-frangestart+1, frangeend-frangestart+1);

for xyguess= xystart:xyend
    for f1gs= frangestart:frangeend
        for f2gs= frangestart:frangeend
            
            K_norm1(1,1)=  f1gs;
            K_norm1(2,2)=  f1gs;
            K_norm1(3,3)=  1;
            K_norm1(1,3)= xyguess;
            K_norm1(2,3)= xyguess;
            
            
            K_norm2(1,1)= f2gs;
            K_norm2(2,2)= f2gs;
            K_norm2(3,3)=  1;
            K_norm2(3,1)= xyguess;
            K_norm2(3,2)= xyguess;
            
            
            
            count=count+1;
            G=(K_norm2)*F*K_norm1;
            
            
            S = svd(G);
            
            
            
            currEr=abs(S(1,1)-S(2,1))+abs(S(3,1));
            err(xyguess-(xystart-1),f1gs-(frangestart-1),f2gs-(frangestart-1))=currEr;
            RR=2*G*(G')*G- trace(G*(G'))*G;
           %  ess{xyguess-(xystart-1),f1gs-(frangestart-1),f2gs-(frangestart-1)}=RR;
            if(currEr<curerrminer)
                curerrminer=currEr;
                currbestf1=f1gs;
                currbestf2=f2gs;
                currbestx=xyguess;
                currbesty=xyguess;
            end
            
            
        end
    end
end
curerrminer
currbestf1
currbestf2
currbestx
currbesty
res=[
    curerrminer
    currbestf1
    currbestf2
    currbestx
    currbesty
    ];
end





