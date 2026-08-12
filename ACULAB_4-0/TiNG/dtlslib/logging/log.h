void AcuDTLSLog(const char* frmt, ...);

#define LogModuleError(frmt, ...) AcuDTLSLog(frmt,__VA_ARGS__)
#define LogModuleWarning(frmt, ...) AcuDTLSLog(frmt,__VA_ARGS__)
#define LogModuleDebugMax(frmt, ...) AcuDTLSLog(frmt,__VA_ARGS__)
#define LogModuleInfo(frmt, ...) AcuDTLSLog(frmt,__VA_ARGS__)

