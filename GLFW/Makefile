#
#
LIBNAME=libCrowGLFW
MAJORVERSION=0
MINORVERSION=1
RELEASEVERSION=0
STATICNAME=${LIBNAME}.${MAJORVERSION}.${MINORVERSION}.${RELEASEVERSION}.a
STATICSHORTNAME=${LIBNAME}.a
LIBINSTALLPATH=../libs
HDRINSTALLPATH=../include/GLFW

OWNER=mduffy
GROUP=mduffy
CTAGS= ctags -x >tags
CFLAGS= -Wall -DLINUX -DGL_GLEXT_PROTOTYPES -I../ -I../include/ -fpic -I/home/mduffy/Libs/glfw-3.2.1/include/
DEBUGFLAGS= -g -DDEBUG
OPTIMIZEFLAGS= #-O2
#-ansi
# remove symbol table and relcoation info from executable
LDFLAGS= -lc -L/home/mduffy/Libs/glfw-3.2.1/src -lglfw3 -lGL -lGLU -lXrandr -lXext -lX11 -lXxf86vm -lXinerama -lXcursor  -lpthread -lm -ldl
LDSHAREDLIB= -shared -Wl,-soname,${SONAME}
LDSTATIC= ../base/libCrowBase.a
LDFLAGS_UNITTEST= -lgtest -L/usr/lib/ -lstdc++ -lm -lGL -lGLU -lpthread -lz
ARFLAGS= rcs

#CC=g++
#C=gcc
CC=colorgcc
C=colorgcc
SHAR=shar
GET=cvs co
INSTALL=install
LDCONFIG=/sbin/ldconfig
ARCHIVE=ar

MANDIR=
BINDIR=
# RBaseSparseArray.cpp

SRCS=\
    BaseApp.cpp \
    GLFWApp.cpp \
    EglUtil.cpp \
    NativeActivity.cpp \
    GLTestSceneDelegate.cpp \
    TestOpenGLES2.cpp \

CSRCS=

SRCS_UNITTEST=\

#    main_unittest.cpp \

OBJS=$(SRCS:.cpp=.o)
COBJS=$(CSRCS:.c=.o)
OBJS_UNITTEST=$(SRCS_UNITTEST:.cpp=.o)
OBJPATH=./obj
DEPEND= makedepend $(CFLAGS)
VPATH=$(OBJPATH)


all: ${STATICNAME}

$(STATICNAME): $(OBJS) $(SRCS) $(COBJS) $(CSRCS)
	@echo ""
	@echo "Archiving $(STATICNAME)"
	$(ARCHIVE) ${ARFLAGS} $@ $(OBJS:%=$(OBJPATH)/%) $(COBJS:%=$(OBJPATH)/%)
	@$(INSTALL) -D -m a+r ${STATICNAME} ${LIBINSTALLPATH}/${STATICNAME}
	@cd ${LIBINSTALLPATH}; ln -fs ${STATICNAME} ${STATICSHORTNAME};


clean:
	rm -f ${OBJPATH}/*.o ${OBJPATH}/*/*.o ./*.o $(STATICNAME)

Clean:
	rm -f ${OBJPATH}/*.o ${OBJPATH}/*/*.o ./*.o $(STATICNAME)

debug:
	@$(MAKE) CFLAGS='$(CFLAGS) $(DEBUGFLAGS)' LIBS='$(LIBS) $(DEBUGLIBS)'

docs:
	doxygen Doxyfile > doxygen.log

# Remove @ at start of $(CC) line to reveal command used for compiling

%.o : %.cpp %.hpp
#	@echo ""
	@echo "Compling $(@)"
	@mkdir -p $(OBJPATH)
	@$(CC) $(CFLAGS) -o $(OBJPATH)/$(@) -c $(@:.o=.cpp) -DCROW_VERSION=\"${MAJORVERSION}.${MINORVERSION}.${RELEASEVERSION}\"
	@$(INSTALL) -D -m a+r $(@:.o=.hpp) ${HDRINSTALLPATH}/$(@:.o=.hpp)

%.o : %.cpp
#	@echo ""
	@echo "Compling $(@)"
	@mkdir -p $(OBJPATH)
	@$(CC) $(CFLAGS) -o $(OBJPATH)/$(@) -c $(@:.o=.cpp) -DCROW_VERSION=\"${MAJORVERSION}.${MINORVERSION}.${RELEASEVERSION}\"

%.o : %.c %.h
#	@echo ""
	@echo "Compling $(@)"
	@mkdir -p $(OBJPATH)
	@$(CC) $(CFLAGS) -o $(OBJPATH)/$(@) -c $(@:.o=.c) -DCROW_VERSION=\"${MAJORVERSION}.${MINORVERSION}.${RELEASEVERSION}\"
	@$(INSTALL) -D -m a+r $(@:.o=.h) ${HDRINSTALLPATH}/$(@:.o=.h)













