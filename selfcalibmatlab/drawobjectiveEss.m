function [ G ] = drawobjective( F , ks)
%plot for graphiong the objective function around the basin of attraction
% this is useful in seeing the shape of the objective function 
G=eye(3);




for f=300:1400
    for xy=200:400
        [EF1] = computerEssentialError([f; xy; xy],F);
        myerr_samexy(f-299,xy-199)=sum(abs(EF1));

        [EF2] = computerEssentialError([f; 256; xy],F);
        myerr_xfixed(f-299,xy-199)=sum(abs(EF2));


        [EF3] = computerEssentialError([f; xy; 256],F);
        myerr_yfixed(f-299,xy-199)=sum(abs(EF3));


    end
end

fc=ks{1}(1,1);
x=ks{1}(1,3);
y=ks{1}(2,3);

sindex=0;
for i=-200:0.1:200

    sindex=sindex+1;


    [EF4] = computerEssentialError([fc+i; x+i; y+i],F);
    errline1(sindex)=sum(abs(EF4));


    [EF5] = computerEssentialError([fc-i; x-i; y-i],F);
    errline2(sindex)=sum(abs(EF5));


    [EF6] = computerEssentialError([fc-i; x+i; y-i],F);
    errline3(sindex)=sum(abs(EF6));



    [EF7] = computerEssentialError([fc+i; x-i; y+i],F);
    errline4(sindex)=sum(abs(EF7));

end

plot(errline1);
hold
plot(errline2);
hold
plot(errline3);
hold
plot(errline4);
