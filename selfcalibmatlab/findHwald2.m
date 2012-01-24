function [nh] = findHwald2(cepsilon,comega,Tepsilon)



counter=0;
diff=1000;
ch=5;
while(counter<100  && abs(diff)>0.00001)
    
    ob=(Tepsilon*pow((comega/cepsilon),ch)) + ((1-Tepsilon)*pow(((1-comega)/(1-cepsilon )),ch)) - 1;
    jac=Tepsilon*log(comega/cepsilon)*pow((comega/cepsilon),ch) - log((comega - 1)/(cepsilon - 1))*pow(((comega - 1)/(cepsilon - 1)),ch)*(Tepsilon - 1);
    diff=( (ob/ jac));
    if(diff>50)
        ch=0;
    else
    ch=ch-(ob/ jac);
    end
    counter=counter+1;
end

nh=ch;

end