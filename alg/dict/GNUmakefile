LINK_SHARED_MODULES = yes

ifdef LINK_SHARED
  SHAREDLIB       = libdrs_alg.so
else
  LIB             = libdrs_alg.a
  sharedlib_o_dir = $(staticlib_o_dir)
endif
lib_o_dir         = $(sharedlib_o_dir)

skip_files = 

LIBCXXFILES = $(filter-out $(skip_files), $(wildcard $(lib_o_dir)/*_dict.cxx))
###############################################################################
SUBPKG = drs/ana
CINT_SUBDIRS = $(SRT_TOP)/include/drs

vpath %.hh .:$(SRT_TOP)/include/drs/alg
vpath %.h  .:$(SRT_TOP)/include/drs/alg

override CPPFLAGS += -I$(SRT_TOP)/include/drs
override CPPFLAGS += -I$(SRT_TOP)/include/drs/alg
###############################################################################
NO_GENERATE_DEPENDS:=true
override SRT_QUAL  := debug
ifdef DEBUG
  x := $(shell echo SRT_TOP  = $(SRT_TOP) >&2)
  x := $(shell echo SRT_QUAL = $(SRT_QUAL)>&2)
endif

###############################################################################
include SoftRelTools/standard.mk
include SoftRelTools/refresh.mk

include SoftRelTools/arch_spec_root.mk
include RootUtils/arch_spec_rootcint.mk

lib: codegen















