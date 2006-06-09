# $Id$

all:
	(cd apron; make all install)
	(cd mlgmpidl; make all debug install)
	(cd mlapronidl; make all install)
	(cd num; make all install)
	(cd newpolka; make allg ml install)
	(cd itv; make allmpq ml install)

clean:
	(cd apron; make clean)
	(cd mlgmpidl; make clean)
	(cd mlapronidl; make clean)
	(cd num; make clean)
	(cd itv; make clean)
	(cd newpolka; make clean)

distclean:
	(cd apron; make distclean)
	(cd mlgmpidl; make distclean)
	(cd mlapronidl; make distclean)
	(cd num; make distclean)
	(cd itv; make distclean)
	(cd newpolka; make distclean)

doc:
	(cd apron; make ps)
	(cd mlgmpidl; make doc)
	(cd mlapronidl; make doc)
