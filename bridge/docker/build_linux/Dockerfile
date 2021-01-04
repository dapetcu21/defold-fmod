FROM debian:8

RUN apt-get update && apt-get install build-essential -y

VOLUME [ "/repo" ]
WORKDIR /repo/bridge

ENTRYPOINT [ "make", "-f", "Makefile.linux" ]