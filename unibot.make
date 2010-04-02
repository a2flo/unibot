# GNU Make project makefile autogenerated by Premake
ifndef config
  config=debug32
endif

ifndef verbose
  SILENT = @
endif

ifndef CC
  CC = gcc
endif

ifndef CXX
  CXX = g++
endif

ifndef AR
  AR = ar
endif

ifeq ($(config),debug32)
  OBJDIR     = obj/x32/Debug
  TARGETDIR  = .
  TARGET     = $(TARGETDIR)/unibot
  DEFINES   += -D_GLIBCXX__PTHREADS -DPLATFORM_X86 -DDEBUG
  INCLUDES  += -I../../usr/include -I../../usr/local/include -I../../usr/include/SDL -I../../usr/include/SDL
  CPPFLAGS  += -MMD $(DEFINES) $(INCLUDES)
  CFLAGS    += $(CPPFLAGS) $(ARCH) -g -m32 -Wall -x c++ -fmessage-length=0 -pipe -Wno-trigraphs -Wreturn-type -Wunused-variable -funroll-loops -ftree-vectorize -msse3 -fvisibility=hidden -fvisibility-inlines-hidden `sdl-config --cflags`
  CXXFLAGS  += $(CFLAGS) 
  LDFLAGS   += -m32 -L/usr/lib32 `sdl-config --libs` -L../../usr/lib -L../../usr/lib -L../../usr/lib
  LIBS      += -lSDL -lSDLmain -lSDL_net -llua
  RESFLAGS  += $(DEFINES) $(INCLUDES) 
  LDDEPS    += 
  LINKCMD    = $(CXX) -o $(TARGET) $(OBJECTS) $(LDFLAGS) $(RESOURCES) $(ARCH) $(LIBS)
  define PREBUILDCMDS
  endef
  define PRELINKCMDS
  endef
  define POSTBUILDCMDS
  endef
endif

ifeq ($(config),release32)
  OBJDIR     = obj/x32/Release
  TARGETDIR  = .
  TARGET     = $(TARGETDIR)/unibot
  DEFINES   += -D_GLIBCXX__PTHREADS -DPLATFORM_X86 -DNDEBUG
  INCLUDES  += -I../../usr/include -I../../usr/local/include -I../../usr/include/SDL -I../../usr/include/SDL
  CPPFLAGS  += -MMD $(DEFINES) $(INCLUDES)
  CFLAGS    += $(CPPFLAGS) $(ARCH) -O2 -m32 -Wall -x c++ -fmessage-length=0 -pipe -Wno-trigraphs -Wreturn-type -Wunused-variable -funroll-loops -ftree-vectorize -msse3 -fvisibility=hidden -fvisibility-inlines-hidden `sdl-config --cflags` -O3
  CXXFLAGS  += $(CFLAGS) 
  LDFLAGS   += -s -m32 -L/usr/lib32 `sdl-config --libs` -L../../usr/lib -L../../usr/lib -L../../usr/lib
  LIBS      += -lSDL -lSDLmain -lSDL_net -llua
  RESFLAGS  += $(DEFINES) $(INCLUDES) 
  LDDEPS    += 
  LINKCMD    = $(CXX) -o $(TARGET) $(OBJECTS) $(LDFLAGS) $(RESOURCES) $(ARCH) $(LIBS)
  define PREBUILDCMDS
  endef
  define PRELINKCMDS
  endef
  define POSTBUILDCMDS
  endef
endif

ifeq ($(config),debug64)
  OBJDIR     = obj/x64/Debug
  TARGETDIR  = .
  TARGET     = $(TARGETDIR)/unibot
  DEFINES   += -D_GLIBCXX__PTHREADS -DPLATFORM_X64 -DDEBUG
  INCLUDES  += -I../../usr/include -I../../usr/local/include -I../../usr/include/SDL -I../../usr/include/SDL
  CPPFLAGS  += -MMD $(DEFINES) $(INCLUDES)
  CFLAGS    += $(CPPFLAGS) $(ARCH) -g -m64 -Wall -x c++ -fmessage-length=0 -pipe -Wno-trigraphs -Wreturn-type -Wunused-variable -funroll-loops -ftree-vectorize -msse3 -fvisibility=hidden -fvisibility-inlines-hidden `sdl-config --cflags`
  CXXFLAGS  += $(CFLAGS) 
  LDFLAGS   += -m64 -L/usr/lib64 `sdl-config --libs` -L../../usr/lib -L../../usr/lib -L../../usr/lib
  LIBS      += -lSDL -lSDLmain -lSDL_net -llua
  RESFLAGS  += $(DEFINES) $(INCLUDES) 
  LDDEPS    += 
  LINKCMD    = $(CXX) -o $(TARGET) $(OBJECTS) $(LDFLAGS) $(RESOURCES) $(ARCH) $(LIBS)
  define PREBUILDCMDS
  endef
  define PRELINKCMDS
  endef
  define POSTBUILDCMDS
  endef
endif

ifeq ($(config),release64)
  OBJDIR     = obj/x64/Release
  TARGETDIR  = .
  TARGET     = $(TARGETDIR)/unibot
  DEFINES   += -D_GLIBCXX__PTHREADS -DPLATFORM_X64 -DNDEBUG
  INCLUDES  += -I../../usr/include -I../../usr/local/include -I../../usr/include/SDL -I../../usr/include/SDL
  CPPFLAGS  += -MMD $(DEFINES) $(INCLUDES)
  CFLAGS    += $(CPPFLAGS) $(ARCH) -O2 -m64 -Wall -x c++ -fmessage-length=0 -pipe -Wno-trigraphs -Wreturn-type -Wunused-variable -funroll-loops -ftree-vectorize -msse3 -fvisibility=hidden -fvisibility-inlines-hidden `sdl-config --cflags` -O3
  CXXFLAGS  += $(CFLAGS) 
  LDFLAGS   += -s -m64 -L/usr/lib64 `sdl-config --libs` -L../../usr/lib -L../../usr/lib -L../../usr/lib
  LIBS      += -lSDL -lSDLmain -lSDL_net -llua
  RESFLAGS  += $(DEFINES) $(INCLUDES) 
  LDDEPS    += 
  LINKCMD    = $(CXX) -o $(TARGET) $(OBJECTS) $(LDFLAGS) $(RESOURCES) $(ARCH) $(LIBS)
  define PREBUILDCMDS
  endef
  define PRELINKCMDS
  endef
  define POSTBUILDCMDS
  endef
endif

OBJECTS := \
	$(OBJDIR)/bot_handler.o \
	$(OBJDIR)/bot_states.o \
	$(OBJDIR)/config.o \
	$(OBJDIR)/log.o \
	$(OBJDIR)/lua.o \
	$(OBJDIR)/lua_bindings.o \
	$(OBJDIR)/main.o \
	$(OBJDIR)/util.o \

RESOURCES := \

SHELLTYPE := msdos
ifeq (,$(ComSpec)$(COMSPEC))
  SHELLTYPE := posix
endif
ifeq (/bin,$(findstring /bin,$(SHELL)))
  SHELLTYPE := posix
endif

.PHONY: clean prebuild prelink

all: $(TARGETDIR) $(OBJDIR) prebuild prelink $(TARGET)

$(TARGET): $(GCH) $(OBJECTS) $(LDDEPS) $(RESOURCES)
	@echo Linking unibot
	$(SILENT) $(LINKCMD)
	$(POSTBUILDCMDS)

$(TARGETDIR):
	@echo Creating $(TARGETDIR)
ifeq (posix,$(SHELLTYPE))
	$(SILENT) mkdir -p $(TARGETDIR)
else
	$(SILENT) mkdir $(subst /,\\,$(TARGETDIR))
endif

$(OBJDIR):
	@echo Creating $(OBJDIR)
ifeq (posix,$(SHELLTYPE))
	$(SILENT) mkdir -p $(OBJDIR)
else
	$(SILENT) mkdir $(subst /,\\,$(OBJDIR))
endif

clean:
	@echo Cleaning unibot
ifeq (posix,$(SHELLTYPE))
	$(SILENT) rm -f  $(TARGET)
	$(SILENT) rm -rf $(OBJDIR)
else
	$(SILENT) if exist $(subst /,\\,$(TARGET)) del $(subst /,\\,$(TARGET))
	$(SILENT) if exist $(subst /,\\,$(OBJDIR)) rmdir /s /q $(subst /,\\,$(OBJDIR))
endif

prebuild:
	$(PREBUILDCMDS)

prelink:
	$(PRELINKCMDS)

ifneq (,$(PCH))
$(GCH): $(PCH)
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
endif

$(OBJDIR)/bot_handler.o: bot_handler.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/bot_states.o: bot_states.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/config.o: config.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/log.o: log.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/lua.o: lua.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/lua_bindings.o: lua_bindings.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/main.o: main.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJDIR)/util.o: util.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(CXXFLAGS) -o $@ -c $<

-include $(OBJECTS:%.o=%.d)
