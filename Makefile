include Makefile.config

MLREQUESTED = mlgmpidl/Makefile.config


LCFLAGS = \
-Lapron -Litv -Lbox -Loctagons -Lnewpolka \
-L$(PPL_PREFIX)/lib -Lppl \
-Lproducts \
-L$(GMP_PREFIX)/lib -L$(MPFR_PREFIX)/lib \
-L$(CAMLIDL_PREFIX)/lib/ocaml

OCAMLINC = \
-I mlgmpidl -I mlapronidl -I box -I octagons -I newpolka -I ppl -I products

OCAMLLDFLAGS = \
-noautolink -ccopt "$(LCFLAGS)" \
bigarray.cma gmp.cma apron.cma box.cma oct.cma polka.cma \
-cclib "-lpolka_caml -lpolkaMPQ -loct_caml -loctMPQ -lbox_caml -lboxMPQ -lapron_caml -lapron -lgmp_caml -lmpfr -lgmp -lbigarray -lcamlidl"

OCAMLLDFLAGS_PPL = \
-noautolink -ccopt "$(LCFLAGS)" -cc "g++" \
bigarray.cma gmp.cma apron.cma box.cma oct.cma polka.cma \
polkaGrid.cma ppl.cma \
-cclib "-lpolkaGrid_caml -lap_pkgrid -lap_ppl_caml -lap_ppl -lppl -lgmpxx" \
-cclib "-lpolka_caml -lpolkaMPQ -loct_caml -loctMPQ -lbox_caml -lboxMPQ -lapron_caml -lapron -lgmp_caml -lmpfr -lgmp -lbigarray -lcamlidl"

ifneq ($(HAS_OCAML),)
ifneq ($(HAS_PPL),)
all: $(REQUESTED) c ml aprontop apronrun apronppltop apronpplrun
else
all: $(REQUESTED) c ml aprontop apronrun
endif
else
all: c
endif

mlgmpidl/Makefile.config: Makefile.config Makefile
	$(SED) -e '1 aHAS_MPFR=1\n' Makefile.config >$@

c:
	(cd num; make all)
	(cd itv; make all)
	(cd apron; make all)
	(cd newpolka; make all)
	(cd box; make all)
	(cd octagons; make MPQ D)
ifneq ($(HAS_PPL),)
	(cd ppl; make)
	(cd products; make)
endif
ifneq ($(HAS_CPP),)
	(cd apronxx; make)
endif

ml: $(MLREQUESTED)
	(cd mlgmpidl; make all)
	(cd mlapronidl; make all)
	(cd newpolka; make ml)
	(cd box; make ml)
	(cd octagons; make mlMPQ mlD)
ifneq ($(HAS_PPL),)
	(cd ppl; make ml)
	(cd products; make ml)
endif

.PHONY: apronrun aprontop apronpplrun apronppltop

apronrun: $(MLREQUESTED)
	$(OCAMLC) $(OCAMLINC) -verbose -make-runtime -o $@ $(OCAMLLDFLAGS)

aprontop: $(MLREQUESTED)
	$(OCAMLMKTOP) $(OCAMLINC) -verbose -custom -o $@ $(OCAMLLDFLAGS)

apronpplrun: $(MLREQUESTED)
	$(OCAMLC) $(OCAMLINC) -verbose -make-runtime -o $@ $(OCAMLLDFLAGS_PPL)

apronppltop: $(MLREQUESTED)
	$(OCAMLMKTOP) $(OCAMLINC) -verbose -custom -o $@ $(OCAMLLDFLAGS_PPL)

rebuild: $(MLREQUESTED)
ifneq ($(HAS_OCAML),)
	(cd mlgmpidl; make rebuild)
	(cd mlapronidl; make rebuild)
	(cd newpolka; make rebuild)
	(cd box; make rebuild)
	(cd octagons; make rebuild)
	(cd ppl; make rebuild)
	(cd products; make rebuild)
endif

install:
	(cd num; make install)
	(cd itv; make install)
	(cd apron; make install)
	(cd newpolka; make install)
	(cd box; make install)
	(cd octagons; make install)
ifneq ($(HAS_PPL),)
	(cd ppl; make install)
	(cd products; make install)
endif
ifneq ($(HAS_OCAML),)
	(cd mlgmpidl; make install)
	(cd mlapronidl; make install)
	$(INSTALLd) $(APRON_PREFIX)/bin
	$(INSTALL) apronrun $(APRON_PREFIX)/bin
	$(INSTALL) aprontop $(APRON_PREFIX)/bin
ifneq ($(HAS_PPL),)
	$(INSTALL) apronpplrun $(APRON_PREFIX)/bin
	$(INSTALL) apronppltop $(APRON_PREFIX)/bin
endif
endif
ifneq ($(HAS_CPP),)
	(cd apronxx; make install)
endif

clean:
	(cd num; make clean)
	(cd itv; make clean)
	(cd apron; make clean)
	(cd mlgmpidl; make clean)
	(cd mlapronidl; make clean)
	(cd box; make clean)
	(cd newpolka; make clean)
	(cd octagons; make clean)
	(cd ppl; make clean)
	(cd products; make clean)
	(cd apronxx; make clean)
	(cd examples; make clean)
	(cd test; make clean)
	rm -fr online tmp apronrun aprontop apronpplrun apronppltop

mostlyclean: clean
	(cd mlgmpidl; make mostlyclean)
	(cd mlapronidl; make mostlyclean)
	(cd box; make mostlyclean)
	(cd octagons; make mostlyclean)
	(cd newpolka; make mostlyclean)
	(cd ppl; make mostlyclean)
	(cd products; make mostlyclean)
	(cd apronxx; make mostlyclean)

distclean:
	(cd num; make distclean)
	(cd itv; make distclean)
	(cd apron; make distclean)
	(cd mlgmpidl; make distclean)
	(cd mlapronidl; make distclean)
	(cd box; make distclean)
	(cd newpolka; make distclean)
	(cd octagons; make distclean)
	(cd examples; make distclean)
	(cd ppl; make distclean)
	(cd products; make distclean)
	(cd apronxx; make distclean)
	(cd $(APRON_PREFIX)/bin; rm -f apronrun aprontop apronpplrun apronppltop)

doc:
	(cd apron; make html apron.pdf)
ifneq ($(HAS_OCAML),)
	(cd mlgmpidl; make html mlgmpidl.pdf)
	(cd mlapronidl; make html mlapronidl.pdf)
endif
ifneq ($(HAS_CPP),)
	(cd apronxx; make doc)
endif

# make distribution, update to reflect current version

PKGNAME  = apron-0.9.8
PKGFILES = Makefile README README.windows AUTHORS COPYING Makefile.config.model Changes
PKGDIRS  = apron num itv octagons box newpolka ppl products mlgmpidl mlapronidl examples test apronxx

dist:
	$(MAKE) all
	$(MAKE) doc
	mkdir -p $(PKGNAME)
	$(MAKE) $(foreach pkg,$(PKGDIRS),pkg_$(pkg))
	cp $(PKGFILES) $(PKGNAME)
	tar vczf $(PKGNAME).tgz $(PKGNAME)
	rm -rf $(PKGNAME)

# these 2 targets are for main developpers only
index.html: index.tex
	hyperlatex index.tex
online: doc index.html
	rm -fr online
	mkdir -p online
	mv index.html poster.gif flyer.pdf online
	mv apron/html online/apron
	mv mlapronidl/html online/mlapronidl
	mv mlgmpidl/html online/mlgmpidl
	cp apron/apron.pdf online
	cp mlapronidl/mlapronidl.pdf online
	cp mlgmpidl/mlgmpidl.pdf online
	cp examples/example1.c online
	cp examples/mlexample?.ml online
	cp Changes online

pkg_mlgmpidl:
	(cd mlgmpidl; $(MAKE) dist)
	(cd $(PKGNAME); tar xzf $(HOME)/mlgmpidl.tgz)
	rm -rf $*.tgz
pkg_%:
	(cd $*; $(MAKE) dist)
	(cd $(PKGNAME); tar xzf ../$*.tgz)
	rm -rf $*.tgz
