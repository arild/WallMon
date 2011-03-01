
CFLAGS_SO_PREPARE = -static -shared -c -fPIC
CFLAGS_SO_CREATE = -static -lc -shared
INSTALL_CMD=mv *.so ../bin