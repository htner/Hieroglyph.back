#-------------------------------------------------------------------------
#
# Makefile--
#    Makefile for access/common
#
# IDENTIFICATION
#    src/backend/access/common/Makefile
#
#-------------------------------------------------------------------------

subdir = src/backend/access/common
top_builddir = ../../../..
include $(top_builddir)/src/Makefile.global
override CPPFLAGS := -I$(top_builddir) $(CPPFLAGS)

OBJS = bufmask.o heaptuple.o indextuple.o printsimple.o printtup.o \
	relation.o reloptions.o scankey.o session.o tupconvert.o tupdesc.o \
	write_result_object.o

OBJS += memtuple.o reloptions_gp.o

include $(top_srcdir)/src/backend/common.mk
