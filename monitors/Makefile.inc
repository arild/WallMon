
CFLAGS_SO_PREPARE = -shared -c -fPIC
CFLAGS_SO_CREATE = -lc -shared
INSTALL_CMD=mv *.so ../bin