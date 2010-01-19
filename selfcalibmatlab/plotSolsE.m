function [] =  plotSolsE(TF,w,h)

[m,n]=size(TF);



styles={'-.or' ,'-.xg', '-.+b', '-.*y', '-.vr' ,'-..c'};
figure
hold

title(['all the local minima points']);
xlabel('x coordinates of optical centers');
ylabel('y coordinates of optical centers');
zlabel('value of focal length ');

for q=1:n
plot3(xfinals(:,q),yfinals(:,q),ffinals(:,q),styles{mod(q,6)+1},'MarkerSize',12)
end
hold



end
