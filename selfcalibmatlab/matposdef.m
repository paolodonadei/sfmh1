function [ isposdef ] = matposdef( M )
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here

lemma= eig(M);
isposdef=true;
for i=1:size(lemma,1)

    if((abs(lemma(i)))>4.11663901269155e-10 && lemma(i)<0)
        isposdef=false;
        break;
    end
    if(isreal(lemma(i))~=1)
        isposdef=false;
        break;
    end




end

