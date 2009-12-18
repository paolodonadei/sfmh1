function [x ] = HartleySelfHoumanexhaustsameK( F,w,h )


ux=w/2;
vy=h/2;
fguess=w+h;

frangestart=400;
frangeend=1400;
xstart=200;
xend=400;

ystart=200;
yend=400;

count=0;

fsol1=0;
fsol2=0;
%while abs(S(1,1)-S(2,2))> 0.0000005 && abs(S(1,1)-S(2,2))< abs(SOLD(1,1)-SOLD(2,2))
curerrminer=10000;
currbestf=1000;
currbestx=1000;
currbesty=1000;
x=[0 0];

K_norm1=zeros(3,3);
K_norm2=zeros(3,3);

Ginter=zeros(3,3);
err=zeros(frangeend-frangestart+1, xend-xstart+1,yend-ystart+1);
%ess=cell(xyend-xystart+1,frangeend-frangestart+1, frangeend-frangestart+1);
trak=0;
F=F*1;


myfstep=1;
xstep=1;
ystep=1;

% while(myfstep>0.4)
    
    [z]=PeterSturmSelf(F,512,512);
     frangestart=z(1)-200;
    frangeend=z(2)+200;
    
    for fgs= frangestart:myfstep:frangeend
         fgs
        % myfstep
        for xguess= xstart:xstep:xend
            for yguess= ystart:ystep:yend
                
                
                K_norm1(1,1)=  fgs;
                K_norm1(2,2)=  fgs;
                K_norm1(3,3)=  1;
                K_norm1(1,3)= xguess;
                K_norm1(2,3)= yguess;
                
                
                K_norm2(1,1)= fgs;
                K_norm2(2,2)= fgs;
                K_norm2(3,3)=  1;
                K_norm2(3,1)= xguess;
                K_norm2(3,2)= yguess;
                
                
                
                count=count+1;
                G=(K_norm2)*F*K_norm1;
                
                Ginter=G*(G');
                trak=Ginter(1,1)+Ginter(2,2)+Ginter(3,3);
                RR=2*Ginter*G- trak*G;
                
                
                currEr= abs(RR(1,1))+abs(RR(1,2))+abs(RR(1,3))+abs(RR(2,1))+abs(RR(2,2))+abs(RR(2,3))+abs(RR(3,1))+abs(RR(3,2))+abs(RR(3,3));
                
                
                
                err(fgs-(frangestart-1),xguess-(xstart-1),yguess-(ystart-1))=currEr;
                
                %  ess{xyguess-(xystart-1),f1gs-(frangestart-1),f2gs-(frangestart-1)}=RR;
                if(currEr<curerrminer)
                    curerrminer=currEr;
                    
                    currbestf=fgs;
                    currbestx=xguess;
                    currbesty=yguess;
                end
                
                
            end
        end
    end
    currbestf=fgs
    currbestx=xguess
    currbesty=yguess
    
%     myfstep=myfstep/2;
%     xstep=xstep/2;
%     ystep=ystep/2;
%     
%     
%     frangestartn=min(frangestart,currbestf-((frangeend-frangestart)/4));
%     frangeendn=max(frangeend,currbestf+((frangeend-frangestart)/4));
%     
%     frangeend=frangeendn;
%     frangestart=frangestartn;
%     
% end
curerrminer
currbestf
currbestx
currbesty
res=[
    curerrminer
    currbestf
    currbestx
    currbesty
    ]
x=[currbestf currbestf];
end





