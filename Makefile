# $Id$

all:
	(cd apron; make all install)
	(cd mlgmpidl; make all debug install)
	(cd mlapronidl; make all install)
	(cd num; make all install)
	(cd newpolka; make allg ml install)
	(cd itv; make allmpq ml install)
	(cd octagons; make allQg allFd install)

clean:
	(cd apron; make clean)
	(cd mlgmpidl; make clean)
	(cd mlapronidl; make clean)
	(cd num; make clean)
	(cd itv; make clean)
	(cd newpolka; make clean)
	(cd octagons; make clean)
	(cd examples; make clean)

distclean:
	(cd apron; make distclean)
	(cd mlgmpidl; make distclean)
	(cd mlapronidl; make distclean)
	(cd num; make distclean)
	(cd itv; make distclean)
	(cd newpolka; make distclean)
	(cd octagons; make distclean)
	(cd examples; make distclean)

doc:
	(hyperlatex index.tex)
	(cd apron; make ps)
	(cd mlgmpidl; make doc)
	(cd mlapronidl; make doc)



# make distribution, update to reflect current version

PKGNAME  = apron-0.9.3
PKGFILES = Makefile README AUTHORS COPYING Makefile.config.model
PKGDIRS  = apron mlapronidl num mlgmpidl octagons itv newpolka examples

dist:
	$(MAKE)
	$(MAKE) doc
	mkdir -p $(PKGNAME)
	$(MAKE) $(foreach pkg,$(PKGDIRS),pkg_$(pkg))
	cp $(PKGFILES) $(PKGNAME)
	tar vczf $(PKGNAME).tgz $(PKGNAME)
	rm -rf $(PKGNAME)

pkg_%:
	(cd $*; $(MAKE) dist)
	(cd $(PKGNAME); tar xzf ../$*.tgz)
	rm -rf $*.tgz
