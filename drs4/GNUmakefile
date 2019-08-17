# Makefile for Stntuple/alg package
#############################################################
# include file products
INC = 

# library product
USESHLIBS   = 1
LINK_SHARED = 1

export USESHLIBS
export LINK_SHARED
# to use JetUser includes
CPPFLAGS     += -D__NO_CDFSOFT__ -DHAVE_LIBUSB -DUSE_DRS_MUTEX

SHAREDLIB  = libdrs_drs4.so

skip_cpp_files = drs_exam.cpp drs_pet.cpp drscl.cpp main.cpp 

LIBCPPFILES = $(filter-out $(skip_cpp_files), $(wildcard *.cpp))

LIBCFILES  = $(wildcard *.c)

LIBFFILES  = $(wildcard *.F)

HAVE_WX       = $(shell which wx-config)
ifeq ($(HAVE_WX),)
$(error Error: wxWidgets required to compile "drsosc")
endif

# wxWidgets libs and flags
WXLIBS        = $(shell wx-config --libs)
WXFLAGS       = $(shell wx-config --cxxflags)

BINS       = drsosc
COMPLEXBIN = $(BINS)

BINFFILES  = 
BINCPPFILES = main.cpp

# subdirectories
# SUBDIRS = dict

override CPPFLAGS += -I$(SRT_TOP)/include/drs/drs4 $(WXFLAGS)

override LINK_root       += drs4
override LINK_root_glibs += drs4

override PACKAGELIST = 1
include PackageList/link_all.mk
##############################################################################

vpath %.hh $(SRT_TOP)/include/drs/drs4
vpath %.h  $(SRT_TOP)/include/drs/drs4

##############################################################################
# force DEBUG mode (see SRT_CDF/special/compilers/KCC.mk
##############################################################################
override SRT_QUAL := debug


override LOADLIBES += -ldrs_drs4 $(ROOTGLIBS) $(WXLIBS) -lusb
override LOADLIBES := $(filter-out -ldcap   , $(LOADLIBES))
override LOADLIBES := $(filter-out -lDCache , $(LOADLIBES))

include SoftRelTools/standard.mk
# include SoftRelTools/refresh.mk

override LDFLAGS += -v

ifdef DEBUG
  x1 := $(shell echo BINLIBS   = $(BINLIBS) >& 2)
  x2 := $(shell echo LOADLIBES = $(LOADLIBES) >& 2)
endif

lib: codegen

ifneq (,$(findstring standalone,$(BUILD)))
  override CPPFLAGS := $(filter-out -I$(CDFSOFT2_DIR)/include, $(CPPFLAGS) )
endif
