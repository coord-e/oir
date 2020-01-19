FROM gcc:latest

VOLUME /work
WORKDIR /work

RUN apt-get update
RUN apt-get install -y inotify-tools make clang clang-format

CMD ["make", "watch"]
