function [seam,im,c,mask] = reduceWidth(im,w,mask,maskWeight)
%UNTITLED8 Summary of this function goes here
%   Detailed explanation goes here

EC = computeEngColor(im, w);
EG = computeEngGradH(im);

E = EC + EG + (maskWeight .* double(mask));

[M, P] = seamV_DP(E);
[seam, c] = bestSeamV(M, P);

im = removeSeamV(im, seam);
mask = removeSeamV(mask, seam);

end

