function   [pvis] = cookUpdate(initialPvi,pvis,residuals);


% here the initial pvis are the leverage

L=initialPvi;
[npts,m]=size(initialPvi);
pvis=zeros(npts,1);
h=zeros(npts,1);
r=residuals';
for i=1:npts
    h(i,1)=(r(i,1)*L(i,1))/((1-L(i,1))*(1-L(i,1)));
end

%
%
%


minh=min(h);
maxh=max(h);
rangeh=maxh-minh;
hh=(h-minh)*(1/rangeh);
pvis=1-hh;



end