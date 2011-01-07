
function [F, inliers,trialcount] = ransacfitfundmatrix2(x1, x2,  feedback)
    t = 1.96*1.96;  % Distance threshold for deciding outliers
    if ~all(size(x1)==size(x2))
        error('Data sets x1 and x2 must have the same dimension');
    end
    
    if nargin == 3
	feedback = 0;
    end
    
    [rows,npts] = size(x1);
    if rows~=2 & rows~=3
        error('x1 and x2 must have 2 or 3 rows');
    end
    
    if rows == 2    % Pad data with homogeneous scale factor of 1
        x1 = [x1; ones(1,npts)];
        x2 = [x2; ones(1,npts)];        
    end
    
    

    s = 7;  % Number of points needed to fit a fundamental matrix. Note that
            % only 7 are needed but the function 'fundmatrix' only
            % implements the 8-point solution.
    
    fittingfn = @vgg_F_from_7pts_2img2;
    distfn    = @sampsonF;
    degenfn   = @isdegenerate;
    scorefunc = @ransacScore;
    sampleFunc= @randsample;
    % x1 and x2 are 'stacked' to create a 6xN array for ransac
    [F, inliers,trialcount] = ransac([x1; x2], fittingfn, distfn, degenfn, s, t,scorefunc,sampleFunc);

    % Now do a final least squares fit on the data points considered to
    % be inliers.
    F = fundmatrix(x1(:,inliers), x2(:,inliers));
    


%----------------------------------------------------------------------
% (Degenerate!) function to determine if a set of matched points will result
% in a degeneracy in the calculation of a fundamental matrix as needed by
% RANSAC.  This function assumes this cannot happen...
     
function r = isdegenerate(x)
    r = 0;    
    
