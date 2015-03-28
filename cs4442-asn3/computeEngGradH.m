function eng = computeEngGradH(im)
%UNTITLED4 Summary of this function goes here
%   Detailed explanation goes here

M = [ -1 0 1 ];

buff = zeros(size(im));
for ch = 1:3
   buff(:, :, ch) = abs(applyMask(im(:, :, ch), M)); 
end

eng = sum(buff, 3);

end

