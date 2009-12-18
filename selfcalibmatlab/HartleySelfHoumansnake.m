function [x] = HartleySelfHoumansnake( F,w,h )

focalstep=1;
xystep=1;

cont=1;


f1=w+h;
f2=w+h;
x1=w/2;
y1=h/2;
x2=w/2;
y2=h/2;

bestf1=w+h;
bestf2=w+h;
bestx1=w/2;
besty1=h/2;
bestx2=w/2;
besty2=h/2;


xhlimit=400;
xllimit=200;
yhlimit=400;
yllimit=200;
fhlimit=1500;
fllimit=300;

count=1;
minerror=10000000;
while count==1
    
    count=0;
    curerr=10000;
    
    for x1vals=(x1-xystep):xystep:(x1+xystep)
        for y1vals=(y1-xystep):xystep:(y1+xystep)
            for x2vals=(x2-xystep):xystep:(x2+xystep)
                for y2vals=(y2-xystep):xystep:(y2+xystep)
                    for f1vals=(f1-focalstep):focalstep:(f1+focalstep)
                        for f2vals=(f2-focalstep):focalstep:(f2+focalstep)
                            
                            curerr=  EvalErrorParams1(F,f1vals,f2vals,x1vals,y1vals,x2vals,y2vals );
                            
                            if(curerr<minerror)
                                % curerr
                                count=1;
                                minerror=curerr;
                                bestf1=f1vals;
                                bestf2=f2vals;
                                bestx1=x1vals;
                                besty1=y1vals;
                                bestx2=x2vals;
                                besty2=y2vals;
                            end
                            
                        end
                    end
                end
            end
        end
    end
    
    f1 = bestf1;
    f2 = bestf2;
    x1 = bestx1;
    y1 = besty1;
    x2 = bestx2;
    y2 = besty2;
    
    disp(['f1=' num2str(f1) '  f2=' num2str(f2) '  x1=' num2str(x1) '  y1=' num2str(y1) '  x2=' num2str(x2) '  y2=' num2str(y2)  ' and minerror is ' num2str(minerror)]);
    
    if(x1<xllimit || x1>xhlimit)
        x1=(randn()*100 ) +(w/2);
    end
    
    
    if(x2<xllimit || x2>xhlimit)
        x2=(randn()*100 ) +(w/2);
    end
    
    if(y1<yllimit || y1>yhlimit)
        y1=(randn()*100 ) +(h/2);
    end
    
    
    if(y2<yllimit || y2>yhlimit)
        y2=(randn()*100 ) +(h/2);
    end
    
    if(f1<fllimit || f1>fhlimit)
        f1=(randn()*500 ) +(h+w);
    end
    
    
    if(f2<fllimit || f2>fhlimit)
        f2=(randn()*500 ) +(h+w);
    end
    
end

count
minerror
x=[f1 f2];

end