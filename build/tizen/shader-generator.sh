#!/bin/bash

# Set Colors
Red='\033[0;31m'          # Red
Yellow='\033[0;33m'       # Yellow
Blue='\033[0;34m'         # Blue
Clear='\033[0m'       # Text Reset

#########################################################################################

Usage()
{
  echo -e "${Yellow}Usage: $(basename ${BASH_SOURCE[0]}) [indir] [outdir] [OPTIONS]"
  echo
  echo -e " ${Blue}Mandatory parameters:${Clear}"
  echo -e "  indir       The input directory where the original shader files are located"
  echo -e "  outdir      The output directory where the header files for the shaders should be generated"
  echo -e " ${Red}NOTE: All the above parameters must be provided${Clear}"
  echo
  echo -e " ${Blue}Options:${Clear}"
  echo -e "  -s|--skip   Skip the generation of header and cpp files that include all the generated shader header files"
  echo -e "  -h|--help   Help"
}

if [ "$1" = "-h" ] || [ "$1" = "--help" ]
then
  Usage
  exit 0
elif [ "$#" -lt 2 ]
then
  echo -e "${Red}ERROR:${Clear} Mandatory parameters are not provided"
  echo
  Usage
  exit 1
fi

#########################################################################################

indir=$1
outdir=$2
skip=""

for option in $*
do
  if [ "$option" = "-s" ] || [ "$option" = "--skip" ]
  then
    skip="--skip"
    continue
  elif [[ $option == -* ]]
  then
    echo -e "${Red}Invalid Option: ${Blue}$option${Clear}"
    echo
    Usage
    exit 1
  fi
done

#########################################################################################

mkdir -p $outdir

if [ ! -e $indir ] ; then
    echo "Error: The folder \""$indir"\" does not exist!"
    exit 1
fi

cd $indir
all_shaders=$(ls -1 *.{vert,frag,def} 2>/dev/null)
cd $OLDPWD

# Generate one header file per shader which is defined as a const std::string_view
for name in $all_shaders ; do
  echo "Generating header files for $name..."
  varname=$(echo "SHADER_$name" | tr [a-z] [A-Z] | sed -e 's/-/_/g;s/\./_/g;')

  newname=$(echo ${name} | sed -e 's/\./-/;')".h"
  echo Writing $newname

  shader_fullpath=$(echo ${indir})$name

  header_name="${varname}_GEN_H"
  echo "const std::string_view" "$varname""{" > $outdir/$newname
  cat $shader_fullpath | sed -e 's/^..*$/"&\\n"/' >> $outdir/$newname
  echo "};" >> $outdir/$newname
done

if [ "$skip" != "--skip" ]; then
  # Generate one cpp file that includes all the previously generated string_views for shaders
  echo "Generating cpp file..."
  echo -e "#include \"../builtin-shader-extern-gen.h\"\n" > $outdir/builtin-shader-gen.cpp

  varnames=
  for name in $all_shaders ; do
    varname=$(echo "SHADER_$name" | tr [a-z] [A-Z] | sed -e 's/-/_/g;s/\./_/g;')
    newname=$(echo ${name} | sed -e 's/\./-/;')".h"
    varnames="${varnames} $varname"
    echo "#include \"$newname\"" >> $outdir/builtin-shader-gen.cpp
  done

  # Generate one header file that defines all the shader string_views as extern variables
  echo "Generating extern header file ( for external use )..."
  echo "#ifndef GRAPHICS_BUILTIN_SHADER_EXTERN_GEN_H" > $outdir/../builtin-shader-extern-gen.h
  echo -e "#define GRAPHICS_BUILTIN_SHADER_EXTERN_GEN_H\n" >> $outdir/../builtin-shader-extern-gen.h

  echo "#include <string_view>" >> $outdir/../builtin-shader-extern-gen.h
  echo "" >> $outdir/../builtin-shader-extern-gen.h

  for name in $all_shaders ; do
    varname=$(echo "SHADER_$name" | tr [a-z] [A-Z] | sed -e 's/-/_/g;s/\./_/g;')
    newname=$(echo ${name} | sed -e 's/\./-/;')".h"
    echo "extern const std::string_view $varname;" >> $outdir/../builtin-shader-extern-gen.h
  done
  cat >> $outdir/../builtin-shader-extern-gen.h << EOF

#endif // GRAPHICS_BUILTIN_SHADER_EXTERN_GEN_H
EOF
fi

