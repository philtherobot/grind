#!/bin/bash

exe=grind.exe

if [[ "$1" == "clean" ]] ; then
	echo Make clean.
	rm $exe > /dev/null 2>&1
fi

if [[ -e grind ]] ; then

  echo Self-building.

  d=$PWD
  pushd $TMP > /dev/null

  cp -p $d/$exe .

  $d/$exe $d/grind.cpp

  mv $exe $d

  popd > /dev/null

else

  echo Bootstrapping.

  if [[ -z "$PSTREAM_DIR" ]] ; then
    echo error: PSTREAM_DIR is not defined.
    exit 1
  fi

  g++ -Wall -Wextra -Wpedantic -pthread --std=gnu++14 -I$PSTREAM_DIR \
    -o$exe \
    grind.cpp \
	/usr/local/lib/libyaml-cpp.a \
    -lboost_filesystem -lboost_system 

fi
