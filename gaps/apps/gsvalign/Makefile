#
# Application name and list of source files.
#

NAME=gsvalign
CCSRCS=$(NAME).cpp poseoptimization.cpp R3CatmullRomSpline.cpp


#
# Dependency libraries
#

#USER_CFLAGS=-I/usr/local/suitesparse -I/usr/local/include/eigen3 -DRN_USE_CERES -DRN_USE_MINPACK -DRN_USE_SPLM -DRN_USE_CSPARSE
#PKG_LIBS=-L/usr/local/lib -lGSV -lR3Graphics -lR3Shapes -lR2Shapes -lRNMath -lRNBasics -ljpeg -lpng -lklt -lceres -lsplm -lCSparse -lminpack -lccolamd -lcamd -lcolamd -lamd -Wl,-Bdynamic -lcxsparse -lcholmod -lccolamd -lcamd -lcolamd -lamd -lsuitesparseconfig -llapack -lblas -Wl,-Bstatic -lcxsparse -Wl,-Bdynamic -lgomp -lglog -lgflags 

USER_CFLAGS=-I/usr/local/suitesparse -I/usr/local/include/eigen3 -DRN_USE_CERES -DRN_USE_SPLM -DRN_USE_CSPARSE
PKG_LIBS=-L/usr/local/lib -lGSV -lR3Graphics -lR3Shapes -lR2Shapes -lRNMath -lRNBasics -ljpeg -lpng -lklt -lceres -lsplm -lCSparse -lccolamd -lcamd -lcolamd -lamd -Wl,-Bdynamic -lcxsparse -lcholmod -lccolamd -lcamd -lcolamd -lamd -lsuitesparseconfig -llapack -lblas -Wl,-Bstatic -lcxsparse -Wl,-Bdynamic -lgomp -lglog -lgflags 

#USER_CFLAGS=-DRN_USE_SPLM -DRN_USE_CSPARSE
#PKG_LIBS=-lGSV -lR3Graphics -lR3Shapes -lR2Shapes -lRNMath -lRNBasics -ljpeg -lpng -lsplm -lCSparse -lklt



#
# R3 application makefile
#

include ../../makefiles/Makefile.apps



