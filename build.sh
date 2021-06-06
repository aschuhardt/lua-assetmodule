#!/bin/sh

function strip_binary {
  strip -v -S --strip-unneeded --remove-section=.note.gnu.gold-version \
    --remove-section=.comment --remove-section=.note \
    --remove-section=.note.gnu.build-id --remove-section=.note.ABI-tag $(find -name "*$1.*")
}

if [ -n "$1" ]; then
  module=$1
  asset=$2
  output=$3

  echo "Module name is $module for file $asset, to be placed in $output"

  # --------------------------------
  # set up the build directory

  builddir="build-$module"
  mkdir -p $builddir
  pushd $builddir
  cmake -DASSET_MODULE_NAME=$module -DCMAKE_BUILD_TYPE=Release ..


  # --------------------------------
  # build the compression utility

  make compress 


  # --------------------------------
  # create a header file with the contents of the asset file dumped into a byte array

  popd
  header="asset-data.h"
  echo "#ifndef ASSET_DATA_H" > $header
  echo "#define ASSET_DATA_H" >> $header
  echo "const unsigned long asset_size = $(stat -c %s $asset);" >> $header
  echo "const unsigned char asset_data[] = {" >> $header
  $builddir/compress $asset | xxd -i >> $header
  echo "};" >> $header
  echo "#endif" >> $header


  # --------------------------------
  # build the library

  pushd $builddir
  make all


  # --------------------------------
  # copy the newly-built library file to base directory

  popd
  mkdir -p $output
  cp $builddir/*$module.* $output
  cp $builddir/*miniz.* $output


  # --------------------------------
  # strip the binary down a bit
  # pushd $output
  # strip_binary $module
  # strip_binary $miniz
  # popd


  # --------------------------------
  # clean up

  rm -rf $builddir
  rm $header


  # --------------------------------
  # test the resulting library module

  pushd $output
  result_size=$(echo "print(require '$module'.length)" | lua)
  initial_size=$(stat -c %s ../$asset)
  echo "Initial (uncompressed) file size: $initial_size"
  echo "Size reported by the module (post-decompression): $result_size"

  popd
else
  echo "Usage: build.sh <module-name> <asset-file-path> <out-dir>"
  return
fi
