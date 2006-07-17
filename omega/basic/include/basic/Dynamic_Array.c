#include <basic/assert.h>
#include <basic/Dynamic_Array.h>

template<class T, int d> void Dynamic_Array<T,d>::do_constr()
    {
#if ! defined SHUT_UP_ABOUT_STATEMENT_WITH_NO_EFFECT_IN_DYNAMIC_ARRAY_CREATION
    assert(d > 0);
#endif
    bounds = NULL;
    elements = NULL;
    partial = false;
    }

template<class T> void Dynamic_Array1<T>::do_construct(int d0)
    {
      Dynamic_Array<T,1>::bounds = new int[1];
      Dynamic_Array<T,1>::bounds[0] = d0;
      Dynamic_Array<T,1>::elements = new T [d0];
      Dynamic_Array<T,1>::partial = false;
    }

template<class T> void Dynamic_Array2<T>::do_construct(int d0, int d1)
    {
      Dynamic_Array<T,2>::bounds = new int[2];
      Dynamic_Array<T,2>::bounds[0] = d0;
      Dynamic_Array<T,2>::bounds[1] = d1;
      Dynamic_Array<T,2>::elements = new T [d0 * d1];
      Dynamic_Array<T,2>::partial = false;
    }

template<class T> void Dynamic_Array3<T>::do_construct(int d0,int d1,int d2)
    {
      Dynamic_Array<T,3>::bounds = new int[3];
      Dynamic_Array<T,3>::bounds[0] = d0;
      Dynamic_Array<T,3>::bounds[1] = d1;
      Dynamic_Array<T,3>::bounds[2] = d2;
      Dynamic_Array<T,3>::elements = new T [d0 * d1 * d2];
      Dynamic_Array<T,3>::partial = false;
    }

template<class T> void Dynamic_Array4<T>::do_construct(int d0,int d1,int d2,int d3)
    {
      Dynamic_Array<T,4>::bounds = new int[4];
      Dynamic_Array<T,4>::bounds[0] = d0;
      Dynamic_Array<T,4>::bounds[1] = d1;
      Dynamic_Array<T,4>::bounds[2] = d2;
      Dynamic_Array<T,4>::bounds[3] = d3;
      Dynamic_Array<T,4>::elements = new T [d0 * d1 * d2 * d3];
      Dynamic_Array<T,4>::partial = false;
    }

template<class T, int d> Dynamic_Array<T,d>::Dynamic_Array()
    {
      Dynamic_Array<T,d>::do_constr();
    }

template<class T> Dynamic_Array1<T>::Dynamic_Array1(char *)
    {
      Dynamic_Array<T,1>::do_constr();
    }

template<class T> Dynamic_Array2<T>::Dynamic_Array2(char *,char *)
    {
      Dynamic_Array<T,2>::do_constr();
    }

template<class T> Dynamic_Array3<T>::Dynamic_Array3(char *,char *,char *)
    {
      Dynamic_Array<T,3>::do_constr();
    }

template<class T> Dynamic_Array4<T>::Dynamic_Array4(char *,char *,char *,char *)
    {
      Dynamic_Array<T,4>::do_constr();
    }

template<class T> Dynamic_Array1<T>::Dynamic_Array1(int d0)
    {
    do_construct(d0);
    } 

template<class T> Dynamic_Array2<T>::Dynamic_Array2(int d0, int d1)
    {
    do_construct(d0, d1);
    }

template<class T> Dynamic_Array3<T>::Dynamic_Array3(int d0,int d1,int d2)
    {
    do_construct(d0, d1, d2);
    }

template<class T> Dynamic_Array4<T>::Dynamic_Array4(int d0,int d1,int d2,int d3)
    {
    do_construct(d0, d1, d2, d3);
    }


template<class T, int d> void Dynamic_Array<T,d>::do_destruct()
    {
    if (! partial)
	{
        delete [] bounds;
        delete [] elements;
	}
    }


template<class T, int d> Dynamic_Array<T,d>::~Dynamic_Array()
    {
    do_destruct();
    }


template<class T> void Dynamic_Array1<T>::resize(int d0)
    {
    assert(!(Dynamic_Array<T,1>::partial));
    Dynamic_Array<T,1>::do_destruct();
    if (d0 == 0)
        Dynamic_Array<T,1>::do_constr();
    else
        do_construct(d0);
    } 

template<class T> void Dynamic_Array2<T>::resize(int d0, int d1)
    {
    assert(!(Dynamic_Array<T,2>::partial));
    Dynamic_Array<T,2>::do_destruct();
    if (d0 == 0 && d1 == 0)
        Dynamic_Array<T,2>::do_constr();
    else
        do_construct(d0, d1);
    }

template<class T> void Dynamic_Array3<T>::resize(int d0, int d1, int d2)
    {
    assert(!(Dynamic_Array<T,3>::partial));
    Dynamic_Array<T,3>::do_destruct();
    if (d0 == 0 && d1 == 0 && d2 == 0)
        Dynamic_Array<T,3>::do_constr();
    else
        do_construct(d0, d1, d2);
    }

template<class T> void Dynamic_Array4<T>::resize(int d0, int d1, int d2, int d3)
    {
    assert(!(Dynamic_Array<T,4>::partial));
    Dynamic_Array<T,4>::do_destruct();
    if (d0 == 0 && d1 == 0 && d2 == 0 && d3 == 0)
        Dynamic_Array<T,4>::do_constr();
    else
        do_construct(d0, d1, d2, d3);
    }


template<class T> T& Dynamic_Array1<T>::operator[](int d0)
    { 
#if !defined (NDEBUG)
    assert((Dynamic_Array<T,1>::elements) != NULL && "Trying to dereference undefined array");
    assert(0 <= d0 && d0 < (Dynamic_Array<T,1>::bounds[0]) && "Array subscript out of bounds");
#endif

    return Dynamic_Array<T,1>::elements[d0];
    }

template<class T>  Dynamic_Array1<T> Dynamic_Array2<T>::operator[](int d0)
    { 
#if !defined (NDEBUG)
    assert((Dynamic_Array<T,2>::elements) != NULL && "Trying to dereference undefined array");
    assert(0 <= d0 && d0 < (Dynamic_Array<T,2>::bounds[0]) && "Array subscript out of bounds");
#endif

    Dynamic_Array1<T> result;
    result.bounds = Dynamic_Array<T,2>::bounds+1;
    result.elements = Dynamic_Array<T,2>::elements + Dynamic_Array<T,2>::bounds[1] * d0;
    result.partial = true;
    return result;
    }

template<class T>  Dynamic_Array2<T> Dynamic_Array3<T>::operator[](int d0)
    { 
#if !defined (NDEBUG)
    assert((Dynamic_Array<T,3>::elements) != NULL && "Trying to dereference undefined array");
    assert(0 <= d0 && d0 < (Dynamic_Array<T,3>::bounds[0]) && "Array subscript out of bounds");
#endif
    Dynamic_Array2<T> result;
    result.bounds = Dynamic_Array<T,3>::bounds+1;
    result.elements = Dynamic_Array<T,3>::elements + Dynamic_Array<T,3>::bounds[1] * Dynamic_Array<T,3>::bounds[2] * d0;
    result.partial = true;
    return result;
    } 

template<class T>  Dynamic_Array3<T> Dynamic_Array4<T>::operator[](int d0)
    { 
#if !defined (NDEBUG)
    assert((Dynamic_Array<T,4>::elements) != NULL && "Trying to dereference undefined array");
    assert(0 <= d0 && d0 < (Dynamic_Array<T,4>::bounds[0]) && "Array subscript out of bounds");
#endif

    Dynamic_Array3<T> result;
    result.bounds = Dynamic_Array<T,4>::bounds+1;
    result.elements = Dynamic_Array<T,4>::elements + Dynamic_Array<T,4>::bounds[1] * Dynamic_Array<T,4>::bounds[2] * Dynamic_Array<T,4>::bounds[3] * d0;
    result.partial = true;
    return result;
    } 


template<class T, int d> 
    Dynamic_Array<T,d>::Dynamic_Array(Dynamic_Array<T,d> &D)
    {
    assert(D.elements != NULL && "Trying to copy an undefined array");
    partial = true;
    bounds = D.bounds;
    elements = D.elements;
    }
