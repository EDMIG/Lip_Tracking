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
videoFile = strcat(pwd, '\..\Data\allo_video.avi');
video = VideoReader(videoFile);

% currentFolder = pwd;
% rawFrame = imread(strcat(currentFolder, '\..\Data\Snip20160318_3.png'));

% Track lips in each frame
while hasFrame(video)
    rawFrame = readFrame(video,'native');
    lipTracking(rawFrame);
end
