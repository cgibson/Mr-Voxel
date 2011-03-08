#!/bin/zsh

zmodload zsh/mathfunc

xpos=0
ypos=4
zpos=-6
pi=3.141592654

width=$1
height=$2

for frame in {0..120}
do
  theta=$(($frame*(pi*2) / 120.0));
  xpos=$((-6 * sin($theta)));
  zpos=$((-6 * cos($theta)));
  sed -e "s!%XPOS%!$xpos!; s!%YPOS%!$ypos!; s!%ZPOS%!$zpos!" output/ocean_tmp.rp > tmp.rp

  echo "./raytracer -I tmp.rp -W $width -H $height"
  ./raytracer -I tmp.rp -W $width -H $height

  cp tmp.tga animation/tmp_$frame.tga
  mv tmp.tga animation/tmp_$((frame+120)).tga
  sleep 2
done
