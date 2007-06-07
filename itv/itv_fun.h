/* ********************************************************************** */
/* itv_fun.h: defines ITV_FUN */
/* ********************************************************************** */

#undef ITVFUN3
#undef ITVFUN2
#undef ITVFUN
#undef NUM_SUFFIX

#if defined(NUM_LONGINT)
#define NUM_SUFFIX li
#elif defined(NUM_LONGLONGINT)
#define NUM_SUFFIX lli
#elif defined(NUM_MPZ)
#define NUM_SUFFIX mpz
#elif defined(NUM_LONGRAT)
#define NUM_SUFFIX lr
#elif defined(NUM_LONGLONGRAT)
#define NUM_SUFFIX llr
#elif defined(NUM_MPQ)
#define NUM_SUFFIX mpq
#elif defined(NUM_DOUBLE)
#define NUM_SUFFIX dbl
#elif defined(NUM_LONGDOUBLE)
#define NUM_SUFFIX ldbl
#else
#error "HERE"
#endif

/* ITVFUN(itv_add) expands to itv_add_mpq, or itv_add_dbl, ... */
#define ITVFUN3(NAME,SUFFIX) NAME ## _ ## SUFFIX
#define ITVFUN2(x,y) ITVFUN3(x,y)
#define ITVFUN(x) ITVFUN2(x,NUM_SUFFIX)
