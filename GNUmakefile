# Top level GNUmakefile for CalTrigger
#
# uses SoftRelTools/standard.mk
#
# This top level just specifies (via SUBDIRS) the subdirectories where
# actions are needed. Nothing is actually done in or to this directory.
#
#############################################################
# subdirectories

LINK_SHARED = 1
export LINK_SHARED

SUBDIRS = drs4 ana 
############################################################
include SoftRelTools/standard.mk

_shared: _drs4 _base _obj _geom _gui _alg _ana _plot

_alg:
	$(MAKE) -C alg USESHLIBS=1 nobin
_ana:
	$(MAKE) -C ana USESHLIBS=1     nobin
_base:
	$(MAKE) -C base USESHLIBS=1    nobin
_drs4:
	$(MAKE) -C drs4 USESHLIBS=1    nobin
_geom:
	$(MAKE) -C geom USESHLIBS=1    nobin
_gui:
	$(MAKE) -C gui USESHLIBS=1     nobin
_note001:
	$(MAKE) -C notes USESHLIBS=1   pdf
_obj:
	$(MAKE) -C obj  USESHLIBS=1    nobin
_plot:
	$(MAKE) -C plot  USESHLIBS=1   nobin
_work:
	$(MAKE) -C .work USESHLIBS=1   all
