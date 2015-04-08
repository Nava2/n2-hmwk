function [seam,c] = bestSeamV(M, P)
%bestSeamV computes the best vertical seam given the energy computations

[c, idx] = min(M(end,:));
seam = zeros(size(M, 1), 1);
seam(end) = idx;

for i = size(M, 1)-1:-1:1
   seam(i) = P(i+1, seam(i+1));
end

end

