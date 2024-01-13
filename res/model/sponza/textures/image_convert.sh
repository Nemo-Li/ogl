#! /bin/bash
for file in `ls .`
do
  if [ "tga" = "${file##*.}" ];then
  echo "convert $file ${file%.*}.png"
  fi
#  echo "${file%.*}.png"
done