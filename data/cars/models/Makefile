##############################################################################
#
#    file                 : Makefile
#    created              : Sat Mar 18 23:00:54 CET 2000
#    copyright            : (C) 2000 by Eric Espie                         
#    email                : torcs@free.fr   
#    version              : $Id$                                  
#
##############################################################################
#
#   This program is free software; you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation; either version 2 of the License, or
#   (at your option) any later version.
#
##############################################################################

ALL_MODEL_SUBDIRS = $(shell find * -maxdepth 0 -type d -print | grep -v CVS)
RETIRED_DEFAULT_CAR_MODELS = 155-DTM acura-nsx-sz mc-larenf1 p406 kc-% pw-%

DATASUBDIRS = $(filter-out $(RETIRED_DEFAULT_CAR_MODELS),$(ALL_MODEL_SUBDIRS))

PKGSUBDIRS	= $(DATASUBDIRS)

include ${MAKE_DEFAULT}


