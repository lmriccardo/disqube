FROM ubuntu:latest

# Install required dependencies for compiling C++ executables
RUN apt update
RUN apt install -y g++ cmake make libssl-dev gdb gdbserver
RUN apt install -y libc6-dev gcc libtool automake autoconf build-essential

# Install other dependencies for networking
RUN apt install -y tcpdump iputils-ping iputils-tracepath iproute2

# Set the working folder and add all dependecies to compile
WORKDIR /usr/disqube/
COPY ./conf/ ./conf/
COPY ./src/ ./src/
COPY ./CMakeLists.txt ./CMakeLists.txt

# Compile the executable
RUN cmake -S . -B ./build -DCMAKE_BUILD_TYPE=Debug
RUN cmake --build ./build --target disqubed

# Remove all the unnecessary folder (cleaning up the environment)
RUN rm -rf ./disqube CMakeLists.txt ./build