function [F,J] = computerEssentialError(x,MYF)
%this function , given a camera center and a focal length and a fundamental
%matrix computes the error with respect to a fundamental matrix




F = computerEssentialErrorVector(x,MYF);


if nargout > 1   % Two output arguments
   % k=88
    J= computerEssentialJacobian(x,MYF);
end

end

