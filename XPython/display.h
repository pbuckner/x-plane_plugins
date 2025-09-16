#ifndef WINDOWS__H
#define WINDOWS__H
#ifdef __cplusplus
extern "C" {
#endif  
void resetWindows(void);
void resetDrawCallbacks(void);
void resetKeySniffCallbacks(void);
void resetHotKeyCallbacks(void);
void resetAvionicsCallbacks(void);
#ifdef __cplusplus
}
#endif
#endif
