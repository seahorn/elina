/*
 * apronxx_test.cc
 *
 * APRON Library / Test-suite for C++ class wrappers
 *
 * Copyright (C) Antoine Mine' 2007
 *
 */

/* This file is part of the APRON Library, released under LGPL license.  
   Please read the COPYING file packaged in the distribution.
*/

#include <iostream>
#include <stdio.h>
#include "apronxx.hh"

using namespace std;
using namespace apron;


void test_scalar()
{
  cout << endl << "scalars" << endl;

  // constructors
  cout << "construct 2:              " << scalar(2) << endl;
  cout << "construct 2L:             " << scalar(2L) << endl;
  cout << "construct 2.:             " << scalar(2.) << endl;
  cout << "construct frac(1,5):      " << scalar(frac(1,5)) << endl;
  cout << "construct infty(-1):      " << scalar(infty(-1)) << endl;
  cout << "construct mpq_class(1,5): " << scalar(mpq_class(1,5)) << endl;

  // assignments 
  scalar s = 12;
  scalar cp = s;  cout << "copy   12:        " << cp << endl;
  cp = 2;         cout << "assign 2:         " << cp << endl;
  cp = 2.;        cout << "assign 2.:        " << cp << endl;
  cp = frac(1,5); cout << "assign frac(1,5): " << cp << endl;
  cp = s;         cout << "copy   12:        " << s  << endl;
  cp = 5;         cout << "before swap 12 5: " << s << " " << cp << endl;
  swap(cp,s);     cout << "after swap  5 12: " << s << " " << cp << endl;
  cp = s = 2;     cout << "chain assign 2:   " << s << " " << cp << endl;
  cp = s = 2.;    cout << "chain assign 2.:  " << s << " " << cp << endl;

  // tests
  cp = s = 12;
  assert(cp==s && cp>=s && cp<=s && !(cp<s) && !(cp>s) && !(cp!=s));
  assert(cp==12 && cp>=11 && cp<=13 && !(4>cp) && !(cp>49));

  // access
  scalar::order o;
  cp = 2;
  assert(cp.get_discr()==AP_SCALAR_MPQ);
  assert(cp.get_mpq()==2);
  try { cp.get_double(); assert(0); } catch (bad_discriminant& b) {};
  assert(cp.to_mpq(GMP_RNDN,o)==2);
  assert(cp.to_double(GMP_RNDN,o)==2);
  //assert((mpq_class)cp==mpq_class(2));
  assert(double(cp)==2);
  cp = 2.;
  assert(cp.get_discr()==AP_SCALAR_DOUBLE);
  assert(cp.get_double()==2);
  try { cp.get_mpq(); assert(0); } catch (bad_discriminant& b) {};
  assert(cp.to_mpq(GMP_RNDN,o)==2);
  assert(cp.to_double(GMP_RNDN,o)==2);
  //assert(mpq_class(cp)==mpq_class(2));
  assert(double(cp)==2);
  cp.get_double() = 15;
  cout << "get_double 15.: " << cp << endl;

  // new
  scalar*ps = new scalar(12);
  scalar*pa = new scalar[2];
  pa[1] = *ps;
  cout << "array 12 0 12: " << *ps << " " << pa[0] << " " << pa[1] << endl;
  delete ps;
  delete[] pa;  

  // operations
  scalar a = 12;
  scalar b = -a;
  a.neg();
  cout << "neg -12 -12: " << a << " " << b << endl;
  b = ~a;
  a.inv();
  cout << "inv -1/12 -1/12: " << a << " " << b << endl;  
  printf("print: "); a.print(); printf("\n");
  cout << "hash: " << a.hash() << endl;
}

void test_interval()
{
  cout << endl << "intervals" << endl;

  // constructors
  cout << "construct 0:                     " << interval() << endl;
  cout << "construct scalar(12):            " << interval(scalar(12), scalar(15)) << endl;
  cout << "construct [12,15]:               " << interval(12,15) << endl;
  cout << "construct [12L,15L]:             " << interval(12L,15L) << endl;
  cout << "construct [5.,18.]:              " << interval(5.,18.) << endl;
  cout << "construct [frac(1,2),frac(5,4)]: " << interval(frac(1,2),frac(5,4)) << endl;
  cout << "construct [mpq(15,3),15]:        " << interval(mpq_class(15,3),15) << endl;
  cout << "construct top:                   " << interval(top()) << endl;
  cout << "construct bottom:                " << interval(bottom()) << endl;
  
  // assignments 
  interval i = interval(-5,5);
  interval c = i;                    cout << "copy [-5,5]:                " << c << endl;
  c = top();                         cout << "assign top:                 " << c << endl;
  c = bottom();                      cout << "assign bottom:              " << c << endl;
  c = i;                             cout << "copy [-5,5]:                " << c << endl;
  c.set(scalar(5),scalar(19));       cout << "set [scalar(5),scalar(19)]: " << c << endl;
  c.set(12,18);                      cout << "set [12,18]:                " << c << endl;
  c.set(12.,18.);                    cout << "set [12.,18.]:              " << c << endl;
  c.set(frac(1,2),frac(3,4));        cout << "set [frac(1,2),frac(3,4)]:  " << c << endl;
  c.set(mpq_class(8),mpq_class(10)); cout <<"set [mpq(8),mpq(10)]:        " <<  c << endl;
  c.set(top());                      cout << "set top:                    " << c << endl;
  c.set(bottom());                   cout << "set bottom:                 " << c << endl;
  c.set(i);                          cout << "set interval(-5,5):         " << c << endl;
  i = top();  swap(i,c);             cout << "after swap top [-5,5]:      " << c << " " << i << endl;
  c = i.set(1,2);                    cout << "chain assign [1,2]:         " << c << " " << i << endl;
  c = i = top();                     cout << "chain assign top:           " << c << " " << i << endl;

  // access
  c = interval(1,2);
  c.get_inf() = -1;        cout << "get_inf: [-1,2]:  " << c << endl;
  c.get_sup() = frac(1,2); cout << "get_sup [-1,1/2]: " << c << endl;

  // tests
  assert(interval(top()).is_top());
  assert(!interval(bottom()).is_top());
  assert(interval(scalar(infty(-1)),scalar(infty(1))).is_top());
  assert(!interval(1,2).is_top());
  assert(interval(bottom()).is_bottom());
  assert(!interval(top()).is_bottom());
  assert(interval(5,-10).is_bottom());
  assert(interval(1,2)<=interval(1,3));
  assert(interval(1,4)>interval(2,3));
  assert(interval(1,2)==interval(1,2));
  assert(interval(1,2)==interval(1.,2.));
  assert(interval(scalar(infty(-1)),scalar(infty(1)))==interval(top()));
  assert(interval(5,-10)==interval(bottom()));
  assert(cmp(interval(1,2),interval(1,3))==interval::INCLUDED);
  assert(cmp(interval(1,3),interval(1,3))==interval::EQUAL);
  assert(cmp(interval(0,3),interval(1,3))==interval::CONTAINS);
  assert(cmp(interval(1,3),interval(2,4))==interval::LESS);
  assert(cmp(interval(5,6),interval(1,2))==interval::GREATER);
 
  // new
  interval*pi = new interval(12,13);
  interval*pa = new interval[2];
  pa[1] = *pi;
  cout << "array [12,13] 0 [12,13]: " << *pi << " " << pa[0] << " " << pa[1] << endl;
  delete pi;
  delete[] pa;  

  // operations
  interval a = interval(1,2);
  interval b = -a;
  a.neg();
  cout << "neg [-2,-1] [-2,-1]: " << a << " " << b << endl;
  printf("print: "); a.print(); printf("\n");
  cout << "hash: " << a.hash() << endl;
}

void test_interval_array()
{
  cout << endl << "interval arrays" << endl;
  interval_array a = interval_array(4);
   a[1] = interval(1,2);
  a.get(2) = interval(1.,2.);
  *(a.get_contents())[3] = interval(top());
  try { a.get(4); assert(0); } catch (out_of_range& x) {};
  interval_array b = a;
  interval_array c = interval_array(3);
  cout << "construct a:    " << a.get_size() << " " << a << endl;
  cout << "copy a:         " << b.get_size() << " " << b << endl;
  cout << "empty:          " << c.get_size() << " " << c << endl;
  c = a;
  cout << "copy a:         " << c.get_size() << " " << c << endl;
  vector<interval> v = vector<interval>(3);
  v[0] = interval(1,2);
  interval_array d = v;
  c = v;
  cout << "from vector v:  " << d.get_size() << " " << d << endl;
  cout << "from vector v:  " << c.get_size() << " " << c << endl;
  c = b = a;
  cout << "chain assign a: " << c.get_size() << " " << c << endl;
  vector<interval> vv = d;
  interval_array dd = vv;
  cout << "to vector v:    " << dd.get_size() << " " << dd << endl;
  printf("print v: "); dd.print(); printf("\n");  
}

void test_coeff()
{
  cout << endl << "coeff" << endl;

  // constructors
  cout << "construct 0:                     " << coeff() << endl;
  cout << "construct 2:                     " << coeff(coeff(2)) << endl;
  cout << "construct scalar(frac(2,3)):     " << coeff(scalar(frac(2,3))) << endl;
  cout << "construct 2:                     " << coeff(2) << endl;
  cout << "construct 2L:                    " << coeff(2L) << endl;
  cout << "construct 2.5:                   " << coeff(2.5) << endl;
  cout << "construct frac(2,3):             " << coeff(frac(2,3)) << endl;
  cout << "construct mpq(2,3):              " << coeff(mpq_class(2,3)) << endl;
  cout << "construct interval(1,2):         " << coeff(interval(1,2)) << endl;
  cout << "construct [scalar(1),scalar(2)]: " << coeff(scalar(1),scalar(2)) << endl;
  cout << "construct [1,2]:                 " << coeff(1,2) << endl;
  cout << "construct [1L,2L]:               " << coeff(1L,2L) << endl;
  cout << "construct [1.1,2.2]:             " << coeff(1.1,2.2) << endl;
  cout << "construct [frac(1,2),frac(3,4)]: " << coeff(frac(1,2),frac(3,4)) << endl;
  cout << "construct [mpq(1,2),mpq(3,4)]:   " << coeff(mpq_class(1,2),mpq_class(3,4)) << endl;
  cout << "construct top:                   " << coeff(top()) << endl;
  cout << "construct bottom:                " << coeff(bottom()) << endl;

  // assignments
  coeff a;
  a = coeff(2);                         cout << "assign coeff(2):           " << a << endl;
  a = scalar(2);                        cout << "assign scalar(2):          " << a << endl;
  a = 2;                                cout << "assign 2:                  " << a << endl;
  a = 2L;                               cout << "assign 2L:                 " << a << endl;
  a = 2.5;                              cout << "assign 2.5:                " << a << endl;
  a = frac(1,2);                        cout << "assign frac(1,2):          " << a << endl;
  a = mpq_class(1,2);                   cout << "assign mpq(1,2):           " << a << endl;
  a = interval(1,2);                    cout << "assign interval(1,2):      " << a << endl;
  a = top();                            cout << "assign top:                " << a << endl;
  a = bottom();                         cout << "assign bottom:             " << a << endl;
  a.set(coeff(2));                      cout << "set coeff(2):              " << a << endl;
  a.set(scalar(2));                     cout << "set scalar(2):             " << a << endl;
  a.set(2);                             cout << "set 2:                     " << a << endl;
  a.set(2L);                            cout << "set 2L:                    " << a << endl;
  a.set(2.);                            cout << "set 2.:                    " << a << endl;
  a.set(frac(1,2));                     cout << "set frac(1,2):             " << a << endl;
  a.set(mpq_class(1,2));                cout << "set mpq(1,2):              " << a << endl;
  a.set(interval(1,2));                 cout << "set interval(1,2):         " << a << endl;
  a.set(scalar(1),scalar(2));           cout << "set [scalar(1),scalar(2)]: " << a << endl;
  a.set(1,2);                           cout << "set [1,2]:                 " << a << endl;
  a.set(1L,2L);                         cout << "set [1L,2L]:               " << a << endl;
  a.set(1.1,2.2);                       cout << "set [1.1,2.2]:             " << a << endl;
  a.set(frac(1,2),frac(3,4));           cout << "set [frac(1,2),frac(3,4)]: " << a << endl;
  a.set(mpq_class(1,2),mpq_class(3,4)); cout << "set [mpq(1,2),mpq(3,4)]:   " << a << endl;
  a.set(top());                         cout << "set top:                   " << a << endl;
  a.set(bottom());                      cout << "set bottom:                " << a << endl;
  a = 2;
  coeff b = 3;
  cout << "before swap 2 3: " << a << " " << b << endl;
  swap(a,b);
  cout << "after swap  3 2: " << a << " " << b << endl;
  a = b = frac(1,2);  cout << "chain assign frac(1,2):      " << a << " " << b << endl;
  a = b.set(1.,2.);   cout << "chain assign [1.,2.]:        " << a << " " << b << endl;

  // access
  coeff x = scalar(2);
  assert(x.get_discr()==AP_COEFF_SCALAR);
  x.get_scalar() = 3; cout << "get_scalar 3: " << x << endl;
  try { x.get_interval(); assert(0); } catch (bad_discriminant b) {}
  x = interval(1,2);
  assert(x.get_discr()==AP_COEFF_INTERVAL);
  x.get_interval().get_inf() = 0; cout << "get_interval [0,2]: " << x << endl;
  try { x.get_scalar(); assert(0); } catch (bad_discriminant b) {}

  // tests
  assert(coeff(0).is_zero()); assert(!(coeff(1).is_zero()));
  assert(coeff(0.).is_zero()); assert(!(coeff(1.).is_zero()));
  assert(coeff(0,0).is_zero()); assert(!(coeff(0,1).is_zero()));
  assert(coeff(0.,0.).is_zero()); assert(!(coeff(0.,1.).is_zero()));
#if 0
  assert(coeff(0)<=coeff(0,1)); assert(!(coeff(0)<=coeff(1,2)));
  assert(coeff(0,1)>=coeff(1)); assert(!(coeff(0,1)>=coeff(2)));
  assert(coeff(0)<coeff(0,1)); assert(!(coeff(0)<coeff(0)));
  assert(coeff(0)>coeff(bottom())); assert(!(coeff(0)>coeff(top())));
#endif
  assert(coeff(0,1)==coeff(interval(0.,1.)));
  assert(!(coeff(0)==coeff(0,1)));
  assert(coeff(10,-10)==coeff(bottom()));
  assert(coeff(0)!=coeff(0,1));

  // operations
  coeff cc = interval(1,1);
  cout << "before reduce [1,1]: " << cc << endl;
  cc.reduce();
  cout << "after reduce 1:      " << cc << endl;
  cc = interval(3,5);
  coeff dd = -cc;
  cc.neg();
  cout << "neg [-5,-3] [-5,-3]: " << cc << " " << dd << endl;
  cout << "hash: " << cc.hash() << endl;
  printf("print: "); cc.print(); printf("\n");
}

void test_dimchange()
{
  cout << endl << "dimchange" << endl;
  dimchange d = dimchange(1,2,(ap_dim_t[]){1,2,3});
  cout << "from array 1 2 3: " << d;
  d = (ap_dim_t[]){3,2,1};
  cout << "from array 3 2 1: " << d;
  cout << d.get_intdim() << " " << d.get_realdim() << " " << d.get(0) << " " << d[1] << endl;
  try { d.get(3); assert(0); } catch (out_of_range& b) {}
  dimchange e = d;
  d[0] = 12;
  cout << "copy 3 2 1: " << e;
  e = d;
  cout << "copy 12 2 1: " << e;
  vector<ap_dim_t> x = vector<ap_dim_t>(3,0);
  x[0] = 4; x[1] = 5; x[2] = 6;
  dimchange f = dimchange(1,2,x);
  cout << "from vector 4 5 6: " << f;
  d = x;
  cout << "from vector 4 5 6: " << d;
  dimchange g = dimchange(1,3);
  try { g = x; assert(0); } catch (invalid_argument& a) {}
  try { dimchange gg = dimchange(1,3,x); assert(0); } catch (invalid_argument& a) {}
  dimchange h = dimchange(d,true);
  dimchange i = -d;
  d.add_invert();
  cout << "invert: " << d;
  cout << "invert: " << h;
  cout << "invert: " << i;
  printf("print: "); d.print();
}

void test_dimperm()
{
  cout << endl << "dimperm" << endl;
  dimperm i = id(3);
  cout << "identity:         " << i << endl;
  dimperm d = dimperm(3,(ap_dim_t[]){1,0,2});
  cout << "from array 1 0 2: " << d;
  d = id(3);
  cout << "identity:         " << d;  
  d = (ap_dim_t[]){1,2,0};
  cout << "from array 1 2 0: " << d;
  cout << d.get_size() << " " << d.get(0) << " " << d[1] << endl;
  try { d.get(3); assert(0); } catch (out_of_range& b) {}
  dimperm e = d;
  d[0] = 12;
  cout << "copy 1 2 0: " << e;
  e = d;
  cout << "copy 12 2 0: " << e;
  vector<ap_dim_t> x = vector<ap_dim_t>(3,0);
  x[0] = 2; x[1] = 0; x[2] = 1;
  dimperm f = x;
  cout << "from vector 2 0 1: " << f;
  d = x;
  cout << "from vector 2 0 1: " << d;
  dimperm g = dimperm(4);
  g = x;
  cout << "from vector 2 0 1: " << d;
  dimperm h = dimperm(g,true);
  g = -d;
  d.invert();
  cout << "invert 1 2 0:     " << d;
  cout << "invert 1 2 0:     " << g;
  cout << "invert 1 2 0:     " << h;
  g = g*g;
  cout << "compose 2 0 1:     " << g;
  d *= d;
  cout << "compose 2 0 1:     " << d;
  dimperm j = dimperm(d,d);
  cout << "invert 1 2 0:      " << j;
  printf("print: "); j.print();
}


void test_linexpr0()
{
  dimchange dim = dimchange(1,1,(ap_dim_t[]){0,2});
  dimperm perm  = dimperm(3,(ap_dim_t[]){1,0,2});
  dimperm perm2 = -perm;

  // dense
  cout << endl << "dense linexpr0" << endl;
  linexpr0 l = linexpr0(AP_LINEXPR_DENSE,5);
  assert(l.get_discr()==AP_LINEXPR_DENSE);
  cout << "construct:                                   " << l.get_size() << ", " << l << endl;
  l.get_coeff(0) = frac(1,4);
  l.get_coeff(3) = -2.;
  assert(l.is_linear() && l.is_quasilinear());
  l.get_cst() = interval(1,2);
  assert(!l.is_linear() && l.is_quasilinear());
  cout << "get_ 1/4x0 - 2x3 + [1,2]:                    " << l.get_size() << ", " << l << endl;
  linexpr0 m = l;
  assert(equal(m,l));
  assert(m.get_discr()==AP_LINEXPR_DENSE);
  m.get_coeff(2) = top();
  assert(!equal(m,l));
  cout << "copy 1/4x0 + [-oo,+oo]x2 - 2x3 + [1,2]:      " << m.get_size() << ", " << m << endl;
  cout << "copy 1/4x0 - 2x3 + [1,2]:                    " << l.get_size() << ", " << l << endl;
  cout << "get_coeff 0: " << l.get_coeff(1) << endl;
  coeff a[] = { 1,2,interval(0,1) };
  linexpr0 n = linexpr0(3,a,2,AP_LINEXPR_DENSE);
  cout << "from array 1x0 + 2x1 + [0,1]x2 + 2:          " << n.get_size() << ", " << n << endl;  
  m = linexpr0();
  n = m = l;
  cout << "chained copy 1/4x0 - 2x3 + [1,2]:            " << n.get_size() << ", " << n << endl;  
  assert(equal(m,l));
  l[2] = interval(0,1);
  assert(!equal(m,l));
  cout << "[]   1/4x0 + [0,1]x2 - 2x3 + [1,2]:          " << l.get_size() << ", " << l << endl;
  cout << "copy 1/4x0 - 2x3 + [1,2]:                    " <<  m.get_size() << ", " << m << endl;
  swap(m,l);
  assert(!equal(m,l));
  cout << "after swap:                                  " << l.get_size() << ", " << l << endl;
  cout << "after swap:                                  " << m.get_size() << ", " << m << endl;
  for (linexpr0::iterator i = m.begin();!i.end();i++)
    i.get_coeff().neg();
  cout << "iterate neg -1/4x0 + [-1,0]x2 + 2x3 + [1,2]: " << m.get_size() << ", " << m << endl; 
  try { l.get_coeff(5); assert(0); } catch (out_of_range &r) {}
  try { l[5]; assert(0); } catch (out_of_range &r) {}
  l.resize(3);
  cout << "resize 1/4x0 + [1,2]:                        " << l.get_size() << ", " << l << endl;
  l[0] = interval(3,3);
  cout << "[]     [3,3]x0 + [1,2]:                      " << l.get_size() << ", " << l << endl;
  l.minimize();
  cout << "minimize 3x0 + [1,2]:                        " << l.get_size() << ", " << l << endl;
  linexpr0 l2 = linexpr0(l,dim);
  cout << "add dims: 3x1 + [1,2]:                       " << l2 << endl;
  linexpr0 l3 = l;
  l3.add_dimensions(dim);
  cout << "add_dims: 3x1 + [1,2]:                       " << l3 << endl;
  l[1] = interval(3,4);
  linexpr0 l4 = linexpr0(l,perm);
  cout << "permute: [3,4]x0 + 3x1 + [1,2]:              " << l4 << endl;
  l4.permute_dimensions(perm2);
  cout << "permute: 3x0 + [3,4]x1 + [1,2]:              " << l4 << endl;
  cout << "hash: " << l.hash() << endl;
  printf("print: "); l.print(); printf("\n");

  // sparse
  cout << endl << "sparse linexpr0" << endl;
  linexpr0 ll = linexpr0(AP_LINEXPR_SPARSE,5);
  assert(ll.get_discr()==AP_LINEXPR_SPARSE);
  cout << "construct:                                   " << ll.get_size() << ", " << ll << endl;
  ll.get_coeff(0) = frac(1,4);
  ll.get_coeff(3) = -2.;
  assert(ll.is_linear() && ll.is_quasilinear());
  ll.get_cst() = interval(1,2);
  assert(!ll.is_linear() && ll.is_quasilinear());
  cout << "get_ 1/4x0 - 2x3 + [1,2]:                    " << ll.get_size() << ", " << ll << endl;
  linexpr0 mm = ll;
  assert(equal(mm,ll));
  assert(mm.get_discr()==AP_LINEXPR_SPARSE);
  mm.get_coeff(2) = top();
  assert(!equal(mm,ll));
  cout << "copy 1/4x0 + [-oo,+oo]x2 - 2x3 + [1,2]:      " << mm.get_size() << ", " << mm << endl;
  cout << "copy 1/4x0 - 2x3 + [1,2]:             :      " << ll.get_size() << ", " << ll << endl;
  cout << "get_coeff 0: " << ll.get_coeff(1) << endl;
  mm = linexpr0(AP_LINEXPR_DENSE,5);
  assert(mm.get_discr()==AP_LINEXPR_DENSE);
  coeff aa[] = { 1,2,interval(0,1) };
  linexpr0 nn = linexpr0(3,aa,2,AP_LINEXPR_SPARSE);
  cout << "from array 1x0 + 2x1 + [0,1]x2 + 2:          " << nn.get_size() << ", " << nn << endl;  
  mm = linexpr0();
  nn = mm = ll;
  cout << "chained copy 1/4x0 - 2x3 + [1,2]:            " << nn.get_size() << ", " << nn << endl;  
  assert(mm.get_discr()==AP_LINEXPR_SPARSE);
  assert(equal(mm,ll));
  assert(nn.get_discr()==AP_LINEXPR_SPARSE);
  assert(equal(nn,ll));
  ll[2] = interval(0,1);
  assert(!equal(mm,ll));
  cout << "copy 1/4x0 + [0,1]x2 - 2x3 + [1,2]:          " << ll.get_size() << ", " << ll << endl;
  cout << "copy 1/4x0 - 2x3 + [1,2]:                    " << mm.get_size() << ", " << mm << endl;
  swap(mm,ll);
  assert(!equal(mm,ll));
  cout << "after swap:                                  " << ll.get_size() << ", " << ll << endl;
  cout << "after swap:                                  " << mm.get_size() << ", " << mm << endl;
  for (linexpr0::iterator i = mm.begin();!i.end();i++)
    i.get_coeff().neg();
  cout << "iterate neg -1/4x0 + [-1,0]x2 + 2x3 + [1,2]: " << mm.get_size() << ", " << mm << endl; 
  ll.get_coeff(4) = frac(1,2);
  ll[5] = interval(0,1);
  cout << "get_ [] 1/4x0 - 2x3 + 1/2x4 + [0,1]x5 + [1,2]: " << ll.get_size() << ", " << ll << endl;
  ll[6] = 5; 
  ll[7] = 5;
  ll[1] = 6;
  cout << "get_ [] 1/4x0 + 6x1 - 2x3 + 1/2x4 + [0,1]x5 + 5x6 + 5x7 + [1,2]: " << ll.get_size() << ", " << ll << endl;
  ll[0] = interval(3,3);
  ll[1] = 0;
  cout << "before minimize:   " << ll.get_size() << ", " << ll << endl;
  ll.minimize();
  cout << "after minimize:    " << ll.get_size() << ", " << ll << endl;
  linexpr0 ll2 = linexpr0(ll,dim);
  cout << "add dims 3x1 - 2x5 + 1/2x6 + [0,1]x7 + 5x8 + 5x9 + [1,2]: " << ll2 << endl;
  linexpr0 ll3 = ll;
  ll3.add_dimensions(dim);
  cout << "add_dims 3x1 - 2x5 + 1/2x6 + [0,1]x7 + 5x8 + 5x9 + [1,2]: " << ll3 << endl;
  perm = id(8);
  perm[0] = 3; perm[3] = 0;
  linexpr0 ll4 = linexpr0(ll,perm);
  cout << "permute -2x0 + 3x3 + 1/2x4 + [0,1]x5 + 5x6 + 5x7 + [1,2]: " << ll4 << endl;
  perm2 = -perm;
  ll4.permute_dimensions(perm2);
  cout << "permute 3x0 - 2x3 + 1/2x4 + [0,1]x5 + 5x6 + 5x7 + [1,2]:  " << ll4 << endl;
  cout << "hash: " << ll.hash() << endl;
  printf("print: "); ll.print(); printf("\n");
}

void test_lincons0()
{
  cout << endl << "lincons0" << endl;
  coeff a[] = {1,2,3};
  coeff b[] = {4,3,2,1};
  cout << "construct:                               " << lincons0() << endl;
  cout << "construct 1x0 + 2x1 + 3x2 + 4 = 0:       " << lincons0(AP_CONS_EQ,linexpr0(3,a,4)) << endl;
  cout << "construct 1x0 + 2x1 + 3x2 + 4 >= 0:      " << lincons0(AP_CONS_SUPEQ,linexpr0(3,a,4)) << endl;
  cout << "construct 1x0 + 2x1 + 3x2 + 4 > 0:       " << lincons0(AP_CONS_SUP,linexpr0(3,a,4)) << endl;
  cout << "construct 1x0 + 2x1 + 3x2 + 4 != 0:      " << lincons0(AP_CONS_DISEQ,linexpr0(3,a,4)) << endl;
  cout << "construct 1x0 + 2x1 + 3x2 + 4 = 0 mod 5: " << lincons0(AP_CONS_EQMOD,linexpr0(3,a,4),5) << endl;
  lincons0 l = lincons0(AP_CONS_EQ,linexpr0(3,a,4));
  lincons0 m = l;
  assert(!l.is_unsat() && l.is_quasilinear() && l.is_linear());
  cout << "copy 1x0 + 2x1 + 3x2 + 4 = 0:            " << m << endl;
  lincons0 n = lincons0(m,dimchange(2,0,(ap_dim_t[]){0,2}));
  assert(!n.is_unsat() && n.is_quasilinear() && n.is_linear());
  cout << "dimchange 1x1 + 2x2 + 3x4 + 4 = 0:       " << n << endl;
  lincons0 o = lincons0(m,dimperm(3,(ap_dim_t[]){1,0,2}));
  assert(!o.is_unsat() && o.is_quasilinear() && o.is_linear());
  cout << "dimperm 2x0 + 1x1 + 3x2 + 4 = 0:         " << o << endl;
  cout << "unsat:                                   " << lincons0(unsat()) << endl;
  l = lincons0(AP_CONS_SUPEQ,linexpr0(4,b,5));
  cout << "copy 4x0 + 3x1 + 2x2 + 1x3 + 5 >= 0:     " << l << endl;
  l = unsat();
  assert(l.is_unsat());
  cout << "unsat:                                   " << l << endl;
  o = n = lincons0(AP_CONS_EQ,linexpr0(3,a,4));
  cout << "chained copy 1x0 + 2x1 + 3x2 + 4 = 0:    " << o << endl;
  o.add_dimensions(dimchange(2,0,(ap_dim_t[]){0,2}));
  cout << "dimchange 1x1 + 2x2 + 3x4 + 4 = 0:       " << o << endl;
  n.permute_dimensions(dimperm(3,(ap_dim_t[]){1,0,2}));
  cout << "dimperm 2x0 + 1x1 + 3x2 + 4 = 0:         " << n << endl;
  n.resize(2);
  cout << "resize 2x0 + 1x1 + 4 = 0:                " << n << endl;
  assert(n.get_size()==2);
  assert(n.get_constyp()==AP_CONS_EQ);
  n.get_constyp()=AP_CONS_SUP;
  assert(n.get_constyp()==AP_CONS_SUP);
  cout << "get_constyp 2x0 + 1x1 + 4 > 0:           " << n << endl;  
  assert(!n.has_modulo());
  assert(n.has_linexpr0());
  try { n.get_modulo(); assert(0); } catch (invalid_argument& b) {}
  n.set_modulo(2);
  n.set_modulo(3);
  assert(n.has_modulo());
  n.get_constyp()=AP_CONS_EQMOD; 
  cout << "set_modulo 2x0 + 1x1 + 4 = 0 mod 3:      " << n << endl;  
  assert(n.has_modulo());
  assert(n.get_modulo()==3);
  n.set_linexpr0(linexpr0(3,a,4));
  cout << "set_linexpr0 1x0 + 2x1 + 3x2 + 4 = 0 mod 3:               " << n << endl;  
  n.get_linexpr0() = linexpr0(4,b,5);
  cout << "get_linexpr0 4x0 + 3x1 + 2x2 + 1x3 + 5 = 0 mod 3:         " << n << endl;
  n.get_linexpr0().get_cst() = interval(1,2);
  assert(!n.is_unsat() && !n.is_linear() && n.is_quasilinear());
  cout << "get_linexpr0 4x0 + 3x1 + 2x2 + 1x3 + [1,2] = 0 mod 3:     " << n << endl;
  n.get_linexpr0()[0] = interval(0,1);
  assert(!n.is_unsat() && !n.is_linear() && !n.is_quasilinear());
  cout << "get_linexpr0 [0,1]x0 + 3x1 + 2x2 + 1x3 + [1,2] = 0 mod 3: " << n << endl;
  printf("print: "); n.print(); printf("\n");
 }

void test_lincons0_array()
{
  cout << endl << "lincons0_array" << endl;
  coeff a[] = {1,2,3};
  coeff b[] = {5,3};
  lincons0_array t = lincons0_array(2);
  try { cout << t; assert(0); } catch (invalid_argument& r) { cout << endl; }
  t[0] = lincons0(AP_CONS_EQ,linexpr0(3,a,4));
  t[1] = lincons0(AP_CONS_EQ,linexpr0(2,b,99));
  cout << "construct:    " << t << endl;
  lincons0_array u = t;
  cout << "copy:         " << t << endl; 
  lincons0_array v = lincons0_array(u,dimperm(3,(ap_dim_t[]){1,0,2}));
  cout << "permutation: " << v << endl;
  lincons0_array w = lincons0_array(u,dimchange(1,1,(ap_dim_t[]){0,2}));
  cout << "dim change:   " << w << endl;
  lincons0 ar[3] = { lincons0(AP_CONS_EQ,linexpr0(3,a,5)), 
		     lincons0(AP_CONS_EQ,linexpr0(2,b,4)),
		     lincons0(AP_CONS_EQMOD,linexpr0(2,b,3),5)  };
  lincons0_array x = lincons0_array(3,ar);
  cout << "from array:  " << x << endl;
  vector<lincons0> ve = vector<lincons0>(2);
  ve[1] = lincons0(AP_CONS_EQ,linexpr0(2,b,99));
  lincons0_array y = lincons0_array(ve);
  cout << "from vector:  " << y << endl;  
  y = ar;
  cout << "from array:   " << y << endl;
  x = ve;
  cout << "from vector:  " << x << endl;  
  w = v;
  cout << "copy:         " << w << endl;
  w.add_dimensions(dimchange(1,1,(ap_dim_t[]){0,2}));
  cout << "dim change:   " << w << endl;
  w = v;
  w.permute_dimensions(dimperm(3,(ap_dim_t[]){1,0,2}));
  cout << "permutation:  " << w << endl;
  cout << "get_size:     " << w.get_size() << endl;
  assert(w.is_linear() && w.is_quasilinear());
  w[0] = lincons0(AP_CONS_SUPEQ,linexpr0(2,b,interval(1,88)));
  b[0] = interval(0,1);
  cout << "[]:           " << w << endl;
  assert(!w.is_linear() && w.is_quasilinear());
  w.get(1) = lincons0(AP_CONS_SUPEQ,linexpr0(2,b,interval(1,88)));
  cout << "get:          " << w << endl;
  assert(!w.is_linear() && !w.is_quasilinear());
  try { w.get(2); assert(0); } catch (out_of_range& b) {}
  *w.get_contents() = w[1];
  cout << "get_contents: " << w << endl;
  vector<lincons0> vv = w;
  cout << "to vector:    " << vv.size() << ": " << vv[0] << "; " << vv[1] << endl;
}

void test_generator0()
{
  cout << endl << "generator0" << endl;
  coeff a[] = {1,2,3};
  coeff b[] = {4,3,2,1};
  cout << "construct:                              " << generator0() << endl;
  cout << "construct vert 1x0 + 2x1 + 3x2 + 4:     " << generator0(AP_GEN_VERTEX,linexpr0(3,a,4)) << endl;
  cout << "construct ray 1x0 + 2x1 + 3x2 + 4:      " << generator0(AP_GEN_RAY,linexpr0(3,a,4)) << endl;
  cout << "construct line 1x0 + 2x1 + 3x2 + 4:     " << generator0(AP_GEN_LINE,linexpr0(3,a,4)) << endl;
  cout << "construct modray 1x0 + 2x1 + 3x2 + 4:   " << generator0(AP_GEN_RAYMOD,linexpr0(3,a,4)) << endl;
  cout << "construct modline 1x0 + 2x1 + 3x2 + 4:  " << generator0(AP_GEN_LINEMOD,linexpr0(3,a,4)) << endl;
  generator0 l = generator0(AP_GEN_VERTEX,linexpr0(3,a,4));
  generator0 m = l;
  cout << "copy vert 1x0 + 2x1 + 3x2 + 4:          " << m << endl;
  generator0 n = generator0(m,dimchange(2,0,(ap_dim_t[]){0,2}));
  cout << "dimchange vert 1x1 + 2x2 + 3x4 + 4:     " << n << endl;
  generator0 o = generator0(m,dimperm(3,(ap_dim_t[]){1,0,2}));
  cout << "dimperm vert 2x0 + 1x1 + 3x2 + 4:       " << o << endl;
  l = generator0(AP_GEN_RAY,linexpr0(4,b,5));
  cout << "copy ray 4x0 + 3x1 + 2x2 + 1x3 + 5:     " << l << endl;
  o = n = generator0(AP_GEN_LINE,linexpr0(3,a,4));
  cout << "chained copy line 1x0 + 2x1 + 3x2 + 4:  " << o << endl;
  o.add_dimensions(dimchange(2,0,(ap_dim_t[]){0,2}));
  cout << "dimchange line 1x1 + 2x2 + 3x4 + 4:     " << o << endl;
  n.permute_dimensions(dimperm(3,(ap_dim_t[]){1,0,2}));
  cout << "dimperm line 2x0 + 1x1 + 3x2 + 4:       " << n << endl;
  n.resize(2);
  cout << "resize line 2x0 + 1x1 + 4:              " << n << endl;
  assert(n.get_size()==2);
  assert(n.get_gentyp()==AP_GEN_LINE);
  n.get_gentyp()=AP_GEN_LINEMOD;
  assert(n.get_gentyp()==AP_GEN_LINEMOD);
  cout << "get_gentyp modline 2x0 + 1x1 + 4:      " << n << endl;  
  assert(n.has_linexpr0());
  n.set_linexpr0(linexpr0(3,a,4));
  cout << "set_linexpr0 modline 1x0 + 2x1 + 3x2 + 4:  " << n << endl;  
  n.get_linexpr0() = linexpr0(4,b,5);
  cout << "get_linexpr0 modline 4x0 + 3x1 + 2x2 + 1x3 + 5:   " << n << endl;
  n.get_linexpr0().get_cst() = 2;
  cout << "get_linexpr0 modline 4x0 + 3x1 + 2x2 + 1x3 + 2:   " << n << endl;
  n.get_linexpr0()[0] = 18;
  cout << "get_linexpr0 modline 18x0 + 3x1 + 2x2 + 1x3 + 2:  " << n << endl;
  printf("print: "); n.print(); printf("\n");
 }

void test_generator0_array()
{
  cout << endl << "generator0_array" << endl;
  coeff a[] = {1,2,3};
  coeff b[] = {5,3};
  generator0_array t = generator0_array(2);
  try { cout << t; assert(0); } catch (invalid_argument& r) { cout << endl; }
  t[0] = generator0(AP_GEN_VERTEX,linexpr0(3,a,4));
  t[1] = generator0(AP_GEN_VERTEX,linexpr0(2,b,99));
  cout << "construct:    " << t << endl;
  generator0_array u = t;
  cout << "copy:         " << t << endl; 
  generator0_array v = generator0_array(u,dimperm(3,(ap_dim_t[]){1,0,2}));
  cout << "permutation: " << v << endl;
  generator0_array w = generator0_array(u,dimchange(1,1,(ap_dim_t[]){0,2}));
  cout << "dim change:   " << w << endl;
  generator0 ar[3] = { generator0(AP_GEN_VERTEX,linexpr0(3,a,5)), 
		       generator0(AP_GEN_VERTEX,linexpr0(2,b,4)),
		       generator0(AP_GEN_RAY,linexpr0(2,b,3))  };
  generator0_array x = generator0_array(3,ar);
  cout << "from array:  " << x << endl;
  vector<generator0> ve = vector<generator0>(2);
  ve[1] = generator0(AP_GEN_VERTEX,linexpr0(2,b,99));
  generator0_array y = generator0_array(ve);
  cout << "from vector:  " << y << endl;  
  y = ar;
  cout << "from array:   " << y << endl;
  x = ve;
  cout << "from vector:  " << x << endl;  
  w = v;
  cout << "copy:         " << w << endl;
  w.add_dimensions(dimchange(1,1,(ap_dim_t[]){0,2}));
  cout << "dim change:   " << w << endl;
  w = v;
  w.permute_dimensions(dimperm(3,(ap_dim_t[]){1,0,2}));
  cout << "permutation:  " << w << endl;
  cout << "get_size:     " << w.get_size() << endl;
  w[0] = generator0(AP_GEN_LINE,linexpr0(2,b,88));
  b[0] = 99;
  cout << "[]:           " << w << endl;
  w.get(1) = generator0(AP_GEN_RAY,linexpr0(2,b,88));
  cout << "get:          " << w << endl;
  try { w.get(2); assert(0); } catch (out_of_range& b) {}
  *w.get_contents() = w[1];
  cout << "get_contents: " << w << endl;
  vector<generator0> vv = w;
  cout << "to vector:    " << vv.size() << ": " << vv[0] << "; " << vv[1] << endl;
}

void test_texpr0()
{
  cout << endl << "texpr0" << endl;
  coeff a[] = {1,2,3};

  // constructors
  cout << "construct 2:              " << texpr0(2) << endl;
  cout << "construct 2L:             " << texpr0(2L) << endl;
  cout << "construct 2.2:            " << texpr0(2.2) << endl;
  cout << "construct mpq(1,2):       " << texpr0(mpq_class(1,2)) << endl;
  cout << "construct coeff(2):       " << texpr0(coeff(2)) << endl;
  cout << "construct scalar(2):      " << texpr0(scalar(2)) << endl;
  cout << "construct 1/2:            " << texpr0(frac(1,2)) << endl;
  cout << "construct interval(1,2):  " << texpr0(interval(1,2)) << endl;
  cout << "construct [2,3]:          " << texpr0(2,3) << endl;
  cout << "construct [2L,3L]:        " << texpr0(2L,3L) << endl;
  cout << "construct [2.2,3.3]:      " << texpr0(2.2,3.3) << endl;
  cout << "construct [1/2,3/4]:      " << texpr0(frac(1,2),frac(3,4)) << endl;
  cout << "construct top:            " << texpr0(top()) << endl;
  cout << "construct x2:             " << texpr0(dim(2)) << endl;
  cout << "construct +x2:            " << texpr0(+dim(2)) << endl;
  cout << "construct -x2:            " << texpr0(-dim(2)) << endl;
  cout << "construct x2+2:           " << texpr0(dim(2)+2) << endl;
  cout << "construct x2-2:           " << texpr0(dim(2)-2) << endl;
  cout << "construct x2*2:           " << texpr0(dim(2)*2) << endl;
  cout << "construct x2/2:           " << texpr0(dim(2)/2) << endl;
  cout << "construct x2%2:           " << texpr0(dim(2)%2) << endl;
  cout << "construct sqrt(x2):       " << texpr0(sqrt(dim(2))) << endl;
  cout << "construct float -x2:      " << texpr0(neg(dim(2),AP_RTYPE_SINGLE)) << endl;
  cout << "construct float x2+2:     " << texpr0(add(dim(2),2,AP_RTYPE_SINGLE)) << endl;
  cout << "construct float x2-2:     " << texpr0(sub(dim(2),2,AP_RTYPE_SINGLE)) << endl;
  cout << "construct float x2*2:     " << texpr0(mul(dim(2),2,AP_RTYPE_SINGLE)) << endl;
  cout << "construct float x2/2:     " << texpr0(div(dim(2),2,AP_RTYPE_SINGLE)) << endl;
  cout << "construct float x2%2:     " << texpr0(mod(dim(2),2,AP_RTYPE_SINGLE)) << endl;
  cout << "construct float sqrt(x2): " << texpr0(sqrt(dim(2),AP_RTYPE_SINGLE)) << endl;
  cout << "construct float cast(x2): " << texpr0(cast(dim(2),AP_RTYPE_SINGLE)) << endl;
  cout << "construct floor(x2):      " << texpr0(floor(dim(2))) << endl;
  cout << "construct ceil(x2):       " << texpr0(ceil(dim(2))) << endl;
  cout << "construct trunc(x2):      " << texpr0(trunc(dim(2))) << endl;
  cout << "construct linexpr:        " << texpr0(linexpr0(3,a,4)) << endl;

  texpr0 x = 2+3*dim(0);
  texpr0 y = x;
  cout << "copy x=2+3*x0:            " << y << endl;
  cout << "construct (x+x)/(x*x):    " << (x+x)/(x*x) << endl;
  cout << "construct x0 + 2:              " << dim(0)+2 << endl;
  cout << "construct x0 + 2L:             " << dim(0)+2L << endl;
  cout << "construct x0 + 2.2:            " << dim(0)+2.2 << endl;
  cout << "construct x0 + mpq(1,2):       " << dim(0)+mpq_class(1,2) << endl;
  cout << "construct x0 + coeff(2):       " << dim(0)+coeff(2) << endl;
  cout << "construct x0 + scalar(2):      " << dim(0)+scalar(2) << endl;
  cout << "construct x0 + 1/2:            " << dim(0)+frac(1,2) << endl;
  cout << "construct x0 + interval(1,2):  " << dim(0)+interval(1,2) << endl;
  cout << "construct x0 + [2,3]:          " << dim(0)+texpr0_node(2,3) << endl;
  cout << "construct x0 + [2L,3L]:        " << dim(0)+texpr0_node(2L,3L) << endl;
  cout << "construct x0 + [2.2,3.3]:      " << dim(0)+texpr0_node(2.2,3.3) << endl;
  cout << "construct x0 + [1/2,3/4]:      " << dim(0)+texpr0_node(frac(1,2),frac(3,4)) << endl;
  cout << "construct x0 + top:            " << dim(0)+top() << endl;
  cout << "construct x0 + x2:             " << dim(0)+dim(2) << endl;
  cout << "construct x0 + linexpr:        " << dim(0)+texpr0(linexpr0(3,a,4)) << endl;

  try { unary(AP_TEXPR_ADD,2); assert(0); } catch (invalid_argument& a) {}
  try { binary(AP_TEXPR_NEG,1,2); assert(0); } catch (invalid_argument& a) {}

  // assignments
  y = 2; cout << "assign 2:              " << y << endl;
  y = 2L; cout << "assign 2L:             " << y << endl;
  y = 2.2; cout << "assign 2.2:            " << y << endl;
  y = mpq_class(1,2); cout << "assign mpq(1,2):       " << y << endl;
  y = coeff(2); cout << "assign coeff(2):       " << y << endl;
  y = scalar(2); cout << "assign scalar(2):      " << y << endl;
  y = interval(1,2); cout << "assign interval(1,2):  " << y << endl;
  y = top(); cout << "assign top:            " << y << endl;
  y = dim(2); cout << "assign x2:             " << y << endl;
  y = linexpr0(3,a,4); cout << "assign linexpr:        " << y << endl;
  y = x; cout << "assign x:              " << y << endl;
  y = x+1; cout << "assign x+1:            " << y << endl;
  x = x*x; cout << "assign x*x:            " << x << endl;
  x = y = sqrt(dim(0)+1); cout << "chained assign sqrt(x0+1): " << x << "; " << y << endl;

  // dimensions
  dimchange dimc = dimchange(1,1,(ap_dim_t[]){0,2});
  dimperm perm  = dimperm(3,(ap_dim_t[]){1,0,2});
  x = dim(0)/dim(1);
  cout << "add dim x1/x2:         " << texpr0(x,dimc) << endl;
  cout << "del dim ]-oo;+oo[/x0:  " << texpr0(x,dimc,false) << endl;
  cout << "dim perm x1/x0:        " << texpr0(x,perm) << endl;
  x.add_dimensions(dimc);
  cout << "add dim x1/x2:         " << x << endl;
  x.remove_dimensions(-dimc);
  cout << "del dim x0/x1:         " << x << endl;
  x.permute_dimensions(perm);
  cout << "dim perm x1/x0:        " << x << endl; 
  y = texpr0(x,perm)+texpr0(x,dimc,false);
  cout << "dim mix x0/x1+x0/]-oo;+oo[:  " << y << endl;

  // access && tests
  x = 2;
  assert(x.max_dim()==0);
  assert(x.depth()==0);
  assert(x.size()==0);
  assert(equal(x,2));
  assert(!equal(x,3) && !equal(x,dim(3)) && !equal(x,dim(4)+dim(3)));
  assert(x.get_discr()==AP_TEXPR_CST);
  assert(x.is_interval_cst() && x.is_interval_linear() && x.is_interval_polynomial() && 
	 x.is_interval_polyfrac() && x.is_scalar());
  try { x.get_dim(); assert(0); } catch (bad_discriminant& b) {}
  try { x.get_op(); assert(0); } catch (bad_discriminant& b) {}
  try { x.get_rtype(); assert(0); } catch (bad_discriminant& b) {}
  try { x.get_rdir(); assert(0); } catch (bad_discriminant& b) {}
  try { x.get_argA(); assert(0); } catch (bad_discriminant& b) {}
  try { x.get_argB(); assert(0); } catch (bad_discriminant& b) {}
  x.get_coeff() = interval(1,2); cout << "get_coeff [1,2]: " << x << endl;
  assert(!equal(x,2) && equal(x,interval(1,2)));
  assert(x.is_interval_cst() && x.is_interval_linear() && x.is_interval_polynomial() && 
	 x.is_interval_polyfrac() && !x.is_scalar());
  x = dim(2);
  assert(x.max_dim()==2);
  assert(x.depth()==0);
  assert(x.size()==0);
  assert(equal(x,dim(2)));
  assert(!equal(x,3) && !equal(x,dim(3)) && !equal(x,dim(4)+dim(3)));
  assert(x.get_discr()==AP_TEXPR_DIM);
  assert(!x.is_interval_cst() && x.is_interval_linear() && x.is_interval_polynomial() && 
	 x.is_interval_polyfrac() && x.is_scalar());
  try { x.get_coeff(); assert(0); } catch (bad_discriminant& b) {}
  try { x.get_op(); assert(0); } catch (bad_discriminant& b) {}
  try { x.get_rtype(); assert(0); } catch (bad_discriminant& b) {}
  try { x.get_rdir(); assert(0); } catch (bad_discriminant& b) {}
  try { x.get_argA(); assert(0); } catch (bad_discriminant& b) {}
  try { x.get_argB(); assert(0); } catch (bad_discriminant& b) {}
  x.get_dim() = 3;   cout << "get_dim 3: " << x << endl;
  assert(!equal(x,dim(2)) && equal(x,dim(3)));
  x = add(dim(0),1,AP_RTYPE_SINGLE,AP_RDIR_NEAREST);
  assert(!x.is_interval_cst() && !x.is_interval_linear() && !x.is_interval_polynomial() && 
	 !x.is_interval_polyfrac() && x.is_scalar());
  assert(x.depth()==1);
  assert(x.size()==1);
  assert(equal(x,add(dim(0),1,AP_RTYPE_SINGLE,AP_RDIR_NEAREST)));
  assert(!equal(x,3) && !equal(x,dim(3)) && !equal(x,dim(4)+dim(3)));
  assert(x.get_discr()==AP_TEXPR_NODE);
  try { x.get_coeff(); assert(0); } catch (bad_discriminant& b) {}
  try { x.get_dim(); assert(0); } catch (bad_discriminant& b) {}
  assert(x.get_op()==AP_TEXPR_ADD);
  assert(is_binop(x.get_op()));
  assert(x.get_rtype()==AP_RTYPE_SINGLE);
  assert(x.get_rdir()==AP_RDIR_NEAREST);
  assert(equal(x.get_argA(),dim(0)));
  assert(equal(x.get_argB(),1));
  x.get_op()=AP_TEXPR_SUB;
  x.get_rtype()=AP_RTYPE_DOUBLE;
  x.get_rdir()=AP_RDIR_ZERO;
  x.get_argA()=2;
  x.get_argB()=dim(1);
  cout << "get_op rtype rdir argA argB: " << x << endl;
  assert(!equal(x,add(dim(0),1,AP_RTYPE_SINGLE,AP_RDIR_NEAREST)));
  assert(equal(x,sub(2,dim(1),AP_RTYPE_DOUBLE,AP_RDIR_ZERO)));
  x.get_rtype()=AP_RTYPE_REAL;
  assert(!x.is_interval_cst() && x.is_interval_linear() && x.is_interval_polynomial() && 
	 x.is_interval_polyfrac() && x.is_scalar());
  x.get_op()=AP_TEXPR_MUL;
  assert(!x.is_interval_cst() && x.is_interval_linear() && x.is_interval_polynomial() && 
	 x.is_interval_polyfrac() && x.is_scalar());
  x.get_op()=AP_TEXPR_DIV;
  assert(!x.is_interval_cst() && !x.is_interval_linear() && !x.is_interval_polynomial() && 
	 x.is_interval_polyfrac() && x.is_scalar());
  x = sqrt(dim(1),AP_RTYPE_SINGLE,AP_RDIR_NEAREST);
  assert(!x.is_interval_cst() && !x.is_interval_linear() && !x.is_interval_polynomial() && 
	 !x.is_interval_polyfrac() && x.is_scalar());
  assert(x.depth()==1);
  assert(x.max_dim()==1);
  assert(!equal(x,3) && !equal(x,dim(3)) && !equal(x,dim(4)+dim(3)));
  assert(equal(x,sqrt(dim(1),AP_RTYPE_SINGLE,AP_RDIR_NEAREST)));
  assert(x.get_discr()==AP_TEXPR_NODE);
  try { x.get_coeff(); assert(0); } catch (bad_discriminant& b) {}
  try { x.get_dim(); assert(0); } catch (bad_discriminant& b) {}
  assert(x.get_op()==AP_TEXPR_SQRT);
  assert(is_unop(x.get_op()));
  assert(x.get_rtype()==AP_RTYPE_SINGLE);
  assert(x.get_rdir()==AP_RDIR_NEAREST);
  assert(equal(x.get_argA(),dim(1)));
  try { x.get_argB(); assert(0); } catch (out_of_range& b) {}  
  x.get_op()=AP_TEXPR_CAST;
  x.get_rtype()=AP_RTYPE_DOUBLE;
  x.get_rdir()=AP_RDIR_ZERO;
  x.get_argA()=2+dim(3);
  cout << "get_op rtype rdir argA: " << x << endl;
  assert(!equal(x,sqrt(dim(3),AP_RTYPE_SINGLE,AP_RDIR_NEAREST)));
  assert(equal(x,cast(2+dim(3),AP_RTYPE_DOUBLE,AP_RDIR_ZERO)));
  assert(x.size()==2);
  assert(x.depth()==2);
  assert(x.max_dim()==3);
  x = dim(0)+dim(5)+dim(2);
  assert(x.has_dim(0) && !x.has_dim(1) && x.has_dim(2) && !x.has_dim(3) && 
	 !x.has_dim(4) && x.has_dim(5) && !x.has_dim(6));
  vector<ap_dim_t> dm = x.dimlist();
  assert(dm.size()==3 && dm[0]==0 && dm[1]==2 && dm[2]==5);

  // substitutions
  x = sqrt(dim(0)+2*dim(1)*dim(1));
  cout << "subst: " << texpr0(x,0,dim(0)+1) << endl;
  assert(equal(x,sqrt(dim(0)+2*dim(1)*dim(1))));
  x.substitute(1,sqrt(dim(0))+1);
  cout << "subst: " << x << endl;
  assert(equal(x,sqrt(dim(0)+2*(sqrt(dim(0))+1)*(sqrt(dim(0))+1))));

  // hash & print
   x = sqrt(dim(0)+2*dim(1)*dim(1));
   cout << "hash: " << x.hash() << endl;
   printf("print: "); x.print(); printf("\n");
}


void test_tcons0()
{
  cout << endl << "tcons0" << endl;
  coeff a[] = {1,2,3};
  lincons0 ll = lincons0(AP_CONS_EQ,linexpr0(3,a,4));
  try { cout << tcons0(); assert(0); } catch (invalid_argument& r) { }
  cout << "construct x0 + 1 = 0:                " << tcons0(AP_CONS_EQ,dim(0)+1) << endl;
  cout << "construct floor(x0+2) >= 0:          " << tcons0(AP_CONS_SUPEQ,floor(dim(0)+2)) << endl;
  cout << "construct x0*x1 > 0:                 " << tcons0(AP_CONS_SUP,dim(0)*dim(1)) << endl;
  cout << "construct [-1,1]*x0 != 0:            " << tcons0(AP_CONS_DISEQ,interval(-1,1)*dim(0)) << endl;
  cout << "construct x0/x1 = 0 mod 5:           " << tcons0(AP_CONS_EQMOD,dim(0)/dim(1),5) << endl;
  cout << "from lincons 1x0+2x1+3x2+4 = 0:      " << tcons0(ll) << endl;
  cout << "construct x0 >= x2+1:                " << (dim(0)>=dim(2)+1) << endl;
  cout << "construct x0 <= x2+1:                " << (dim(0)<=dim(2)+1) << endl;
  cout << "construct x0 >  x2+1:                " << (dim(0)>dim(2)+1) << endl;
  cout << "construct x0 <  x2+1:                " << (dim(0)<dim(2)+1) << endl;
  cout << "construct x0 == x2+1:                " << (dim(0)==dim(2)+1) << endl;
  cout << "construct x0 != x2+1:                " << (dim(0)!=dim(2)+1) << endl;
  cout << "construct x0 == 0:                   " << (dim(0)==0) << endl;
  cout << "construct 0 != x2+1:                 " << (0!=dim(2)+1) << endl;
  tcons0 l = tcons0(AP_CONS_EQ,dim(0)*dim(2));
  tcons0 m = l;
  assert(!l.is_interval_cst() && !l.is_interval_linear() && l.is_interval_polynomial() && 
	 l.is_interval_polyfrac() && l.is_scalar());
  cout << "copy x0 * x2 = 0:                    " << m << endl;
  tcons0 n = tcons0(m,dimchange(2,0,(ap_dim_t[]){0,2}));
  cout << "dimchange x1 * x4 = 0:               " << n << endl;
  tcons0 o = tcons0(m,dimperm(3,(ap_dim_t[]){1,0,2}));
  cout << "dimperm x1 * x2 = 0:                 " << o << endl;
  cout << "unsat:                               " << tcons0(unsat()) << endl;
  l = tcons0(AP_CONS_SUPEQ,dim(0));
  l = unsat();
  cout << "unsat:                               " << l << endl;
  l = ll;
  cout << "from lincons 1x0+2x1+3x2+4 = 0:      " << l << endl;
  assert(!l.is_interval_cst() && l.is_interval_linear() && l.is_interval_polynomial() && 
	 l.is_interval_polyfrac() && l.is_scalar());
  o = n = tcons0(AP_CONS_EQ,dim(0)*dim(2)+2);
  cout << "chained copy x0 * x2 +2 = 0:         " << o << endl;
  o.add_dimensions(dimchange(2,0,(ap_dim_t[]){0,2}));
  cout << "dimchange x1 * x4 + 2 = 0:           " << o << endl;
  o = tcons0(AP_CONS_EQ,dim(0)*dim(3)+2);
  o.remove_dimensions(dimchange(2,0,(ap_dim_t[]){0,2}));
  cout << "dimchange ]-oo;+oo[ * x1 + 2 = 0:    " << o << endl;
  n.permute_dimensions(dimperm(3,(ap_dim_t[]){1,0,2}));
  cout << "dimperm x1 * x2 + 2 = 0:              " << n << endl;
  assert(n.get_constyp()==AP_CONS_EQ);
  n.get_constyp()=AP_CONS_SUP;
  assert(n.get_constyp()==AP_CONS_SUP);
  cout << "get_constyp x1 * x2 + 2 > 0:         " << n << endl;  
  assert(!n.has_modulo());
  assert(n.has_texpr0());
  try { n.get_modulo(); assert(0); } catch (invalid_argument& b) {}
  n.set_modulo(2);
  n.set_modulo(3);
  assert(n.has_modulo());
  n.get_constyp()=AP_CONS_EQMOD; 
  cout << "set_modulo x1 * x2 + 2 = 0 mod 3:    " << n << endl;  
  assert(!n.is_interval_cst() && !n.is_interval_linear() && l.is_interval_polynomial() && 
	 n.is_interval_polyfrac() && n.is_scalar());
  assert(n.has_modulo());
  assert(n.get_modulo()==3);
  n.set_texpr0(sqrt(dim(0)+1));
  cout << "set_texpr0 sqrt(x0+1) = 0 mod 3:     " << n << endl;  
  assert(!n.is_interval_cst() && !n.is_interval_linear() && !n.is_interval_polynomial() && 
	 !n.is_interval_polyfrac() && n.is_scalar());
  n.get_texpr0() = dim(0)/dim(1)+5;
  cout << "get_texpr0 x0/x1+5 = 0 mod 3:        " << n << endl;
  assert(!n.is_interval_cst() && !n.is_interval_linear() && !n.is_interval_polynomial() && 
	 n.is_interval_polyfrac() && n.is_scalar());
  n.get_texpr0().get_argA().get_argA() = interval(0,1);
  cout << "get_texpr0 [0,1]/x1+5 = 0 mod 3:     " << n << endl;
  assert(!n.is_interval_cst() && !n.is_interval_linear() && !n.is_interval_polynomial() && 
	 n.is_interval_polyfrac() && !n.is_scalar());
  printf("print: "); n.print(); printf("\n");
}


void test_tcons0_array()
{
  cout << endl << "tcons0_array" << endl;
  coeff a[] = {1,2,3};
  coeff b[] = {5,3};
  tcons0_array t = tcons0_array(2);
  try { cout << t; assert(0); } catch (invalid_argument& r) { cout << endl; }
  t[0] = tcons0(AP_CONS_SUPEQ,sqrt(dim(0)+15*dim(2)));
  t[1] = tcons0(AP_CONS_EQ,interval(2,5));
  cout << "construct:    " << t << endl;
  tcons0_array u = t;
  cout << "copy:         " << t << endl; 
  tcons0_array v = tcons0_array(u,dimperm(3,(ap_dim_t[]){1,0,2}));
  cout << "permutation:  " << v << endl;
  tcons0_array w = tcons0_array(u,dimchange(1,1,(ap_dim_t[]){0,2}));
  cout << "add dim:      " << w << endl;
  tcons0_array ww = tcons0_array(u,dimchange(1,1,(ap_dim_t[]){0,2}),false);
  cout << "rem dim:      " << ww << endl;
  tcons0 ar[3] = { tcons0(AP_CONS_SUPEQ,dim(0)*dim(1)*dim(2)),
		   tcons0(AP_CONS_EQ,dim(0)%dim(1)),
		   tcons0(AP_CONS_EQMOD,interval(1,2)*dim(0),5)  };
  tcons0_array x = tcons0_array(3,ar);
  cout << "from array:   " << x << endl;
  vector<tcons0> ve = vector<tcons0>(2);
  ve[0] = tcons0(AP_CONS_EQ,0);
  ve[1] = tcons0(AP_CONS_EQ,1-dim(5));
  tcons0_array y = tcons0_array(ve);
  cout << "from vector:  " << y << endl;  
  y = ar;
  cout << "from array:   " << y << endl;
  x = ve;
  cout << "from vector:  " << x << endl;  
  w = u;
  cout << "copy:         " << w << endl;
  w.add_dimensions(dimchange(1,1,(ap_dim_t[]){0,2}));
  cout << "add dim:      " << w << endl;
  w = u;
  w.remove_dimensions(dimchange(1,1,(ap_dim_t[]){0,2}));
  cout << "del dim:      " << w << endl;
  w = u;
  w.permute_dimensions(dimperm(3,(ap_dim_t[]){1,0,2}));
  cout << "permutation:  " << w << endl;
  cout << "get_size:     " << w.get_size() << endl;
  assert(!w.is_interval_linear());
  w[0] = tcons0(AP_CONS_SUPEQ,dim(1)*5+dim(2));
  b[0] = interval(0,1);
  cout << "[]:           " << w << endl;
  assert(w.is_interval_linear());
  w.get(1) = tcons0(AP_CONS_SUPEQ,dim(0)*dim(2));
  cout << "get:          " << w << endl;
  try { w.get(2); assert(0); } catch (out_of_range& b) {}
  *w.get_contents() = w[1];
  cout << "get_contents: " << w << endl;
  vector<tcons0> vv = w;
  cout << "to vector:    " << vv.size() << ": " << vv[0] << "; " << vv[1] << endl;

  // from lincons0_array
  lincons0_array lt = lincons0_array(2);
  lt[0] = lincons0(AP_CONS_EQ,linexpr0(3,a,4));
  lt[1] = lincons0(AP_CONS_EQ,linexpr0(2,b,99));
  tcons0_array A = lt;
  cout << "from lincons0_array: " << A << endl;
  v = lt;
  cout << "from lincons0_array: " << v << endl;    
}

void test_var()
{
  cout << endl << "var" << endl;
  /* TODO */
}

void test_envrionment()
{
  cout << endl << "envrionment" << endl;
  /* TODO */
}

int main()
{
#if 0
  test_scalar();
  test_interval();
  test_interval_array();
  test_coeff();
  test_dimchange();
  test_dimperm();
  test_linexpr0();
  test_lincons0();
  test_lincons0_array();
  test_generator0();
  test_generator0_array();
  test_texpr0();
  test_tcons0();
  test_tcons0_array();
#endif
  test_var();
  test_envrionment();
  return 0;
}
