FROM gcc:latest

VOLUME /work
WORKDIR /work

RUN apt-get update
RUN apt-get install inotify-tools make

ENV CC=gcc

CMD ["make", "watch"]
