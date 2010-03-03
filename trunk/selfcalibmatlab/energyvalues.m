clear all
clc
close all
% this script was created to study the value of the objective function, the
% ultimate goal of this was to find the standard deviation of the values
% for the objective function for delineating outliers in the case deletion
% and diagnostics case, my hypothesis was that the best way to find the
% average value of the objectuive function was to provide a wrong solution
% to the system and find the median of the objective function of the
% fundamental matrices

% error in solution
[corrs, IMS, P,ks, F] = readCorrsOxford('C:\Documents and Settings\hrast019\Desktop\data\euclidean\wadham', 0,0);
x=[ks{1,1}(1,1) ks{1,1}(1,3) ks{1,1}(2,3) ks{1,1}(2,2)]
for i=-700:700
    x1=x;
    x2=x;
    x1(1,1)=x1(1,1)+i;
    x1(1,4)=x1(1,4)+i;
    x2(1,2)=x2(1,2)+i;
    x2(1,3)=x2(1,3)+i;
    z1(i+701)=computerEssentialErrorSVDNFramesWeighted(x1,F);
    z2(i+701)=computerEssentialErrorSVDNFramesWeighted(x2,F);
end

t=-700:1:700;
plot(t,z1);
figure
plot(t,z2);
figure


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

noise=0;
w=1024;
h=768;
count=1;


for i=0:0.001:0.1
    noise=i;
 
    %noise
    [corrs, IMS, P,ks, F] = readCorrsOxford('C:\Documents and Settings\hrast019\Desktop\data\euclidean\wadham', noise,1);
    x1=[ks{1,1}(1,1) ks{1,1}(1,3) ks{1,1}(2,3) ks{1,1}(2,2)];
    x2=[w w/2 h/2 w];
    
    
    v1(count)=computerEssentialErrorSVDNFramesWeighted(x1,F);
    v2(count)=computerEssentialErrorSVDNFramesWeighted(x2,F);
    count=count+1;
end

tv=0:0.001:0.1;
plot(tv,v1);
figure
plot(tv,v2);
