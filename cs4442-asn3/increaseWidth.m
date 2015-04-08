function [seam,im,c,mask] = increaseWidth(im,w,mask,maskWeight)
%UNTITLED11 Summary of this function goes here
%   Detailed explanation goes here

EC = computeEngColor(im, w);
EG = computeEngGradH(im);

E = EC + EG + (maskWeight .* double(mask));

[M, P] = seamV_DP(E);
[seam, c] = bestSeamV(M, P);

im = addSeamV(im, seam);
mask = addSeamV(mask, seam);

end

