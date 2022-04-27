#
#
LIBNAME=libCrowGameTech
PROGNAMETEST=CrowGameTechTest
MAJORVERSION=0
MINORVERSION=1
RELEASEVERSION=0
BUILD_INSTALL=${PROJECT_ROOT}/tools
SONAME=${LIBNAME}.so.${MAJORVERSION}
FULLLIBNAME=${SONAME}.${MINORVERSION}.${RELEASEVERSION}
STATICNAME=${LIBNAME}.${MAJORVERSION}.${MINORVERSION}.${RELEASEVERSION}.a
STATICSHORTNAME=${LIBNAME}.a
LIBINSTALLPATH=../libs
HDRINSTALLPATH=../include/GameTech

OWNER=mduffy
GROUP=mduffy
CTAGS= ctags -x >tags
CFLAGS= -Wall -DLINUX -DGL_GLEXT_PROTOTYPES -I../ -I../include/ -fpic
DEBUGFLAGS= -g -DDEBUG
OPTIMIZEFLAGS= #-O2
#-ansi
# remove symbol table and relcoation info from executable
#LDFLAGS= -lc  # -lGL -lGLU
LDFLAGS= -L$(HOME)/lib -L/home/mduffy/Libs/glfw-3.2.1/src -lz -lglfw3 -lGL -lGLU -lGLEW -lEGL -lGLESv2 -lXrandr -lXext -lX11 -lXxf86vm -lXinerama -lXcursor -lpthread -lm -ldl
LDSHAREDLIB= -shared -Wl,-soname,${SONAME}
LDSTATIC= ../libs/libCrowCommand.a \
	  ../libs/libCrowComponent.a \
          ../libs/libCrowGameTech.a \
          ../libs/libCrowBase.a \
          /usr/lib/x86_64-linux-gnu/libpng.a

#LDFLAGS_UNITTEST= -lgtest -L/usr/lib/ -lstdc++ -lm -lGL -lGLU -lpthread -lz
LDFLAGS_UNITTEST= -L$(HOME)/lib -L/home/mduffy/Libs/glfw-3.2.1/src -lz -lglfw3 -lGL -lGLU -lGLEW -lEGL -lGLESv2 -lXrandr -lXext -lX11 -lXxf86vm -lXinerama -lXcursor -lgtest -lpthread -lm -ldl -L/usr/lib/ -lstdc++

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

SRCS=\
    CurrencyManager.cpp \
    CurrencyRegenTimed.cpp \
    LootManager.cpp \
    UIHelper.cpp \
    RegisterGameTech.cpp \
    WindowBaseComponent.cpp \
    ScreenComponent.cpp \
    DialogComponent.cpp \
    IntroOutroComponent.cpp \
    FnGUI.cpp \
    FnValueRegistry.cpp \
    TweenSetComponent.cpp \

HDRS=\
#    CPlane.hpp \

CSRCS=

SRCS_UNITTEST=\
    main_unittest.cpp \
    CurrencyManager_unittest.cpp \
    CurrencyRegenTimed_unittest.cpp \
    LootManager_unittest.cpp \

OBJS=$(SRCS:.cpp=.o)
COBJS=$(CSRCS:.c=.o)
OBJS_UNITTEST=$(SRCS_UNITTEST:.cpp=.o)
OBJPATH=./obj
DEPEND= makedepend $(CFLAGS)
VPATH=$(OBJPATH)

all: ${FULLLIBNAME} ${STATICNAME} ${PROGNAMETEST}
#	@echo "Testing all\n"
#	echo $(OBJS)
# get things out of CVS
#$(SRCS):
#	$(GET) $@
	@echo "\n\nUnit Testing\n"
	@./${PROGNAMETEST}

test: ${PROGNAMETEST}
	@echo "\n\nUnit Testing\n"
	./${PROGNAMETEST}

# to make an executable

$(FULLLIBNAME): $(OBJS) $(SRCS) $(COBJS) $(CSRCS)
	@echo ""
	@echo "Linking $(FULLLIBNAME)"
	$(CC) -o $@ $(OBJS:%=$(OBJPATH)/%) $(COBJS:%=$(OBJPATH)/%) $(LDFLAGS) ${LDSHAREDLIB}
	@$(INSTALL) -D -m a+r ${FULLLIBNAME} ${LIBINSTALLPATH}/${FULLLIBNAME}
	@$(LDCONFIG) -n ${LIBINSTALLPATH}

$(STATICNAME): $(OBJS) $(SRCS) $(COBJS) $(CSRCS)
	@echo ""
	@echo "Archiving $(STATICNAME)"
	@$(ARCHIVE) ${ARFLAGS} $@ $(OBJS:%=$(OBJPATH)/%) $(COBJS:%=$(OBJPATH)/%)
	@$(INSTALL) -D -m a+r ${STATICNAME} ${LIBINSTALLPATH}/${STATICNAME}
	@cd ${LIBINSTALLPATH}; ln -fs ${STATICNAME} ${STATICSHORTNAME};

$(PROGNAMETEST): $(OBJS) $(SRCS) $(OBJS_UNITTEST) $(SRCS_UNITTEST) $(COBJS) $(CSRCS)
	@echo ""
	@echo "Linking $(PROGNAMETEST)"
	mkdir -p $(OBJPATH)
	$(CC) -o $@ $(OBJS:%=$(OBJPATH)/%) $(OBJS_UNITTEST:%=$(OBJPATH)/%) $(COBJS:%=$(OBJPATH)/%) ${LDSTATIC} $(LDFLAGS_UNITTEST)

clean:
	rm -f ${OBJPATH}/*.o ${OBJPATH}/*/*.o ./*.o ${FULLLIBNAME} $(STATICNAME)

Clean:
	rm -f ${OBJPATH}/*.o ${OBJPATH}/*/*.o ./*.o ${FULLLIBNAME} $(STATICNAME)

debug:
	@$(MAKE) CFLAGS='$(CFLAGS) $(DEBUGFLAGS)' LIBS='$(LIBS) $(DEBUGLIBS)'


install:
	$(INSTALL) -m 0664 -g $(GROUP) $(FULLLIBNAME) ${HOME}/lib/$(FULLLIBNAME)
	$(LDCONFIG) -n ${HOME}/lib
uninstall:
	rm ${HOME}/lib/$(FULLLIBNAME)
release:
	$(INSTALL) -m 0664 -g $(GROUP) $(FULLLIBNAME) ${BUILD_INSTALL}/lib/${FULLLIBNAME}
	$(LDCONFIG) -n ${BUILD_INSTALL}/lib
	@ln -s -f ${BUILD_INSTALL}/lib/${FULLLIBNAME} ${BUILD_INSTALL}/lib/${LIBNAME}.so

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





