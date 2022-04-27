#
#
LIBNAME=libCrowAndroid
MAJORVERSION=0
MINORVERSION=1
RELEASEVERSION=0
STATICNAME=${LIBNAME}.${MAJORVERSION}.${MINORVERSION}.${RELEASEVERSION}.a
STATICSHORTNAME=${LIBNAME}.a
LIBINSTALLPATH=../libs
HDRINSTALLPATH=../include/Android

include ../common.make

OWNER=mduffy
GROUP=mduffy
CTAGS= ctags -x >tags
CFLAGS= -Wall -DLINUX -DGL_GLEXT_PROTOTYPES -I../include/ -I../ -isystem $(ANDROID_NDK)/platforms/$(ANDROID_PLATFORM)/arch-x86/usr/include/ -fpic
DEBUGFLAGS= -g -DDEBUG
OPTIMIZEFLAGS= #-O2
#-ansi
# remove symbol table and relcoation info from executable
LDFLAGS= -lc  # -lGL -lGLU
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
    AndroidHelper.cpp \
    BaseApp.cpp \
    EglUtil.cpp \
    NativeActivity.cpp \
    SceneDelgate.cpp \
    Analytics.cpp \
    AnalyticsAdapter.cpp \
    AnalyticsFirebase.cpp \
    GLTestSceneDelegate.cpp \
    TestOpenGLES2.cpp

CSRCS=

SRCS_UNITTEST=\

#    main_unittest.cpp \

OBJS=$(SRCS:.cpp=.o)
COBJS=$(CSRCS:.c=.o)
OBJS_UNITTEST=$(SRCS_UNITTEST:.cpp=.o)
OBJPATH=./obj
DEPEND= makedepend $(CFLAGS)
VPATH=$(OBJPATH)

# Note: Android only builds a staic library, and copies headers to the release
#         directory.

all: ${STATICNAME}

$(STATICNAME): $(OBJS) $(SRCS) $(COBJS) $(CSRCS)
	@echo ""
	@echo "Archiving $(STATICNAME)"
	$(ARCHIVE) ${ARFLAGS} $@ $(OBJS:%=$(OBJPATH)/%) $(COBJS:%=$(OBJPATH)/%)
	@$(INSTALL) -D -m a+r ${STATICNAME} ${LIBINSTALLPATH}/${STATICNAME}
	@cd ${LIBINSTALLPATH}; ln -fs ${STATICNAME} ${STATICSHORTNAME}; )


clean:
	rm -f ${OBJPATH}/*.o ${OBJPATH}/*/*.o ./*.o $(STATICNAME)

Clean:
	rm -f ${OBJPATH}/*.o ${OBJPATH}/*/*.o ./*.o $(STATICNAME)

debug:
	@$(MAKE) CFLAGS='$(CFLAGS) $(DEBUGFLAGS)' LIBS='$(LIBS) $(DEBUGLIBS)'

docs:
	doxygen Doxyfile > doxygen.log


%.o : %.cpp %.hpp
	@echo ""
	@echo "Compling $(@)"
	@mkdir -p $(OBJPATH)
	$(CC) $(CFLAGS) -o $(OBJPATH)/$(@) -c $(@:.o=.cpp) -DCROW_VERSION=\"${MAJORVERSION}.${MINORVERSION}.${RELEASEVERSION}\"
	@$(INSTALL) -D -m a+r $(@:.o=.hpp) ${HDRINSTALLPATH}/$(@:.o=.hpp)

%.o : %.cpp
	@echo ""
	@echo "Compling $(@)"
	@mkdir -p $(OBJPATH)
	$(CC) $(CFLAGS) -o $(OBJPATH)/$(@) -c $(@:.o=.cpp) -DCROW_VERSION=\"${MAJORVERSION}.${MINORVERSION}.${RELEASEVERSION}\"

%.o : %.c %.h
	@echo ""
	@echo "Compling $(@)"
	@mkdir -p $(OBJPATH)
	$(CC) $(CFLAGS) -o $(OBJPATH)/$(@) -c $(@:.o=.c) -DCROW_VERSION=\"${MAJORVERSION}.${MINORVERSION}.${RELEASEVERSION}\"
	@$(INSTALL) -D -m a+r $(@:.o=.h) ${HDRINSTALLPATH}/$(@:.o=.h)













