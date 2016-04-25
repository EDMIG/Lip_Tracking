%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% This program tracks the movements of the lips
% It is composed of a 2-step process:
%
%   1 - Identify the lips from RGB images
%
%   2 - Locate points on the lips to plot a line
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

close all;

% Load a video
% videoFile = strcat(pwd, '\..\Data\allo_video.avi');
videoFile = 'C:\Users\sebno\Dropbox\Subject\Sub1\Hello\Hello World\Hello World_5\Hello World_5_video.avi';
video = VideoReader(videoFile);


% Track lips in each frame
numFrames = video.NumberOfFrames;
baseFrames = cell(1,numFrames);
bwFrames = cell(1,numFrames);
lipLines = cell(1,numFrames);

video = VideoReader(videoFile);     % video has to be recreated after a call to NumberOfFrames
frameIdx = 0;
while hasFrame(video)
    frameIdx = frameIdx + 1;
    
    rawFrame = readFrame(video,'native');
    [frame, bwFrame, lipLine] = lipTracking(rawFrame);
    
    baseFrames{1, frameIdx} = frame;
    bwFrames{1, frameIdx} = bwFrame;
    lipLines{1, frameIdx} = lipLine;
end

%% Plot frames with contrast and lips tracking

% Create the two figures with their properties
bwFig = figure;
bwAx = axes('Parent', bwFig);
lipTrackFig = figure;

bwFig.NumberTitle = 'off';
bwFig.Name = 'Contrast Frames for "Hello World"';
lipTrackFig.NumberTitle = 'off';
lipTrackFig.Name = 'Lips Tracking for "Hello World"';

% This code block is to display all frames 
% numRows = int32(6);
% numCols = idivide(numFrames, numRows, 'ceil');
% numCols = double(numCols);
% numRows = double(numRows);
% selectFrames = 1:numFrames;

% This code block is to display only frames of interest
numRows = 2;
numCols = 3;
selectFrames = [5, 11, 13, 24, 32, 40];

% Display contrast frames
figure(bwFig);
j = 0;
for i = selectFrames
    j = j + 1;
    subplot(numRows, numCols, j);
    imshow(bwFrames{1,i});
end

% Display lips tracking frames
figure(lipTrackFig);
j = 0;
for i = selectFrames
    j = j + 1;
    subplot(numRows, numCols, j);
	hold on;
    imshow(baseFrames{1,i});
    lipLine = lipLines{1,i};
    plot(lipLine(:,1), lipLine(:,2), '-go', 'LineWidth', 4);
    hold off;
end
