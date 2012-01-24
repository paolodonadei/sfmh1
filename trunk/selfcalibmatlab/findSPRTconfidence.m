function [cn,nk,a, h,h2]=findSPRTconfidence(epsilon, omega, k)

miu0=0.05;

num=size(epsilon,1);

cn=1;
pg=epsilon(num)^7;


for i=1:(num);
    a(i)=findAsprt(epsilon(i),omega(i));
    h(i)=findHwald2(epsilon(i),omega(i),epsilon(num));
   
end


for i=1:(num);
    cn=cn*((1-pg*(1-(1.0/(a(i)^h(i)))))^k(i));
end

cnless=1;
for i=1:(num-1);
    cnless=cnless*((1-pg*(1-(1.0/(a(i)^h(i)))))^k(i));
end

nk=(log(miu0) - log(cnless))/(log(1-(pg*(1/a(num))))) ;

end