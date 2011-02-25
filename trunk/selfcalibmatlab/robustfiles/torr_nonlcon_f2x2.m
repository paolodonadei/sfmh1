%	By Philip Torr 2002
%	copyright Microsoft Corp.

function [c,ceq] = torr_nonlcon_f2x2(f, x,t,W)
%c = ...     % Compute nonlinear inequalities at f.
%ceq = ...   % Compute nonlinear equalities at f.

%g(1) = norm(f) -1.0;
%g(2) = f(1) * (f(5) * f(9) - f(6) * f(8)) - f(2) * (f(4) * f(9) - f(6) * f(7)) + f(3) * (f(4) * f(8) - f(5) * f(7));
c = [];


F(1,1)=f(1,1);
F(1,2)=f(2,1);
F(1,3)=f(3,1);
F(2,1)=f(4,1);
F(2,2)=f(5,1);
F(2,3)=f(6,1);
F(3,1)=f(7,1);
F(3,2)=f(8,1);
F(3,3)=f(9,1);

%what norm should we use!
ceq(1) = norm(F)- 1;
%ceq(1)= norm(f) -1.0;
ceq(2) =det(F);
