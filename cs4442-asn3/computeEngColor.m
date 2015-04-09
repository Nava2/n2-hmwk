function eng = computeEngColor(im, w)
%computeEngColor compute a weighted colour gradient.

t = ones(size(im));

for i = 1:3
    t(:, :, i) = t(:, :, i) .* w(i);
end

eng = sum((t .* double(im)) .^ 2, 3) .^ 0.5;

end

