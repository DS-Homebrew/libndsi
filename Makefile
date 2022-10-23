ifeq ($(strip $(DEVKITPRO)),)
$(error "Please set DEVKITPRO in your environment. export DEVKITPRO=<path to>devkitPro)
endif

export TOPDIR	:=	$(CURDIR)

export LIBNDSI_MAJOR	:= 1
export LIBNDSI_MINOR	:= 8
export LIBNDSI_PATCH	:= 0


VERSION	:=	$(LIBNDSI_MAJOR).$(LIBNDSI_MINOR).$(LIBNDSI_PATCH)


.PHONY: release debug clean all docs

all: include/nds/libversion.h release debug

#-------------------------------------------------------------------------------
release: lib
#-------------------------------------------------------------------------------
	$(MAKE) -C arm9 BUILD=release || { exit 1;}
	$(MAKE) -C arm7 BUILD=release || { exit 1;}

#-------------------------------------------------------------------------------
debug: lib
#-------------------------------------------------------------------------------
	$(MAKE) -C arm9 BUILD=debug || { exit 1;}
	$(MAKE) -C arm7 BUILD=debug || { exit 1;}

#-------------------------------------------------------------------------------
lib:
#-------------------------------------------------------------------------------
	mkdir lib

#-------------------------------------------------------------------------------
clean:
#-------------------------------------------------------------------------------
	@$(MAKE) -C arm9 clean
	@$(MAKE) -C arm7 clean

#-------------------------------------------------------------------------------
dist-src:
#-------------------------------------------------------------------------------
	@tar --exclude=*CVS* --exclude=.svn -cjf libndsi-src-$(VERSION).tar.bz2 arm7/Makefile arm9/Makefile source include icon.bmp Makefile libndsi_license.txt Doxyfile

#-------------------------------------------------------------------------------
dist-bin: all
#-------------------------------------------------------------------------------
	@tar --exclude=*CVS* --exclude=.svn -cjf libndsi-$(VERSION).tar.bz2 include lib icon.bmp libndsi_license.txt

dist: dist-bin dist-src

#-------------------------------------------------------------------------------
install: dist-bin
#-------------------------------------------------------------------------------
	mkdir -p $(DESTDIR)$(DEVKITPRO)/libndsi
	bzip2 -cd libndsi-$(VERSION).tar.bz2 | tar -xf - -C $(DESTDIR)$(DEVKITPRO)/libndsi

#---------------------------------------------------------------------------------
docs:
#---------------------------------------------------------------------------------
	doxygen Doxyfile
	cat warn.log

#---------------------------------------------------------------------------------
include/nds/libversion.h : Makefile
#---------------------------------------------------------------------------------
	@echo "#ifndef __LIBNDSIVERSION_H__" > $@
	@echo "#define __LIBNDSIVERSION_H__" >> $@
	@echo >> $@
	@echo "#define _LIBNDSI_MAJOR_	$(LIBNDSI_MAJOR)" >> $@
	@echo "#define _LIBNDSI_MINOR_	$(LIBNDSI_MINOR)" >> $@
	@echo "#define _LIBNDSI_PATCH_	$(LIBNDSI_PATCH)" >> $@
	@echo >> $@
	@echo '#define _LIBNDSI_STRING "libNDSi Release '$(LIBNDSI_MAJOR).$(LIBNDSI_MINOR).$(LIBNDSI_PATCH)'"' >> $@
	@echo >> $@
	@echo "#endif // __LIBNDSIVERSION_H__" >> $@


