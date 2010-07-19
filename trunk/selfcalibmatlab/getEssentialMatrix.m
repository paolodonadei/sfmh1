% getEssentialMatrix - computes the Essential matrix from the Fundamental
% matrix given the two camera matrices.
%
%
% Computes the essential matrix for calibrated cameras from the fundamental
% matrix F given the two camera matrices as described by Zisserman in p257.
%
%
% Input  - F  -> Fundamental matrix
%        - K1 -> First camera matrix
%        - K2 -> Second camera matrix
%
% Output - 3x3 essential matrix E
%
%
%
% Author: Isaac Esteban
% IAS, University of Amsterdam
% TNO Defense, Security and Safety
% iesteban@science.uva.nl
% isaac.esteban@tno.nl


function E = getEssentialMatrix(F,K1,K2)

    E = (K2')*F*K1;