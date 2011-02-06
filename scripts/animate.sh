#!/bin/zsh

zmodload zsh/mathfunc

xpos=6
ypos=2
zpos=0
pi=3.141592654

width=$1
height=$2

for frame in {0..120}
do
  theta=$(($frame*(pi*2) / 120.0));
  xpos=$((5 * cos($theta)));
  zpos=$((5 * sin($theta)));
  sed -e "s!%XPOS%!$xpos!; s!%YPOS%!$ypos!; s!%ZPOS%!$zpos!" resources/template.rp > tmp.rp

  echo "./raytracer -I tmp.rp -W $width -H $height"
  ./raytracer -I tmp.rp -W $width -H $height

  cp tmp.tga animation/tmp_$frame.tga
  mv tmp.tga animation/tmp_$((frame+120)).tga
  sleep 2
done
