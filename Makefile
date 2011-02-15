
DIRS = 	lib \
		monitors \
		daemon \
		server

# Dummy targets for building and clobbering everything in all subdirectories
all: 	
	@ for dir in ${DIRS}; do (cd $${dir}; ${MAKE}) ; done

clean: 	
	@ for dir in ${DIRS}; do (cd $${dir}; ${MAKE} clean) ; done
