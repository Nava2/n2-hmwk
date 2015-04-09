function eng = computeEngGradH(im)
%computeEngGradH Compute a greyscale gradient 

M = [ -1 0 1 ];

buff = zeros(size(im));
for ch = 1:3
   buff(:, :, ch) = abs(applyMask(im(:, :, ch), M)); 
end

eng = sum(buff, 3);

end

