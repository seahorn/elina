
#ifndef _BOX_CONFIG_H_
#define _BOX_CONFIG_H_

#include "num.h"
#include "bound.h"
#include "ap_global0.h"

#ifdef __cplusplus
#define HAS_BOOL
extern "C" {
#endif

#ifndef HAS_BOOL
#define HAS_BOOL

typedef enum bool {
  false=0,
  true=1
} bool;
#endif

#ifdef __cplusplus
}
#endif

#endif
