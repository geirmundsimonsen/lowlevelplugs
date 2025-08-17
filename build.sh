set -euo pipefail

with_timer() {
  echo -n compiling $3 "... "
  start=$(date +%s%N)
  $@
  end=$(date +%s%N)
  elapsed_ns=$((end - start))
  elapsed_ms=$((elapsed_ns / 1000000))
  echo "${elapsed_ms} ms"
}

CXX=gcc
CXXFLAGS="-fPIC -Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-variable -Wno-unused-but-set-variable -Wpedantic -O3"
mkdir -p build

#$CXX tables/main.cpp $CXXFLAGS -Wl,--no-undefined -o tables/build/tables
#tables/build/tables

with_timer $CXX -c src/log.c -o build/log.o $CXXFLAGS
#with_timer $CXX -c src/tables.cpp -o build/tables.o $CXXFLAGS
#with_timer $CXX -c src/osc.cpp -o build/osc.o $CXXFLAGS
#with_timer $CXX -c src/plugindescs.cpp -o build/plugindescs.o $CXXFLAGS
#with_timer $CXX -c src/factory.cpp -o build/factory.o $CXXFLAGS
#with_timer $CXX -c src/util.cpp -o build/util.o $CXXFLAGS
#with_timer $CXX -c src/p000.cpp -o build/p000.o $CXXFLAGS
#with_timer $CXX -c src/p001.cpp -o build/p001.o $CXXFLAGS
with_timer $CXX -shared src/main.c $CXXFLAGS -Wl,--no-undefined -o build/testclapplugin.clap \
  #build/log.o \
  #build/tables.o \
  #build/osc.o \
  #build/plugindescs.o \
  #build/factory.o \
  #build/util.o \
  #build/p000.o \
  #build/p001.o \
  #raylib/src/libraylib.a \
  #/usr/lib/libcsound64.so \
  #-ljack \

#sudo cp build/testclapplugin.clap /usr/lib/clap/testclapplugin.clap