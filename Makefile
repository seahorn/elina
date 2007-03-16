
include Makefile.config

LCFLAGS = \
-L$(GMP_PREFIX)/lib \
-L$(MPFR_PREFIX)/lib \
-Lapron -Litv -Lbox -Loctagons -Lnewpolka \
-L$(PPL_PREFIX)/lib -Lppl \
-L$(CAMLIDL_PREFIX)/lib/ocaml

OCAMLINC = \
-I mlgmpidl -I mlapronidl -I box -I octagons -I newpolka -I ppl

ifdef HAS_PPL
OCAMLLDFLAGS_PPL = ppl.cma -cclib "-lppl_caml -lapron_ppl -lppl -lgmpxx" -cc "g++"
endif

OCAMLLDFLAGS = \
-noautolink -ccopt "$(LCFLAGS)" \
bigarray.cma gmp.cma apron.cma box.cma oct.cma polka.cma \
$(OCAMLLDFLAGS_PPL) \
-cclib "-lpolka_caml -lpolkag_debug -loct_caml -loctQg -lbox_caml -lboxmpq_debug -litvmpq_debug -lapron_caml_debug -lapron_debug -lgmp_caml -lmpfr -lgmp -lbigarray -lcamlidl" 

ifdef HAS_OCAML
all: c ml aprontop apronrun 
else
all: c
endif

c:
	(cd apron; make all)
	(cd num; make all)
	(cd itv; make all)
	(cd newpolka; make allg)
	(cd box; make all)
	(cd octagons; make Qg Fd)
ifdef HAS_PPL
	(cd ppl; make)
endif

ml:
	(cd mlgmpidl; make all)
	(cd mlapronidl; make all)
	(cd newpolka; make ml)
	(cd box; make ml)
	(cd octagons; make mlQg mlFd)
ifdef HAS_PPL
	(cd ppl; make ml)
endif

.PHONY: apronrun aprontop

apronrun:
	$(OCAMLC) $(OCAMLINC) -verbose -make-runtime -o $@ $(OCAMLLDFLAGS)

aprontop:
	$(OCAMLMKTOP) $(OCAMLINC) -verbose -custom -o $@ $(OCAMLLDFLAGS)

rebuild:
ifdef HAS_OCAML
	(cd mlgmpidl; make rebuild)
	(cd mlapronidl; make rebuild)
	(cd newpolka; make rebuild)
	(cd box; make rebuild)
	(cd octagons; make rebuild)
	(cd ppl; make rebuild)
endif

install:
	(cd apron; make install)
	(cd num; make install)
	(cd itv; make install)
	(cd newpolka; make install)
	(cd box; make install)
	(cd octagons; make install)
ifdef HAS_PPL
	(cd ppl; make install)
endif

ifdef HAS_OCAML
	(cd mlgmpidl; make install)
	(cd mlapronidl; make install)
	$(INSTALLd) $(APRON_PREFIX)/bin
	$(INSTALL) apronrun $(APRON_PREFIX)/bin
	$(INSTALL) aprontop $(APRON_PREFIX)/bin
endif

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
	(cd ppl; make clean)
	rm -fr online tmp apronrun aprontop

mostlyclean: clean
	(cd mlgmpidl; make mostlyclean)
	(cd mlapronidl; make mostlyclean)
	(cd box; make mostlyclean)
	(cd octagons; make mostlyclean)
	(cd newpolka; make mostlyclean)
	(cd ppl; make mostlyclean)

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
	(cd ppl; make distclean)

doc:
	(hyperlatex index.tex)
	(cd apron; make html apron.pdf)
ifdef HAS_OCAML
	(cd mlgmpidl; make html mlgmpidl.pdf)
	(cd mlapronidl; make html mlapronidl.pdf)
endif

# make distribution, update to reflect current version

PKGNAME  = apron-0.9.5
PKGFILES = Makefile README AUTHORS COPYING Makefile.config.model Changes
PKGDIRS  = apron num itv octagons box newpolka mlgmpidl mlapronidl examples

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
	cp Changes online

pkg_%:
	(cd $*; $(MAKE) dist)
	(cd $(PKGNAME); tar xzf ../$*.tgz)
	rm -rf $*.tgz
