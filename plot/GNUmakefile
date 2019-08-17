# Makefile for drs/plot subpackage
#############################################################
# include file products
INC = 

LINK_SHARED   = 1
# CPPFLAGS     += -D__NO_CDFSOFT__

export LINK_SHARED

# library product
SHAREDLIB  = libdrs_plot.so

LIBCCFILES = $(filter-out $(skip_cc_files), $(wildcard *.cc))

LIBFFILES  = $(wildcard *.F)

BINS       = 
COMPLEXBIN = 

BINFFILES  = 
BINCCFILES = 

# subdirectories
SUBDIRS = dict

CPPFLAGS += -I./plot
##############################################################################
vpath %.hh $(SRT_TOP)/include/$(CURPKG)
vpath %.h  $(SRT_TOP)/include/$(CURPKG)
##############################################################################
override SRT_QUAL := debug

include SoftRelTools/standard.mk
include SoftRelTools/refresh.mk
include SoftRelTools/arch_spec_root.mk

lib: codegen

ifdef LINK_SHARED
  override CPPFLAGS += -DLINK_SHARED
endif

# override CPPFLAGS := $(filter-out -I$(CDFSOFT2_DIR)/include, $(CPPFLAGS) )
