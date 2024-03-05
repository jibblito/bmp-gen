#!/bin/bash

ffmpeg -framerate 10 -pattern_type glob -i 'out/*.bmp'   -c:v libx264 -pix_fmt yuv420p out.mp4

# ffmpeg -framerate 10 -pattern_type glob -i 'out/*.bmp'   -vf scale=200x100  out.gif
