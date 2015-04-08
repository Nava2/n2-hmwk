function [M,P] = seamV_DP(E)
%seamV_DP Compute vertical seam via 

M = zeros(size(E));
P = zeros(size(E));
M(1,:) = E(1,:);

for r = 2:size(E,1)
    for c = 1:size(E,2)
        if c == 1 
            a = 0;
        else 
            a = 1;
        end
        
        [m, i] = min(M(r-1,max(c-1,1):min(c+1,size(E,2))));
        M(r, c) = E(r, c) + m;
        P(r, c) = c - (1 + a) + i;
    end
    
end

end

