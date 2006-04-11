/* ********************************************************************** */
/* pk_meetjoin.h: Meet and join operations */
/* ********************************************************************** */

#ifndef _PK_MEETJOIN_H_
#define _PK_MEETJOIN_H_

#include "pk_config.h"
#include "pk_vector.h"
#include "pk_satmat.h"
#include "pk_matrix.h"
#include "pk.h"

/* ********************************************************************** */
/* I. Meet/Join */
/* ********************************************************************** */

/* In all the following functions, for the structure poly_t:

   - If meet is true, standard meaning of everything
   - If meet is false,
     - matrices C and F, satC and satF have been exchanged,
     - nbeq and nbline have been exchanged
     - in status, nothing has changed
*/

/* Meet/Join of a polyhedronwith an array of constraints/generators.

  The polyhedron is supposed:
   - to have constraints,
   - if lazy is false, to be minimized, and to have satC
   - possible emptiness not detected,

   Matrix of constraints is supposed:
   - to be sorted
   - to be canonical too.

   Return true if exception
 */
bool _poly_meet_matrix(bool meet, bool lazy,
		       ap_manager_t* man,
		       poly_t* po, 
		       const poly_t* pa, matrix_t* mat);

/* Meet/Join of two polyhedra */
void _poly_meet(bool meet,
		bool lazy,
		ap_manager_t* man,
		poly_t* po, const poly_t* pa, const poly_t* pb);
/* Meet/Join of an array of polyhedra */
poly_t* _poly_meet_array(bool meet,
			 bool lazy,
			 ap_manager_t* man,
			 const poly_t** po, size_t size);

/* ********************************************************************** */
/* II. Meet */
/* ********************************************************************** */

void _poly_meet_lincons_array(bool lazy,
			      ap_manager_t* man,
			      poly_t* po, 
			      const poly_t* pa, const ap_lincons0_array_t* array);
poly_t* poly_meet(ap_manager_t* man, bool destructive, poly_t* a1, const poly_t* a2);
poly_t* poly_meet_array(ap_manager_t* man, const poly_t** tab, size_t size);
poly_t* poly_meet_lincons_array(ap_manager_t* man,
				bool destructive, poly_t* a,
				const ap_lincons0_array_t* array);

/* ********************************************************************** */
/* II. Join */
/* ********************************************************************** */

void _poly_add_ray_array(bool lazy, 
			 ap_manager_t* man,
			 poly_t* po, const poly_t* pa, const ap_generator0_array_t* array);

poly_t* poly_join(ap_manager_t* man, bool destructive, poly_t* a1, const poly_t* a2);
poly_t* poly_join_array(ap_manager_t* man, const poly_t** po, size_t size);
poly_t* poly_add_ray_array(ap_manager_t* man, 
			   bool destructive, poly_t* pa, const ap_generator0_array_t* array);

#endif
