#ifndef WINDOWS__H
#define WINDOWS__H
void resetWindows(void);
void resetDrawCallbacks(void);
void resetKeySniffCallbacks(void);
void resetHotKeyCallbacks(void);
void resetAvionicsCallbacks(void);
#include <unordered_map>
extern std::unordered_map<void*, PyObject*> windowIDCapsules;
#endif
