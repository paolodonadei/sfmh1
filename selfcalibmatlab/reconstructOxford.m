%function[X] = reconstructOxford(seqname)
%close all;
clc;
%\
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
            
            if(i==frame1 && j==frame2)
                enum=counter;
                EFinal=E{enum}';
                
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
                
                if(j==frame1 && i==frame2)
                    enum=counter;
                    EFinal=E{enum};
                         x1=corrs{1,enum};
                   x2= corrs{2,enum};
                end
            end
            
            
        end
    end
end
EFinal
% find Ps for frames 1 and 2 by decomposing E
%[P_candids]=decomposeEmatrix(E{1});
[P_candids] = getCameraMatrix(EFinal);



%
%

for j=1:size(P_candids,2)
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
    Pc2=K{1,frame1}*R1*[eye(3,3) t1];
    Pc1=K{1,frame2}*P_candids{j};
    %      Pc1
    %      Pc2
    
    
    % [Kc1, Rc1, tc1] = vgg_KR_from_P(Pc1);
    % [Kc2, Rc2, tc2] = vgg_KR_from_P(Pc2);
    %
    % tdiff= tc1- tc2;
    % tdiff=tdiff/tdiff(3,1);
    %
    % Rdiff=inv(Rc1)*Rc2;
    % tdiff
    % Rdiff
    % Kc1
    % Kc2
    for i=1:size(x1,2)
        
        xc1=x1(:,i);
        xc2=x2(:,i);
        stru=trangulate(Pc1,Pc2,xc2,xc1);
        X(i,1)=stru(1,1)/stru(4,1);
        X(i,2)=stru(2,1)/stru(4,1);
        X(i,3)=stru(3,1)/stru(4,1);
        
    end
    
    figure
    scatter3(X(:,1),X(:,2),X(:,3),5)
    
end

%end