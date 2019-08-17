#
SHAREDLIB       = libdrs_plot.so
staticlib_o_dir = $(sharedlib_o_dir)
lib_o_dir       = $(sharedlib_o_dir)

skip_files = # TCdfLljj_dict.cxx

LIBCXXFILES = $(filter-out $(skip_files), $(wildcard $(lib_o_dir)*_dict.cxx))
###############################################################################
SUBPKG = plot
CINT_SUBDIRS = $(SRT_TOP)/include/drs/plot:$(SRT_TOP)/include/drs

vpath %.hh .:$(SRT_TOP)/include/drs/plot
vpath %.h  .:$(SRT_TOP)/include/drs/plot

override CPPFLAGS += -I$(SRT_TOP)/include/drs
override CPPFLAGS += -I$(SRT_TOP)/include/drs/plot
###############################################################################
override SRT_QUAL := debug

include SoftRelTools/standard.mk
include SoftRelTools/refresh.mk

include SoftRelTools/arch_spec_root.mk
include RootUtils/arch_spec_rootcint.mk

lib: codegen


