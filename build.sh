#!/bin/sh

out=cmake-build-debug
cmake -S. -B${out} -GNinja -DCMAKE_EXPORT_COMPILE_COMMANDS=On -DCMAKE_BUILD_TYPE=Debug
[ -f ${out}/compile_commands.json ] && ln -sf ${out}/compile_commands.json compile_commands.json
ninja -C ${out} && \
	ctest --test-dir ${out} --rerun-failed --output-on-failure
