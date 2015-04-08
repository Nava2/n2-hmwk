function [seam,im,c,mask] = increaseHeight(im,w,mask,maskWeight)
%UNTITLED12 Summary of this function goes here
%   Detailed explanation goes here

im_t = permute(im, [2 1 3]);
mask_t = permute(mask, [2 1 3]);

[seam, im_t, c, mask_t] = increaseWidth(im_t, w, mask_t, maskWeight);

im = permute(im_t, [2 1 3]);
mask = permute(mask_t, [2 1 3]);

end

