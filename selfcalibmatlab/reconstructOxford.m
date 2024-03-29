%function[X] = reconstructOxford(seqname)
close all;
clc;

close all
clear all
seqname='merton2';


[corrs, IMS, P,K, F, E]  = readCorrsOxford(seqname,0, 0);

frame1=1;
frame2=2;

enum=0;
counter=0;
for i=1:size(P,2)
    for j=1:i
        if(i~=j)
            counter=counter+1;
            
            if(j==frame1 && i==frame2)
                enum=counter;
                EFinal=E{enum}';
                FFinal=F{enum}';
                x2=corrs{1,enum};
                x1= corrs{2,enum};
            end
        end
        
        
        
    end
end
counter=0;
if(enum==0)
    for i=1:size(P,2)
        for j=1:i
            if(i~=j)
                counter=counter+1;
                
                if(i==frame1 && j==frame2)
                    enum=counter;
                    EFinal=E{enum};
                    FFinal=F{enum};
                    x1=corrs{1,enum};
                    x2= corrs{2,enum};
                end
            end
            
            
        end
    end
end


%EFinal=EFinal';
EFinal
% find Ps for frames 1 and 2 by decomposing E
%[P_candids]=decomposeEmatrix(E{1});
[P_candids] = getCameraMatrix(EFinal);

X=zeros(3,2);

X(1,1)=x1(1,1);
X(2,1)=x1(2,1);
X(3,1)=1;
    

X(1,2)=x2(1,1);
X(2,2)=x2(2,1);
X(3,2)=1;

[Pf] = getCorrectCameraMatrix(P_candids, K{frame1},K{frame2}, X);


%
%


%
R1=eye(3);
t1=[0 ; 0 ; 0 ];
%
%

%
[Kc1, Rc1, tc1] = vgg_KR_from_P(P{frame1});
[Kc2, Rc2, tc2] = vgg_KR_from_P(P{frame2});
%
%
%

Pc1=K{1,frame1}*R1*[eye(3,3) t1];
Pc2=K{1,frame2}*Pf;
P
Pc1
Pc2

clear X

for i=1:size(x1,2)
    
    xc1=x1(:,i);
    xc2=x2(:,i);
    stru=trangulate(Pc1,Pc2,xc1,xc2);
    X(i,1)=stru(1,1);
    X(i,2)=stru(2,1);
    X(i,3)=stru(3,1);
    
end

figure
scatter3(X(:,1),X(:,2),X(:,3),5)
%     k = waitforbuttonpress
%     clc
%     close all



%end