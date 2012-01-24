
clear all; clc; close all


j=1;
omega=0.05;
epsilon=0.2;
i=1;
for epsilonT=epsilon:0.05:1

        hv(i)= fzero(@(h)((epsilonT*((omega/epsilon)^h))+((1-epsilonT)*(((1-omega)/(1-epsilon))^h))-1),4);
    i=i+1;
 end

plot(epsilon:0.05:1,hv);

% 
% clear all; clc; close all
% 
% 
% j=1;
% omega=0.05;
% epsilon=0.2;
% 
% j=1;
% figure
% hold
% for epsilonT=epsilon:0.05:1
%     epsilonT
%     i=1;
%     for h=0:.1:10
%       
%         er(i)=((epsilonT*((omega/epsilon)^h))+((1-epsilonT)*(((1-omega)/(1-epsilon))^h)));
%         i=i+1;
%   
%     end
%   plot(0:.1:10,er,0:.1:10,ones(size(er)) );
% end
% 

