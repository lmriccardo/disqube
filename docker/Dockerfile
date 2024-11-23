FROM alpine:latest

# Install required dependencies for compiling C++ executables
RUN apk update
RUN apk add g++ cmake make openssl-dev gdb musl-dev
RUN apk add libc-dev gcc libtool automake autoconf linux-headers

# Install other dependencies for networking
RUN apk add tcpdump iputils

# Set the working folder and add all dependecies to compile
WORKDIR /usr/disqube/
COPY ../conf/ ./conf/
COPY ../disqube/ ./disqube/
COPY ../CMakeLists.txt ./CMakeLists.txt

# Compile the executable
RUN cmake -S . -B ./build -DCMAKE_BUILD_TYPE=Debug
RUN cmake --build ./build --target Main
RUN mv ./build/Main disqube

# Remove all the unnecessary folder (cleaning up the environment)
RUN rm -rf ./disqube CMakeLists.txt ./build