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

### With the data output

```shell
cd build
./SUPDXFD_15226
```

### Silent (timestamps and function names only)

```shell
cd build
./SUPDXFD_15226 silent
```

```
get_symbols_by_type::begin: 07.08.2023 18:14:07.138
get_symbols_by_type::end: 07.08.2023 18:14:09.410
get_symbols_by_type_starting_with_symbol_name::begin: 07.08.2023 18:14:09.413
get_symbols_by_type_starting_with_symbol_name::end: 07.08.2023 18:14:10.618
get_expire_date_of_option::begin: 07.08.2023 18:14:10.621
get_expire_date_of_option::end: 07.08.2023 18:14:14.232
get_call_or_put_strike_price_on_specific_expire_date_of_option::begin: 07.08.2023 18:14:14.236
get_call_or_put_strike_price_on_specific_expire_date_of_option::end: 07.08.2023 18:14:16.919  
get_call_or_put_strike_price_on_specific_expire_date_of_option::begin: 07.08.2023 18:14:16.925
get_call_or_put_strike_price_on_specific_expire_date_of_option::end: 07.08.2023 18:14:19.750
```
