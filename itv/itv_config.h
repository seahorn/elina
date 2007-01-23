
#ifndef _ITV_CONFIG_H_
#define _ITV_CONFIG_H_

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
