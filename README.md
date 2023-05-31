## Prerequisites

- CMake
- clang\gcc\Visual Studio
- libCURL

## Install dependencies

Ubuntu:
```shell
sudo apt-get install -y libcurl4-openssl-dev
```

MSYS2 (MinGW):
```shell
pacman -S libcurl-devel
```

Or install the [curl](https://curl.se/) library manually and add to your project.

## Build

```shell
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . --config Release 

```
## Run

```shell
cd build
./SUPDXFD_15226
```