#!/bin/bash -eu

./download-pkg.sh bee https://github.com/bec-ca/bee/archive/refs/tags/v1.1.0.tar.gz
./download-pkg.sh command https://github.com/bec-ca/command/archive/refs/tags/v1.0.0.tar.gz
./download-pkg.sh async https://github.com/bec-ca/async/archive/refs/tags/v1.0.0.tar.gz
./download-pkg.sh yasf https://github.com/bec-ca/yasf/archive/refs/tags/v1.0.0.tar.gz

for file in termino/*.cpp; do
  echo "Compiling $file..."
  clang++ -c $(cat compile_flags.txt) $file
done
