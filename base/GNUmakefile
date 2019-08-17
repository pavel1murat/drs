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
CPPFLAGS     += -D__NO_CDFSOFT__

SHAREDLIB  = libdrs_base.so

skip_cc_files = InitStntupleDataset.cc TTrackIneffModule.cc \
                ces.cc ces_alignment.cc ces_book.cc ces_cluster.cc ces_gain.cc

LIBCCFILES = $(filter-out $(skip_cc_files), $(wildcard *.cc))

LIBFFILES  = $(wildcard *.F)

BINS       = 
COMPLEXBIN = 

BINFFILES  = 
BINCCFILES = 

# subdirectories
SUBDIRS = dict

override CPPFLAGS += -I$(SRT_TOP)/include/drs
override CPPFLAGS += -I$(SRT_TOP)/include/drs/base
##############################################################################

vpath %.hh $(SRT_TOP)/include/$(CURPKG)/base
vpath %.h  $(SRT_TOP)/include/$(CURPKG)/base

##############################################################################
# force DEBUG mode (see SRT_CDF/special/compilers/KCC.mk
##############################################################################
override SRT_QUAL := debug

include SoftRelTools/standard.mk
include SoftRelTools/refresh.mk
include SoftRelTools/arch_spec_root.mk

lib: codegen

ifneq (,$(findstring standalone,$(BUILD)))
  override CPPFLAGS := $(filter-out -I$(CDFSOFT2_DIR)/include, $(CPPFLAGS) )
endif
