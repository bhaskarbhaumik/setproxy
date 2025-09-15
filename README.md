# SetProxy

A command-line utility for dynamically setting or unsetting HTTP/HTTPS proxy environment variables in your shell environment.

## Overview

`setproxy` is a lightweight C utility designed to simplify the management of proxy environment variables. It:

- Automatically tests connectivity to proxy servers before applying settings
- Sets both lowercase and uppercase environment variables (http_proxy, HTTP_PROXY, etc.)
- Falls back to a list of default proxies if no arguments are provided
- Includes a comprehensive NO_PROXY list to bypass the proxy for local addresses

## Usage

```bash
./setproxy [--unset|-u] [--version|-v] [--help|-h] [[proxy-host1:port1] [proxy-host2:port2] [proxy-host3:port3] ...]
```

### Examples

Set proxy using a specified server:

```bash
eval $(./setproxy 192.168.1.10:8080)
```

Set proxy using the first working default proxy:

```bash
eval $(./setproxy)
```

Unset all proxy environment variables:

```bash
eval $(./setproxy -u)
```

Show version information:

```bash
./setproxy -v
```

Display help information:

```bash
./setproxy -h
```

## Building from Source

### Prerequisites

- C compiler (clang or gcc)
- make, cmake (version 3.10 or higher), or autotools (autoconf, automake)

### Build Steps Using Make

1. Clone or download the repository
2. Navigate to the project directory
3. Run make:

```bash
make
```

This will create the `setproxy` executable in the current directory.

### Build Steps Using CMake

1. Clone or download the repository
2. Navigate to the project directory
3. Create a build directory and use CMake:

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

Alternatively, use the provided build script:

```bash
./build.sh
```

The build script accepts these options:

- `-c`: Clean build (removes build directory before building)
- `-i`: Install after building (requires sudo privileges)
- `-d BUILD_DIR`: Specify build directory (default: 'build')
- `-h`: Show help message

### Installation (Optional)

Using make:

```bash
sudo make install
```

Using CMake:

```bash
cd build
sudo cmake --install .
```

Or using the build script:

```bash
./build.sh -i
```

This will install the executable to `/usr/local/bin/`.

### Build Steps Using Autotools

1. Clone or download the repository
2. Navigate to the project directory
3. Generate the build system, configure, and build:

```bash
./autogen.sh
./configure
make
```

### Installation with Autotools

After building with Autotools:

```bash
sudo make install
```

This will install the executable to `/usr/local/bin/` by default. You can customize the installation path using the `--prefix` option with `configure`:

```bash
./configure --prefix=/your/custom/path
make
sudo make install
```

## How It Works

1. When called without arguments, `setproxy` will attempt to connect to each default proxy server in order.
2. When called with proxy arguments, it will try each one in the order specified.
3. For each proxy, it:
   - Tests TCP connectivity (with a 5-second timeout)
   - If successful, outputs export commands for all proxy environment variables
   - If unsuccessful, tries the next proxy in the list

The output needs to be evaluated by your shell to take effect, which is why the `eval $()` syntax is used in the examples.

## Environment Variables

`setproxy` sets the following environment variables:

- `http_proxy`, `HTTP_PROXY`
- `https_proxy`, `HTTPS_PROXY`
- `all_proxy`, `ALL_PROXY`
- `no_proxy`, `NO_PROXY`

## Notes

- The utility is designed to be used with the `eval` command in shell environments
- When run interactively (with output to a terminal), it displays an error message for security reasons
- Proxy connectivity is checked before setting variables to ensure the proxy is actually working

## Version

Current version: 0.1.0
