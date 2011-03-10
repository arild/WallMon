# In order to force static linking:  -Wl,-Bstatic [libs] -Wl,-Bdynamic

CFLAGS_SO_PREPARE = -c -fPIC
CFLAGS_SO_CREATE = -shared
INSTALL_CMD=mv *.so ../bin
