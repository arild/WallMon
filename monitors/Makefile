DIRS = 	boids2d
		#processall
		#fastlogger \
		#slowlogger
		#process 

all: 	
	@ for dir in ${DIRS}; do (cd $${dir}; ${MAKE}) ; done

clean: 	
	@ for dir in ${DIRS}; do (cd $${dir}; ${MAKE} clean) ; done
	rm -rf bin/*.so