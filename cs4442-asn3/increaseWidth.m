function [seam,im,c,mask] = increaseWidth(im,w,mask,maskWeight)
%increaseWidth add a low energy vertical seam

EC = computeEngColor(im, w);
EG = computeEngGradH(im);

E = EC + EG + (maskWeight .* double(mask));

[M, P] = seamV_DP(E);
[seam, c] = bestSeamV(M, P);

im = addSeamV(im, seam);
mask = addSeamV(mask, seam);

end

