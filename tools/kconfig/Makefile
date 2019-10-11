# ===========================================================================
# Kernel configuration targets
# These targets are used from top-level makefile

# SRCDIR is kconfig source dir, allows for out-of-tree builds
# if building in tree, SRCDIR==build dir
SRCDIR := $(abspath $(dir $(firstword $(MAKEFILE_LIST))))

PHONY += xconfig gconfig menuconfig config silentoldconfig \
	localmodconfig localyesconfig clean

ifdef KBUILD_KCONFIG
Kconfig := $(KBUILD_KCONFIG)
else
Kconfig := Kconfig
endif

ifeq ($(quiet),silent_)
silent := -s
endif

# We need this, in case the user has it in its environment
unexport CONFIG_

# Unset some environment variables set in the project environment
CFLAGS :=
CPPFLAGS :=
LDFLAGS :=

# Workaround for a bug on Windows if the mingw32 host compilers
# are installed in addition to the MSYS ones. The kconfig tools
# need to be compiled using the MSYS compiler.
#
# See https://github.com/espressif/esp-idf/issues/1296
ifdef MSYSTEM
ifeq ("$(MSYSTEM)", "MINGW32")
ifeq ("$(CC)", "cc")
CC := /usr/bin/gcc
endif
ifeq ("$(LD)", "ld")
LD := /usr/bin/ld
endif
endif # MING32
endif # MSYSTEM

default: mconf-idf conf-idf

xconfig: qconf
	$< $(silent) $(Kconfig)

gconfig: gconf
	$< $(silent) $(Kconfig)

menuconfig: mconf-idf
	$< $(silent) $(Kconfig)

config: conf-idf
	$< $(silent) --oldaskconfig $(Kconfig)

nconfig: nconf
	$< $(silent) $(Kconfig)

silentoldconfig: conf-idf
	mkdir -p include/config include/generated
	$< $(silent) --$@ $(Kconfig)

localyesconfig localmodconfig: streamline_config.pl conf-idf
	mkdir -p include/config include/generated
	perl $< --$@ . $(Kconfig) > .tmp.config
	if [ -f .config ]; then 					\
			cmp -s .tmp.config .config ||			\
			(mv -f .config .config.old.1;			\
			 mv -f .tmp.config .config;			\
			 conf-idf $(silent) --silentoldconfig $(Kconfig); \
			 mv -f .config.old.1 .config.old)		\
	else								\
			mv -f .tmp.config .config;			\
			conf-idf $(silent) --silentoldconfig $(Kconfig); \
	fi
	rm -f .tmp.config


# These targets map 1:1 to the commandline options of 'conf-idf'
simple-targets := oldconfig allnoconfig allyesconfig allmodconfig \
	alldefconfig randconfig listnewconfig olddefconfig
PHONY += $(simple-targets)

$(simple-targets): conf-idf
	$< $(silent) --$@ $(Kconfig)

PHONY += oldnoconfig savedefconfig defconfig

# oldnoconfig is an alias of olddefconfig, because people already are dependent
# on its behavior (sets new symbols to their default value but not 'n') with the
# counter-intuitive name.
oldnoconfig: olddefconfig

savedefconfig: conf-idf
	$< $(silent) --$@=defconfig $(Kconfig)

defconfig: conf-idf
ifeq ($(KBUILD_DEFCONFIG),)
	$< $(silent) --defconfig $(Kconfig)
else
ifneq ($(wildcard $(srctree)/arch/$(SRCARCH)/configs/$(KBUILD_DEFCONFIG)),)
	@$(kecho) "*** Default configuration is based on '$(KBUILD_DEFCONFIG)'"
	$< $(silent) --defconfig=arch/$(SRCARCH)/configs/$(KBUILD_DEFCONFIG) $(Kconfig)
else
	@$(kecho) "*** Default configuration is based on target '$(KBUILD_DEFCONFIG)'"
	$(MAKE) -f $(srctree)/Makefile $(KBUILD_DEFCONFIG)
endif
endif

%_defconfig: conf-idf
	$< $(silent) --defconfig=arch/$(SRCARCH)/configs/$@ $(Kconfig)

configfiles=$(wildcard $(srctree)/kernel/configs/$@ $(srctree)/arch/$(SRCARCH)/configs/$@)

%.config: conf-idf
	$(if $(call configfiles),, $(error No configuration exists for this target on this architecture))
	$(CONFIG_SHELL) $(srctree)/scripts/kconfig/merge_config.sh -m .config $(configfiles)
	+yes "" | $(MAKE) -f $(srctree)/Makefile oldconfig

PHONY += kvmconfig
kvmconfig: kvm_guest.config
	@:

PHONY += xenconfig
xenconfig: xen.config
	@:

PHONY += tinyconfig
tinyconfig:
	$(MAKE) -f $(srctree)/Makefile allnoconfig tiny.config

# Help text used by make help
help:
	@echo  '  config	  - Update current config utilising a line-oriented program'
	@echo  '  nconfig         - Update current config utilising a ncurses menu based'
	@echo  '                    program'
	@echo  '  menuconfig	  - Update current config utilising a menu based program'
	@echo  '  xconfig	  - Update current config utilising a Qt based front-end'
	@echo  '  gconfig	  - Update current config utilising a GTK+ based front-end'
	@echo  '  oldconfig	  - Update current config utilising a provided .config as base'
	@echo  '  localmodconfig  - Update current config disabling modules not loaded'
	@echo  '  localyesconfig  - Update current config converting local mods to core'
	@echo  '  silentoldconfig - Same as oldconfig, but quietly, additionally update deps'
	@echo  '  defconfig	  - New config with default from ARCH supplied defconfig'
	@echo  '  savedefconfig   - Save current config as ./defconfig (minimal config)'
	@echo  '  allnoconfig	  - New config where all options are answered with no'
	@echo  '  allyesconfig	  - New config where all options are accepted with yes'
	@echo  '  allmodconfig	  - New config selecting modules when possible'
	@echo  '  alldefconfig    - New config with all symbols set to default'
	@echo  '  randconfig	  - New config with random answer to all options'
	@echo  '  listnewconfig   - List new options'
	@echo  '  olddefconfig	  - Same as silentoldconfig but sets new symbols to their'
	@echo  '                    default value'
	@echo  '  kvmconfig	  - Enable additional options for kvm guest kernel support'
	@echo  '  xenconfig       - Enable additional options for xen dom0 and guest kernel support'
	@echo  '  tinyconfig	  - Configure the tiniest possible kernel'

# lxdialog stuff
check-lxdialog  := $(SRCDIR)/lxdialog/check-lxdialog.sh

# Use recursively expanded variables so we do not call gcc unless
# we really need to do so. (Do not call gcc as part of make mrproper)
CFLAGS += $(shell $(CONFIG_SHELL) $(check-lxdialog) -ccflags) \
                    -DLOCALE -MMD -MP

CFLAGS += -I "." -I "${SRCDIR}"

%.o: $(SRCDIR)/%.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@

lxdialog/%.o: $(SRCDIR)/lxdialog/%.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@

%.o: %.c
	$(CC) -I $(SRCDIR) -c $(CFLAGS) $(CPPFLAGS) $< -o $@

# ===========================================================================
# Shared Makefile for the various kconfig executables:
# conf-idf:	  Used for defconfig, oldconfig and related targets
# nconf:  Used for the nconfig target.
#         Utilizes ncurses
# mconf-idf:  Used for the menuconfig target
#         Utilizes the lxdialog package
# qconf:  Used for the xconfig target
#         Based on Qt which needs to be installed to compile it
# gconf:  Used for the gconfig target
#         Based on GTK+ which needs to be installed to compile it
# object files used by all kconfig flavours

lxdialog := lxdialog/checklist.o lxdialog/util.o lxdialog/inputbox.o
lxdialog += lxdialog/textbox.o lxdialog/yesno.o lxdialog/menubox.o

conf-objs	:= conf.o  zconf.tab.o expand_env.o
mconf-objs     := mconf.o zconf.tab.o $(lxdialog) expand_env.o
nconf-objs     := nconf.o zconf.tab.o nconf.gui.o expand_env.o
kxgettext-objs	:= kxgettext.o zconf.tab.o expand_env.o
qconf-cxxobjs	:= qconf.o
qconf-objs	:= zconf.tab.o expand_env.o
gconf-objs	:= gconf.o zconf.tab.o expand_env.o

hostprogs-y := conf-idf nconf mconf-idf kxgettext qconf gconf

all-objs := $(conf-objs) $(mconf-objs) $(lxdialog)
all-deps := $(all-objs:.o=.d)

clean-files	:= qconf.moc .tmp_qtcheck .tmp_gtkcheck
clean-files	+= zconf.tab.c zconf.lex.c zconf.hash.c gconf.glade.h
clean-files += $(all-objs) $(all-deps) conf-idf mconf-idf conf mconf
# (note: cleans both mconf & conf (old names) and conf-idf & mconf-idf (new names))

# Check that we have the required ncurses stuff installed for lxdialog (menuconfig)
PHONY += dochecklxdialog
$(addprefix ,$(lxdialog)): dochecklxdialog
dochecklxdialog: lxdialog
	$(CONFIG_SHELL) $(check-lxdialog) -check $(CC) $(CFLAGS) $(LOADLIBES_mconf)

lxdialog:
	mkdir -p lxdialog

always := dochecklxdialog

# Add environment specific flags
EXTRACFLAGS += $(shell $(CONFIG_SHELL) $(srctree)/$(src)/check.sh $(CC) $(CFLAGS))

# generated files seem to need this to find local include files
CFLAGS_zconf.lex.o	:= -I$(src)
CFLAGS_zconf.tab.o	:= -I$(src)

LEX_PREFIX_zconf	:= zconf
YACC_PREFIX_zconf	:= zconf

LOADLIBES_qconf	= $(KC_QT_LIBS)
CXXFLAGS_qconf.o	= $(KC_QT_CFLAGS)

LOADLIBES_gconf	= `pkg-config --libs gtk+-2.0 gmodule-2.0 libglade-2.0`
CFLAGS_gconf.o	= `pkg-config --cflags gtk+-2.0 gmodule-2.0 libglade-2.0` \
                          -Wno-missing-prototypes

LOADLIBES_mconf   = $(shell $(CONFIG_SHELL) $(check-lxdialog) -ldflags $(CC))

LOADLIBES_conf = $(LOADLIBES_mconf)

LOADLIBES_nconf	= $(shell \
				pkg-config --libs menuw panelw ncursesw 2>/dev/null \
				|| pkg-config --libs menu panel ncurses 2>/dev/null \
				|| echo "-lmenu -lpanel -lncurses"  )

ifeq ("$(OS)","Windows_NT")
# Windows toolchains don't seem to count libintl
# as a system library
LOADLIBES_mconf += -lintl
LOADLIBES_nconf += -lintl
endif

qconf.o: .tmp_qtcheck

ifeq ($(MAKECMDGOALS),xconfig)
.tmp_qtcheck: $(src)/Makefile
-include .tmp_qtcheck

# Qt needs some extra effort...
.tmp_qtcheck:
	@set -e; $(kecho) "  CHECK   qt"; \
	if pkg-config --exists Qt5Core; then \
	    cflags="-std=c++11 -fPIC `pkg-config --cflags Qt5Core Qt5Gui Qt5Widgets`"; \
	    libs=`pkg-config --libs Qt5Core Qt5Gui Qt5Widgets`; \
	    moc=`pkg-config --variable=host_bins Qt5Core`/moc; \
	elif pkg-config --exists QtCore; then \
	    cflags=`pkg-config --cflags QtCore QtGui`; \
	    libs=`pkg-config --libs QtCore QtGui`; \
	    moc=`pkg-config --variable=moc_location QtCore`; \
	else \
	    echo >&2 "*"; \
	    echo >&2 "* Could not find Qt via pkg-config."; \
	    echo >&2 "* Please install either Qt 4.8 or 5.x. and make sure it's in PKG_CONFIG_PATH"; \
	    echo >&2 "*"; \
	    exit 1; \
	fi; \
	echo "KC_QT_CFLAGS=$$cflags" > $@; \
	echo "KC_QT_LIBS=$$libs" >> $@; \
	echo "KC_QT_MOC=$$moc" >> $@
endif

gconf.o: .tmp_gtkcheck

ifeq ($(MAKECMDGOALS),gconfig)
-include .tmp_gtkcheck

# GTK+ needs some extra effort, too...
.tmp_gtkcheck:
	@if `pkg-config --exists gtk+-2.0 gmodule-2.0 libglade-2.0`; then		\
		if `pkg-config --atleast-version=2.0.0 gtk+-2.0`; then			\
			touch $@;								\
		else									\
			echo >&2 "*"; 							\
			echo >&2 "* GTK+ is present but version >= 2.0.0 is required.";	\
			echo >&2 "*";							\
			false;								\
		fi									\
	else										\
		echo >&2 "*"; 								\
		echo >&2 "* Unable to find the GTK+ installation. Please make sure that"; 	\
		echo >&2 "* the GTK+ 2.0 development package is correctly installed..."; 	\
		echo >&2 "* You need gtk+-2.0, glib-2.0 and libglade-2.0."; 		\
		echo >&2 "*"; 								\
		false;									\
	fi
endif

zconf.tab.o: zconf.lex.c zconf.hash.c

qconf.o: qconf.moc

quiet_cmd_moc = MOC     $@
      cmd_moc = $(KC_QT_MOC) -i $< -o $@

%.moc: $(src)/%.h .tmp_qtcheck
	$(call cmd,moc)

# Extract gconf menu items for i18n support
gconf.glade.h: gconf.glade
	intltool-extract --type=gettext/glade --srcdir=$(srctree) \
	gconf.glade


mconf-idf: lxdialog $(mconf-objs)
	$(CC) -o $@ $(mconf-objs) $(LOADLIBES_mconf)

conf-idf: $(conf-objs)
	$(CC) -o $@ $(conf-objs) $(LOADLIBES_conf)

zconf.tab.c: zconf.lex.c

zconf.lex.c: $(SRCDIR)/zconf.l
	flex -L -Pzconf -ozconf.lex.c $<

zconf.hash.c: $(SRCDIR)/zconf.gperf
# strip CRs on Windows systems where gperf will otherwise barf on them
	sed -E "s/\\x0D$$//" $< | gperf -t --output-file zconf.hash.c -a -C -E -g -k '1,3,$$' -p -t

zconf.tab.c: $(SRCDIR)/zconf.y
	bison -t -l -p zconf -o zconf.tab.c $<

clean:
	rm -f $(clean-files)

-include $(all-deps)

