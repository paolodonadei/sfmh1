%function[X] = reconstructOxford(seqname)
%close all;
%clc;
close all
seqname='merton2';

frame1=1;
[corrs, IMS, P,K, F, E]  = readCorrsOxford(seqname,0, 0);

% find Ps for frames 1 and 2 by decomposing E
 %[P_candids]=decomposeEmatrix(E{1});
%  [P_candids] = getCameraMatrix(E{1});
%  
% 
% for j=1:size(P_candids,2)
%     
%     R1=eye(3);
%   t1=[0 ; 0 ; 0 ];
%     
%     
    x1=corrs{1,1};
     x2= corrs{2,1};
%     
%  [Kc1, Rc1, tc1] = vgg_KR_from_P(Pc1);
%  [Kc2, Rc2, tc2] = vgg_KR_from_P(Pc2);
%   
%        
%  
%      Pc2=K{1,2}*R1*[eye(3,3) t1];
%      Pc1=K{1,1}*P_candids{j};
%      Pc1
%      Pc2
     
   Pc1=K{1,2}*PZ1;
   Pc2=K{1,1}*PZ2;   
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
        stru=trangulate(Pc2,Pc1,xc2,xc1);
        X(i,1)=stru(1,1)/stru(4,1);
        X(i,2)=stru(2,1)/stru(4,1);
        X(i,3)=stru(3,1)/stru(4,1);
        
    end
    
    figure
    scatter3(X(:,1),X(:,2),X(:,3),5)
    
 %end

%end