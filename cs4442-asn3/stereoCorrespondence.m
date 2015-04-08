function disp = stereoCorrespondence(left,right,wSize,maxDisp)
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here

L = double(left);
R = double(right);

function I = integralIm(im)
    I = zeros(size(im));
    I(1, :) = double(im(1, :));
    I(:, 1) = double(im(:, 1));
    
    for j = 1:size(I, 1)
       I(j, 1) = sum(im(1:j, 1)); 
    end
    
    for i = 1:size(I, 2)
       I(1, i) = sum(im(1, 1:i)); 
    end

    for j = 2:size(I, 1)
        for i = 2:size(I, 2)
            I(j, i) = I(j - 1, i) + I(j, i - 1) ...
                - I(j - 1, i - 1) + double(im(j, i));
        end  
    end    
end

disp = zeros(size(L));
bcost = realmax * double(ones(size(L)));
offset = floor(wSize / 2);
for d = 0:maxDisp
    buff = zeros(size(L, 1), size(L, 2) + d);
    buff(:, 1:end-d) = L;
    buff(:, 1+d:end) = abs(buff(:, 1+d:end) - R(:, 1:end));
    
    I = integralIm(buff);
    
    for y = 2+offset:size(L,1)-offset
        for x = 2+offset+d:size(L,2)-offset-d
            x1 = x - offset; y1 = y - offset;
            x2 = x + offset; y2 = y + offset;
            cost = I(y2, x2) - ...
                (I(y2, x1-1) + I(y1-1, x2)) + ...
                I(y1-1, x1-1);
            
            if cost < bcost(y, x)
               disp(y, x) = d;
               bcost(y, x) = cost;
            end
        end
    end
    
end

end

