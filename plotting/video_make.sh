#!/bin/bash

ffmpeg -framerate 15 -pattern_type glob -i 'out/*.bmp' -s 500x500  -c:v libx264 -pix_fmt yuv420p vid.mp4

# ffmpeg -framerate 10 -pattern_type glob -i 'out/*.bmp'   -vf scale=200x200  out.gif
