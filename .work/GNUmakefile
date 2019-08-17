###############################################################################
# This top level just specifies (via SUBDIRS) the subdirectories where
# actions are needed. Nothing is actually done in or to this directory.
###############################################################################
include SoftRelTools/standard.mk

idir   = $(SRT_TOP)/include

refresh: links
links:
	if [ .${VERBOSE} != "." ] ; then echo W-mode=${w_mode} ; fi
#
#  tmp cleanup
#
	chmod u+w ${idir}
	if [ -h ${idir}/drs ] ; then rm ${idir}/drs ; fi

	mkdir -p ${idir}/drs

	for pkg in base drs4 ana alg obj geom gui plot; do \
	  if [ .${VERBOSE} != "." ] ; then echo drs: linking pkg=$$pkg ; fi ;\
	  if [ ! -h ${idir}/drs/$$pkg  ] ; then \
	    ln -s ../../drs/$$pkg/$$pkg   ${idir}/drs/$$pkg  ; \
	  fi ; \
	done




