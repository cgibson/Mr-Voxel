ffmpeg -i animation/tmp_%d.tga -vcodec libx264 -vpre lossless_medium -threads 0 -acodec copy output.mkv
