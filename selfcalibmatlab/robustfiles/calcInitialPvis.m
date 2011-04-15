function [initpvis] = calcInitialPvis(typenum, x1, x2)
npts=size(x1,2);
if(typenum==1 )
    initpvis=ones(npts,1);
elseif(typenum==2)
    initpvis=ones(npts,1)*0.05;
elseif(typenum==3 )
     [Fnew,initpvis] = fundmatrix(x1, x2);
elseif(typenum==4 )
    initpvis = initLeveragepvi( x1, x2);
else
    display(' bad option');
end




end

function [L] = initLeveragepviGroup( x1, x2)
eachgroup=10;
npts=size(x1,2);
L=[];
numgroups=floor(npts/eachgroup);

for i=1:(numgroups-1)
    d1=1+((i-1)*eachgroup);
    d2=d1+eachgroup-1;
    clear V
 %   [d1 d2]
    V=calc_leveragefromCorrs(x1(:,d1:d2), x2(:,d1:d2));
    L=[L ; V];
    
end

%[ d2+1 (npts)]
V=calc_leveragefromCorrs(x1(:, d2+1:(npts)), x2(:,d2+1:(npts)));
L=[L ; V];


end


function [initpvis] = initLeveragepvi( x1, x2)

L = calc_leveragefromCorrs(x1, x2);

% should i be doing this? zzz
minh=min(L);
maxh=max(L);
rangeh=maxh-minh;
initpvis=(L-minh)*(1/rangeh);

end
