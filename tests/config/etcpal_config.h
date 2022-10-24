#ifdef __cplusplus
extern "C" {
#endif

bool EtcPalTestingAssertVerify(bool condition, const char* expr, const char* file, const char* func, unsigned int line);

#ifdef __cplusplus
}
#endif

#define ETCPAL_ASSERT_VERIFY(expr) EtcPalTestingAssertVerify(expr, #expr, __FILE__, __func__, __LINE__)
#define ETCPAL_LOGGING_ENABLED     0
