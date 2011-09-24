
DIRS = 	lib \
		modules \
		daemon \
		server

all: 	
	@ for dir in ${DIRS}; do (cd $${dir}; ${MAKE}) ; done

clean: 	
	@ for dir in ${DIRS}; do (cd $${dir}; ${MAKE} clean) ; done
