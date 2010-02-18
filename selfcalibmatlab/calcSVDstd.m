clear all; clc; close all;


% numtries=50;
% 
% perfFocal=zeros(32,1);
% perfOC=zeros(32,1);
% count=0;
% for j=1:32
%     t(j)=(3^j)*(1e-16);
%     curval=(3^j);
%     for i=1:numtries
%         clear F ks;
%         numPs=floor(rand()*5)+2;
%         [ F, ks ] = generateF( floor(rand()*3), floor(rand()*3), 1.0+((rand()*0.01)),50+(rand()*30),0 ,numPs,0,0);
%         [m,numFs]=size(F);
%         
%         gfocal=(ks{1,1}(1,1)+ks{1,1}(2,2))/2;
%         gocx=  ks{1,1}(1,3);
%         gocy=ks{1,1}(2,3)  ;
% 
% 
% 
%         [fcl, centerloc] =S2nonlinsolveEssNfram(F,512,512,curval);
% 
%         errps(i,j)=abs(fcl(1)-gfocal);
%         errnframOC(i,j)=sqrt(((centerloc(1)-gocx)^2)+((centerloc(2)-gocy)^2));
% 
%         count=count+1;
%         
%         disp(['count ' num2str(count) ' out of total: ' num2str(numtries*32)]);
% 
%     end
% 
% 
%     perfFocal(j,1)=mean(errps(:,j));
%     perfOC(j,1)=mean(errnframOC(:,j));
% 
% end
% 
% plot(t',perfFocal);
% title('focal length error versus Tolx Tolf');
% figure
% 
% 
% plot(t',perfOC);
% title('Optical center error versus Tolx Tolf');
clear all; clc; close all


numtries=1000;

inliersvals=zeros(numtries,1);
for i=1:numtries
    clear F ks;
    numPs=floor(rand()*5)+2;
    [ F, ks ] = generateF( floor(rand()*3), floor(rand()*3), 1.0+((rand()*0.01)),50+(rand()*30),0 ,numPs,0.6,2);
    [m,numFs]=size(F);

    [fcl, centerloc] =S2nonlinsolveEssNfram(F);

    currVal=computerEssentialErrorSVDNFramesWeighted([fcl(1,1) centerloc(1,1) centerloc(1,2)],F,ones(numFs,1));


    inliersvals(i,1)=currVal;

end

mean(inliersvals)

std(inliersvals)