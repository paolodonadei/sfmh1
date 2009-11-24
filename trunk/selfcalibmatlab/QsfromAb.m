function [Q1, Q2]=QsfromAb(A,b)
%this is for the rank deficient version that works with 2 frames


%so this function has to check b, if b is all zeros then we do the
%simplified version which is just the last two vectors of V, otherwise we
%gonna folow the recipe in the multiple view geomtry book's appendic and
%get the two solutions


biszeros=1;

[m n]=size(b);







end
