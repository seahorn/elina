# $Id$


all: c ml

c:
	(cd apron; make all)
	(cd num; make all)
	(cd itv; make all)
	(cd newpolka; make allg)
	(cd box; make all)
	(cd octagons; make allQg allFd)

ml:
	(cd mlgmpidl; make all)
	(cd mlapronidl; make all)
	(cd newpolka; make ml)
	(cd box; make ml)
	(cd octagons; make mlQg mlFd)

install:
	(cd apron; make install)
	(cd mlgmpidl; make install)
	(cd mlapronidl; make install)
	(cd num; make install)
	(cd itv; make install)
	(cd newpolka; make install)
	(cd box; make install)
	(cd octagons; make install)

clean:
	(cd apron; make clean)
	(cd mlgmpidl; make clean)
	(cd mlapronidl; make clean)
	(cd num; make clean)
	(cd itv; make clean)
	(cd box; make clean)
	(cd newpolka; make clean)
	(cd octagons; make clean)
	(cd examples; make clean)
	rm -fr online

mostlyclean: clean
	(cd apron; make mostlyclean)
	(cd mlgmpidl; make mostlyclean)
	(cd mlapronidl; make mostlyclean)
	(cd itv; make mostlyclean)
	(cd box; make mostlyclean)
	(cd newpolka; make mostlyclean)


distclean:
	(cd apron; make distclean)
	(cd mlgmpidl; make distclean)
	(cd mlapronidl; make distclean)
	(cd num; make distclean)
	(cd itv; make distclean)
	(cd box; make distclean)
	(cd newpolka; make distclean)
	(cd octagons; make distclean)
	(cd examples; make distclean)

doc: 
	(hyperlatex index.tex)
	(cd apron; make html apron.pdf)
	(cd mlgmpidl; make html mlgmpidl.pdf)
	(cd mlapronidl; make html mlapronidl.pdf)

# make distribution, update to reflect current version

PKGNAME  = apron-0.9.4
PKGFILES = Makefile README AUTHORS COPYING Makefile.config.model Changes
PKGDIRS  = apron mlapronidl num itv mlgmpidl octagons box newpolka examples

dist:
	$(MAKE) all
	$(MAKE) doc
	mkdir -p $(PKGNAME)
	$(MAKE) $(foreach pkg,$(PKGDIRS),pkg_$(pkg))
	cp $(PKGFILES) $(PKGNAME)
	tar vczf $(PKGNAME).tgz $(PKGNAME)
	rm -rf $(PKGNAME)

online: doc
	rm -fr online
	mkdir -p online
	mv index.html online
	mv apron/html online/apron
	mv mlapronidl/html online/mlapronidl
	mv mlgmpidl/html online/mlgmpidl
	cp apron/apron.pdf online
	cp mlapronidl/mlapronidl.pdf online
	cp mlgmpidl/mlgmpidl.pdf online
	cp examples/example1.c online
	cp examples/mlexample?.ml online

pkg_%:
	(cd $*; $(MAKE) dist)
	(cd $(PKGNAME); tar xzf ../$*.tgz)
	rm -rf $*.tgz
