function [ G ] = drawobjectiveEss( F , ks)
%plot for graphiong the objective function around the basin of attraction
% this is useful in seeing the shape of the objective function
G=eye(3);
nowtime=num2str(sum(round(100*clock)));
fc=ks{1}(1,1);
x=ks{1}(1,3);
y=ks{1}(2,3);


myrange=20;
startf=ceil(fc)-myrange;
endf=ceil(fc)+myrange;

for f=startf:endf
    for as=0:0.1:1.5
       aspect=as;
        [EF1] = computerEssentialErrorTesting([f  x  y aspect],F);
 
        myerr_samexy(f-startf+1,uint32(as*10)+1)=sum((EF1.^2));
      
        [EF2] = computerEssentialErrorTesting([f  x+10  y+10 aspect],F);
        myerr_xfixed(f-startf+1,uint32(as*10)+1)=sum((EF2.^2));
        
        
        [EF3] = computerEssentialErrorTesting([f  x-10  y-10 aspect],F);
        myerr_yfixed(f-startf+1,uint32(as*10)+1)=sum((EF3.^2));
        
        
        
    end
end

[X,Y]=meshgrid(0:0.1:1.5,startf:endf);

meshc(X,Y,myerr_samexy);
xlabel('x and y coordinates of optical centers');
ylabel('focal length in pixels');
zlabel('value of energy function ');
title(['energy function for x=y']);
saveas(gcf,['objective_xy'  nowtime '.jpg']);

figure
meshc(X,Y,myerr_xfixed);
xlabel('y coordinates of optical centers');
ylabel('focal length in pixels');
zlabel('value of energy function ');
title(['energy function for x=256, y varying']);
saveas(gcf,['objective_xfixed'  nowtime '.jpg']);

figure
meshc(X,Y,myerr_yfixed);
xlabel('x coordinates of optical centers');
ylabel('focal length in pixels');
zlabel('value of energy function ');
title(['energy function for y=256, x varying']);
saveas(gcf,['objective_yfixed'  nowtime '.jpg']);


%now just for x and y


% 
% 
% 
% for x=200:350
%     for y=200:350
%         [EF7] = computerEssentialError([ks{1}(1,1)  x  y],F);
%         myerr_xyE_fcor(x-199,y-199)=sum((EF7.^2));
%         
%         [EF8] = computerEssentialError([ks{1}(1,1)-10  x  y],F);
%         myerr_xyE_flow(x-199,y-199)=sum((EF8.^2));
%         
%         
%         [EF9] = computerEssentialError([ks{1}(1,1)+10  x  y],F);
%         myerr_xyE_fhigh(x-199,y-199)=sum((EF9.^2));
%         
%         
%         
%     end
% end
% 
% [X,Y]=meshgrid(200:350,200:350);
% 
% figure
% meshc(X,Y,myerr_xyE_fcor);
% xlabel('x coordinates of optical centers');
% ylabel('y coordinates of optical centers');
% zlabel('value of energy function ');
% title(['energy function for x vs y, when f is set correctly']);
% saveas(gcf,['objective_xvsy_corf'  nowtime '.jpg']);
% 
% figure
% meshc(X,Y,myerr_xyE_flow);
% xlabel('x coordinates of optical centers');
% ylabel('y coordinates of optical centers');
% zlabel('value of energy function ');
% title(['energy function for x vs y, when f=ftrue-10']);
% saveas(gcf,['objective_xvsy_flow'  nowtime '.jpg']);
% 
% figure
% meshc(X,Y,myerr_xyE_fhigh);
% xlabel('x coordinates of optical centers');
% ylabel('y coordinates of optical centers');
% zlabel('value of energy function ');
% title(['energy function for x vs y, when f=ftrue+10']);
% saveas(gcf,['objective_xvsy_fhigh'  nowtime '.jpg']);
% 





%now for the profile of the objective function
sindex=0;
for i=-20:0.1:20
    
    sindex=sindex+1;
    
    
    [EF4] = computerEssentialError([fc+i x  y],F);
    errline1(sindex)=sum((EF4.^2));
    
    
    [EF5] = computerEssentialError([fc+i  x+10  y+10],F);
    errline2(sindex)=sum((EF5.^2));
    
    
    [EF6] = computerEssentialError([fc+i  x-10  y-10],F);
    errline3(sindex)=sum((EF6.^2));
    
    
    
    [EF7] = computerEssentialError([fc+i  x-10  y+10],F);
    errline4(sindex)=sum((EF7.^2));
    
end

i=[-20:0.1:20];
figure
plot(i,errline1,i,errline2,i,errline3,i,errline4);
legend('correct f','u_x,u_y plus 10','u_x,u_y minus 10','u_x,u_y mixed errors');
title(['The basin of attraction and the magnitude of the objective function around it']);
xlabel('distance from the optimal point');
ylabel('energy function value');
saveas(gcf,['basin_distance'  nowtime '.jpg']);
