FROM emscripten/emsdk:1.39.17-upstream

VOLUME [ "/repo" ]
WORKDIR /repo/bridge

ENTRYPOINT [ "make", "-f", "Makefile.emscripten" ]