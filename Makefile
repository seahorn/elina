# $Id$

all:
	(cd apron; make clib install)
	(cd mlgmpidl; make all debug install)
	(cd mlapronidl; make all install)
	(cd num; make all install)
	(cd newpolka; make allg ml polkarung polkatopg install)
	(cd omega; make libomega.a)
