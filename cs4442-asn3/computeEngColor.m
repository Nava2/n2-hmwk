function eng = computeEngColor(im, w)
%UNTITLED7 Summary of this function goes here
%   Detailed explanation goes here

t = ones(size(im));

for i = 1:3
    t(:, :, i) = t(:, :, i) .* w(i);
end

eng = sum(t .* double(im), 3);

end

