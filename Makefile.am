AUTOMAKE_OPTIONS = foreign

bin_PROGRAMS = setproxy
setproxy_SOURCES = setproxy.c

# Add compilation flags
setproxy_CFLAGS = -Wall -Wextra -O2

# Install target (optional)
install-exec-hook:
	@echo "setproxy has been installed to $(bindir)"

uninstall-hook:
	@echo "setproxy has been uninstalled from $(bindir)"

# Define the distribution files
EXTRA_DIST = README.md CMakeLists.txt build.sh .gitignore cmake_clean.cmake