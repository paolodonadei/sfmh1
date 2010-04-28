%function[X] = reconstructOxford(seqname)
close all;

seqname='merton1';
[corrs, IMS, P,K, F, E]  = readCorrsOxford(seqname,0, 0);

% find Ps for frames 1 and 2 by decomposing E
% [P_candids]=decomposeEmatrix(E{1});
% 
% 
% for j=1:size(P_candids,2)
    
%    R1=eye(3);
%    t1=[0 ; 0 ; 0 ];
    
    
    x1=corrs{1,1};
    x2= corrs{2,1};
    
  %  Pc1=K{1,1}*[R1 t1];
   % Pc2=K{1,2}*P_candids{j};
  
      Pc1=PZ1;
    Pc2=PZ2;
%    Pc1=P{1};
%    Pc2=P{2};
    % chose P2 from P_candids
    for i=1:size(x1,2)
       
        xc1=inv(K{1,2})*x1(:,i);
        xc2=inv(K{1,1})*x2(:,i);
        stru=trangulate(Pc1,Pc2,xc2,xc1);
        X(i,1)=stru(1,1)/stru(4,1);
        X(i,2)=stru(2,1)/stru(4,1);
        X(i,3)=stru(3,1)/stru(4,1);
        
    end
    
    figure
    scatter3(X(:,1),X(:,2),X(:,3),5)
    
% end

%end