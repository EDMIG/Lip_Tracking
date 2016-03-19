function [ frameNorm ] = colorNormSat( frame )
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here

[numRow, numCol, ~] = size(frame);

frameNorm = zeros(numRow, numCol, 'double');

for row = 1 : numRow
    for col = 1 : numCol
        
        red     = frame(row, col, 1);
        green   = frame(row, col, 2);

        frameNorm(row, col) = 2 * atan((red - green) / green) / pi;    
    end
end

end

