function [nh] = findHwald(epsilon,omega,epsilonT)



nh=fzero(@(h)((epsilonT*((omega/epsilon)^h))+((1-epsilonT)*(((1-omega)/(1-epsilon))^h))-1),100);






end