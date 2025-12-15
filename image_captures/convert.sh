#/usr/bin/env bash

image=${1%.ppm}
ppm_image=$1
png_image="$image.png"

magick "$ppm_image" "$png_image" && rm "$ppm_image"
