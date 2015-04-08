function [imOut, cost] = intelligentResize(im,v,h,w, mask,maskWeight)
%UNTITLED13 Summary of this function goes here
%   Detailed explanation goes here

narginchk(4, 6);
if (nargin() < 5)
   mask = uint8(zeros(size(im, 1), size(im, 2)));
end

if (nargin() < 6)
   maskWeight = 1;
end

osize = size(im) + abs([v h 0]);
out = im;
cost = 0;

vid = VideoWriter('fuck.avi');
open(vid);

while (v ~= 0 && h ~= 0)
    if (v < 0) 
        [~, out, c, mask] = reduceWidth(out, w, mask, maskWeight);
        v = v + 1;
    elseif (v > 0)
        [~, out, c, mask] = increaseWidth(out, w, mask, maskWeight); 
        v = v - 1;
    end
    cost = cost + c;
    
    if (h < 0) 
        [~, out, c, mask] = reduceHeight(out, w, mask, maskWeight);
        h = h + 1;
    elseif (h > 0)
        [~, out, c, mask] = increaseHeight(out, w, mask, maskWeight); 
        h = h - 1;
    end
    cost = cost + c;
    
    padSize = abs(osize - size(out));
    padded = out;
    padded(end+1:end+padSize(1), :, :) = zeros(padSize(1), size(padded, 2), size(padded, 3));
    padded(:, end+1:end+padSize(2), :) = zeros(size(padded, 1), padSize(2), size(padded, 3));
    
    writeVideo(vid, im2frame(padded));
end

imOut = out;
close(vid);

end

