FROM gcc:4.9
RUN mkdir /work
COPY main.cpp /work
RUN g++ /work/main.cpp -o /work/main.exe -std=c++17 -O2
ENTRYPOINT /work/main.exe
