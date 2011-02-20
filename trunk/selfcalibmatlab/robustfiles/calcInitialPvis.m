function [initpvis] = calcInitialPvis(typenum, x1, x2)
npts=size(x1,2);
if(typenum==0 || typenum==1 || typenum==2 || typenum==3 || typenum==6  )
    initpvis=ones(npts,1);
elseif(  typenum==4 || typenum==5 || typenum==7 || typenum==8)
    initpvis = initLeveragepvi( x1, x2);
    
end




end


function [initpvis] = initLeveragepvi( x1, x2)

L = calc_leveragefromCorrs(x1, x2);

minh=min(L);
maxh=max(L);
rangeh=maxh-minh;
initpvis=(L-minh)*(1/rangeh);

end
