# Generates a dependency graph for Make. Without this, source files
# depending on a header file that has been changed, will NOT be re-compiled
depend: .depend
.depend:
	$(CC) $(INC_PATH) -MM $(shell find . -name \*.cpp) > $@
-include .depend