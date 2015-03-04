#!/bin/bash
#
# Script to assign meta data to images
#
# Usage: add_meta.sh metafile.xmp image-files
#
if [ $# -eq 0 ]
then
echo "Usage: add_meta metafile.xmp image-files"
else
echo "Processing meta information..."
template_file=$1
num_files=$#
xmp_ext=".xmp"
for (( i=1; i<$num_files; i++ ))
do
  file_name=$2
  echo $file_name
  base_name="${file_name%.*}"
  xmp_file=$base_name$xmp_ext
  cp $template_file $xmp_file
  exiv2 -ixX $file_name
  rm $xmp_file
  shift
done
fi

