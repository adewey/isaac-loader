#ifndef IMPORTS
#define GLOBAL EXTERN_C __declspec(dllexport)
#else
#define GLOBAL EXTERN_C __declspec(dllimport)
#endif

GLOBAL bool gbAttached;
