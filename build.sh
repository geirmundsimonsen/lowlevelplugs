set -euo pipefail

timer() {
  #echo -n compiling $0 "... "
  start=$(date +%s%N)
  $@
  end=$(date +%s%N)
  elapsed_ns=$((end - start))
  elapsed_ms=$((elapsed_ns / 1000000))
  echo "... ${elapsed_ms} ms"
}

desc() {
  echo -n $1
  echo -n " "
}

CXX=gcc
CXXFLAGS="-std=c23 -O2 -fPIC -Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-variable -Wno-unused-function -Wno-unused-but-set-variable -Wpedantic"

mkdir -p tables/build
desc "comp tables"; timer $CXX tables/main.c $CXXFLAGS -Wl,--no-undefined -o tables/build/tables -lm
desc "run tables"; timer tables/build/tables

mkdir -p dist
desc "log"; timer $CXX -c src/log.c -o dist/log.o $CXXFLAGS
desc "factory"; timer $CXX -c src/factory.c -o dist/factory.o $CXXFLAGS
desc "plugindescs"; timer $CXX -c src/plugindescs.c -o dist/plugindescs.o $CXXFLAGS
desc "tables"; timer $CXX -c src/tables.c -o dist/tables.o $CXXFLAGS
desc "osc"; timer $CXX -c src/osc.c -o dist/osc.o $CXXFLAGS
desc "filter"; timer $CXX -c src/filter.c -o dist/filter.o $CXXFLAGS
desc "util"; timer $CXX -c src/util.c -o dist/util.o $CXXFLAGS
desc "p000"; timer $CXX -c src/p000.c -o dist/p000.o $CXXFLAGS
desc "p001"; timer $CXX -c src/p001.c -o dist/p001.o $CXXFLAGS
desc "p002"; timer $CXX -c src/p002.c -o dist/p002.o $CXXFLAGS
desc "main"; timer $CXX -shared src/main.c $CXXFLAGS -Wl,--no-undefined -o dist/lowlevelplugs.clap -lm \
  dist/log.o \
  dist/factory.o \
  dist/plugindescs.o \
  dist/tables.o \
  dist/osc.o \
  dist/filter.o \
  dist/util.o \
  dist/p000.o \
  dist/p001.o \
  dist/p002.o \
  #raylib/src/libraylib.a \
  #/usr/lib/libcsound64.so \
  #-ljack \

sudo cp dist/lowlevelplugs.clap /usr/lib/clap