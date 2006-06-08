# $Id$

all:
	(cd apron; make all install)
	(cd mlgmpidl; make all debug install)
	(cd mlapronidl; make all install)
	(cd num; make all install)
	(cd newpolka; make allg ml polkarung polkatopg install)
	(cd omega; make libomega.a)

clean:
	(cd apron; make clean)
	(cd mlgmpidl; make clean)
	(cd mlapronidl; make clean)
	(cd num; make clean)
	(cd newpolka; make clean)
	(cd omega; make clean)

distclean:
	(cd apron; make distclean)
	(cd mlgmpidl; make distclean)
	(cd mlapronidl; make distclean)
	(cd num; make distclean)
	(cd newpolka; make distclean)
	(cd omega; make veryclean)

doc:
	(cd apron; make ps)
	(cd mlgmpidl; make doc)
	(cd mlapronidl; make doc)
