set -euo pipefail

timer() {
  #echo -n compiling $0 "... "
  start=$(date +%s%N)
  $@
  end=$(date +%s%N)
  elapsed_ns=$((end - start))
  elapsed_ms=$((elapsed_ns / 1000000))
  echo ": ${elapsed_ms} ms"
}

CXX=gcc
CXXFLAGS="-std=c23 -fPIC -Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-variable -Wno-unused-function -Wno-unused-but-set-variable -Wpedantic -O3"

mkdir -p tables/build
echo -n "comp tables"; timer $CXX tables/main.c $CXXFLAGS -Wl,--no-undefined -o tables/build/tables -lm
echo -n "run tables"; timer tables/build/tables

mkdir -p build
echo -n "log"; timer $CXX -c src/log.c -o build/log.o $CXXFLAGS
echo -n "factory"; timer $CXX -c src/factory.c -o build/factory.o $CXXFLAGS
echo -n "plugindescs"; timer $CXX -c src/plugindescs.c -o build/plugindescs.o $CXXFLAGS
echo -n "tables"; timer $CXX -c src/tables.c -o build/tables.o $CXXFLAGS
echo -n "osc"; timer $CXX -c src/osc.c -o build/osc.o $CXXFLAGS
echo -n "util"; timer $CXX -c src/util.c -o build/util.o $CXXFLAGS
echo -n "p000"; timer $CXX -c src/p000.c -o build/p000.o $CXXFLAGS
#echo -n "p001"; timer $CXX -c src/p001.c -o build/p001.o $CXXFLAGS
echo -n "main"; timer $CXX -shared src/main.c $CXXFLAGS -Wl,--no-undefined -o build/lowlevelplugs.clap -lm \
  build/log.o \
  build/factory.o \
  build/plugindescs.o \
  build/tables.o \
  build/osc.o \
  build/util.o \
  build/p000.o \
  #build/p001.o \
  #raylib/src/libraylib.a \
  #/usr/lib/libcsound64.so \
  #-ljack \

sudo cp build/lowlevelplugs.clap /usr/lib/clap