AC_PREREQ([2.69])
AC_INIT([setproxy], [0.1.0], [your-email@example.com])
AM_INIT_AUTOMAKE([foreign -Wall -Werror])

# Check for programs
AC_PROG_CC
AC_PROG_INSTALL

# Check for header files
AC_CHECK_HEADERS([arpa/inet.h netdb.h stdlib.h string.h sys/socket.h unistd.h])

# Check for C library functions
AC_CHECK_FUNCS([socket])

# Generate Makefiles
AC_CONFIG_FILES([Makefile])
AC_OUTPUT