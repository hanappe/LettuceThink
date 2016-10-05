#ifndef __wu_h__
#define __wu_h__

#ifdef __cplusplus
extern "C" {
#endif

typedef int (*wu_callback_t) (double energy, void* data);

extern int wu_start(void);
extern int wu_stop(void);
extern int wu_set_listener(wu_callback_t callback, void* data);

#ifdef __cplusplus
}
#endif

#endif
