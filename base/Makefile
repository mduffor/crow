#
#
LIBNAME=libCrowBase
PROGNAMETEST=CrowBaseTest
MAJORVERSION=0
MINORVERSION=2
RELEASEVERSION=0
BUILD_INSTALL=${PROJECT_ROOT}/tools
SONAME=${LIBNAME}.so.${MAJORVERSION}
FULLLIBNAME=${SONAME}.${MINORVERSION}.${RELEASEVERSION}
STATICNAME=${LIBNAME}.${MAJORVERSION}.${MINORVERSION}.${RELEASEVERSION}.a
STATICSHORTNAME=${LIBNAME}.a
LIBINSTALLPATH=../libs
HDRINSTALLPATH=../include

OWNER=mduffy
GROUP=mduffy
CTAGS= ctags -x >tags
CFLAGS= -Wall -DLINUX -DGL_GLEXT_PROTOTYPES -I./ -I./base/ -I./mesh/ -fpic -std=c++11
DEBUGFLAGS= -g -DDEBUG
OPTIMIZEFLAGS= #-O2
#-ansi
# remove symbol table and relcoation info from executable
LDFLAGS= -lc  # -lGL -lGLU -lGLEW
LDSHAREDLIB= -shared -Wl,-soname,${SONAME}
LDFLAGS_UNITTEST= -lgtest -L/usr/local/lib/ -lstdc++ -lm -lGL -lGLU -lGLEW -lpthread
ARFLAGS= rcs
# -lgtest_main

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
    Sys/Types.cpp \
    Debug.cpp \
    Util/RStr.cpp \
    Util/RStrParser.cpp \
    Sys/Timer.cpp \
    Sys/DeviceTime.cpp \
    Sys/Shell.cpp \
    Sys/TKeyValuePair.cpp \
    Gfx/GLUtil.cpp \
    Util/RegEx.cpp \
    Sys/FilePath.cpp \
    Math/RVec.cpp \
    Math/RMatrix.cpp \
    Math/TMatrix.cpp \
    Math/Rect.cpp \
    Math/RectSolver.cpp \
    Containers/BaseArray.cpp \
    Containers/RVecArray.cpp \
    Containers/RMatrixArray.cpp \
    Containers/IntArray.cpp \
    Containers/RStrArray.cpp \
    Containers/FloatArray.cpp \
    Containers/DoubleArray.cpp \
    Containers/PtrArray.cpp \
    Containers/BaseList.cpp \
    Containers/LinkedList.cpp \
    Gfx/Euler.cpp \
    Gfx/Transform.cpp \
    Gfx/Color8U.cpp \
    Gfx/TransformComponent.cpp \
    Gfx/Tween.cpp \
    Composite/Attr.cpp \
    Composite/AttrFloat.cpp \
    Composite/AttrInt.cpp \
    Composite/AttrString.cpp \
    Composite/AttrStringArray.cpp \
    Composite/Component.cpp \
    Composite/Node.cpp \
    Composite/World.cpp \
    Composite/NodeDelegate.cpp \
    Composite/SceneLoader.cpp \
    Composite/Resource.cpp \
    Composite/AttrBinarySerializer.cpp \
    Composite/AttrIntArray.cpp \
    Composite/AttrFloatArray.cpp \
    Sys/InputManager.cpp \
    Gfx/Curve.cpp \
    Gfx/ColorOps.cpp \
    Gfx/Noise.cpp \
    Gfx/Anim.cpp \
    Util/CalcHash.cpp \
    Util/XmlTools.cpp \
    Util/ParseTools.cpp \
    Util/NTP.cpp \
    Util/RandGen.cpp \
    Util/RandGenTinyMT.cpp \
    Util/Random.cpp \
    Composite/ComponentDefaultLooper.cpp \
    Composite/WakeNodesLooper.cpp \
    Composite/TriggerTracker.cpp \
    Net/Socket.cpp \
    Net/URLBuilder.cpp \
    Containers/KVPArray.cpp \
    Containers/KVPIntArray.cpp \
    Net/HTTP.cpp \
    Net/Base64.cpp \
    Net/RC4.cpp \
    Net/AnalyticsAdapter.cpp \
    Net/Analytics.cpp \
    Net/AnalyticsLog.cpp \
    ValueRegistry/ValueRegistry.cpp \
    ValueRegistry/ValueRegistrySimple.cpp \
    ValueRegistry/ConfigSubset.cpp \
    ValueRegistry/ConfigLayer.cpp \
    ValueRegistry/ConfigDeck.cpp \
    ValueRegistry/ContentDepot.cpp \
    Script/ExpressionToken.cpp \
    Script/Expression.cpp \
    Script/ExpressionDefaultFn.cpp \
    Script/InkScript.cpp \
    Script/InkParser.cpp \
    Script/InkExecute.cpp \
    Script/InkScriptManager.cpp \
    Script/SequenceManager.cpp \
    Script/TimerExpression.cpp \
    Sys/Output.cpp \
    Composite/NamedAttr.cpp \

HDRS=\
    Containers/TList.hpp \
    Containers/TArray.hpp \
    Util/Signal.h \
    Util/Delegate.h \


SRCS_UNITTEST=\
    Sys/main_unittest.cpp \
    Sys/FilePath_unittest.cpp \
    Sys/InputManager_unittest.cpp \
    Sys/Timer_unittest.cpp \
    Net/Base64_unittest.cpp \
    Net/RC4_unittest.cpp \
    Net/HTTP_unittest.cpp \
    Net/URLBuilder_unittest.cpp \
    Util/RStrParser_unittest.cpp \
    Util/RegEx_unittest.cpp \
    Util/CalcHash_unittest.cpp \
    ValueRegistry/ValueRegistry_unittest.cpp \
    ValueRegistry/Config_unittest.cpp \
    ValueRegistry/ContentDepot_unittest.cpp \
    Script/Expression_unittest.cpp \
    Script/InkScript_unittest.cpp \
    Script/InkScriptManager_unittest.cpp \
    Script/SequenceManager_unittest.cpp \
    Gfx/Color8U_unittest.cpp \
    Composite/Attr_unittest.cpp \
    Composite/AttrBinarySerializer_unittest.cpp \
    Composite/Component_unittest.cpp \
    Composite/Node_unittest.cpp \
    Containers/Containers_unittest.cpp \
    Containers/TList_unittest.cpp \
    Gfx/Anim_unittest.cpp \
    Util/ParseTools_unittest.cpp \
    Sys/DeviceTime_unittest.cpp \

#    HTTP_unittest.cpp \


CSRCS=
# iffCodec

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
	./${PROGNAMETEST}

test: ${PROGNAMETEST}
	@echo "\n\nUnit Testing\n"
	./${PROGNAMETEST}


# to make an executable

$(FULLLIBNAME): $(OBJS) $(SRCS) $(COBJS) $(CSRCS) $(HDRS)
	@echo ""
	@echo "Linking $(FULLLIBNAME)"
	$(CC) -o $@ $(OBJS:%=$(OBJPATH)/%) $(COBJS:%=$(OBJPATH)/%) ${LDFLAGS} ${LDSHAREDLIB}
	@$(INSTALL) -D -m a+r ${FULLLIBNAME} ${LIBINSTALLPATH}/${FULLLIBNAME}
	$(foreach file, $(HDRS), $(INSTALL) -D -m a+r $(file) ${HDRINSTALLPATH}/$(file);)
	@$(LDCONFIG) -n ${LIBINSTALLPATH}


$(STATICNAME): $(OBJS) $(SRCS) $(COBJS) $(CSRCS) $(HDRS)
	@echo ""
	@echo "Archiving $(STATICNAME)"
	@$(ARCHIVE) ${ARFLAGS} $@ $(OBJS:%=$(OBJPATH)/%) $(COBJS:%=$(OBJPATH)/%)
	@$(INSTALL) -D -m a+r ${STATICNAME} ${LIBINSTALLPATH}/${STATICNAME}
	@cd ${LIBINSTALLPATH}; ln -fs ${STATICNAME} ${STATICSHORTNAME};
	@$(foreach file, $(HDRS), $(INSTALL) -D -m a+r $(file) ${HDRINSTALLPATH}/$(file);)

$(PROGNAMETEST): $(OBJS) $(SRCS) $(OBJS_UNITTEST) $(SRCS_UNITTEST) $(COBJS) $(CSRCS) $(HDRS)
	@echo ""
	@echo "Linking $(PROGNAMETEST)"
#	@mkdir -p $(addprefix $(OBJPATH), $(dir $(SRCS)))
	$(CC) -o $@ $(OBJS:%=$(OBJPATH)/%) $(OBJS_UNITTEST:%=$(OBJPATH)/%) $(COBJS:%=$(OBJPATH)/%)  $(LDFLAGS_UNITTEST)

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
	@mkdir -p $(dir $(OBJPATH)/$(@))
	@$(CC) $(CFLAGS) -o $(OBJPATH)/$(@) -c $(@:.o=.cpp) -DCROW_VERSION=\"${MAJORVERSION}.${MINORVERSION}.${RELEASEVERSION}\"
	@$(INSTALL) -D -m a+r $(@:.o=.hpp) ${HDRINSTALLPATH}/$(@:.o=.hpp)

# Remove @ at start of $(CC) line to reveal command used for compiling
%.o : %.cpp
#	@echo ""
	@echo "Compling $(@)"
	@mkdir -p $(OBJPATH)
	@mkdir -p $(dir $(OBJPATH)/$(@))
	@$(CC) $(CFLAGS) -o $(OBJPATH)/$(@) -c $(@:.o=.cpp) -DCROW_VERSION=\"${MAJORVERSION}.${MINORVERSION}.${RELEASEVERSION}\"

# Remove @ at start of $(CC) line to reveal command used for compiling
%.o : %.c %.h
#	@echo ""
	@echo "Compling $(@)"
	@mkdir -p $(OBJPATH)
	@mkdir -p $(dir $(OBJPATH)/$(@))
	@$(CC) $(CFLAGS) -o $(OBJPATH)/$(@) -c $(@:.o=.c) -DCROW_VERSION=\"${MAJORVERSION}.${MINORVERSION}.${RELEASEVERSION}\"
	@$(INSTALL) -D -m a+r $(@:.o=.h) ${HDRINSTALLPATH}/$(@:.o=.h)


