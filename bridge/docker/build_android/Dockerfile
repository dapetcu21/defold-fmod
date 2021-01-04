FROM lakoo/android-ndk:28-28.0.2-r17c

RUN apt-get update && apt-get install make -y

VOLUME [ "/repo" ]
WORKDIR /repo/bridge

ENTRYPOINT [ "make", "-f", "Makefile.android" ]