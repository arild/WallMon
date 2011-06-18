# In order to force static linking:  -Wl,-Bstatic [libs] -Wl,-Bdynamic



CFLAGS_SO_PREPARE = -c -fPIC
CFLAGS_SO_CREATE = -shared

ifeq ($(PLATFORM), Darwin)	
	CFLAGS_SO_CREATE+=-undefined dynamic_lookup
endif

INSTALL_CMD=mv *.so ../bin
