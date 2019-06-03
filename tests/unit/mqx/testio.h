#ifndef _TEST_IO_H_
#define _TEST_IO_H_

#include <mqx.h>
#include <bsp.h>

/* A standard way for the embedded test apps to communicate test status back to
 * a script, by way of simple parse-able output through the debugger
 * semihosting console. */

#define OUTPUT_TEST_SUITE_BEGIN() printf("[TestSuiteBegin]\n")

#define OUTPUT_TEST_SUITE_END() printf("[TestSuiteEnd]\n")

#define OUTPUT_TEST_SUITE_TIMEOUT() printf("[TestSuiteTimeout]\n")

#define OUTPUT_TEST_MODULE_BEGIN(name) printf("[TestModuleBegin:" name "]\n")

#define OUTPUT_TEST_MODULE_END(name, did_pass) printf("[TestModuleEnd:" name ":%s]\n", did_pass ? "PASS" : "FAIL")

#define OUTPUT_TEST_BEGIN(name) printf("[TestBegin:" name "]\n")

#define OUTPUT_TEST_RESULT(did_pass) ((did_pass) ? printf("[TestResult:PASS]\n") : printf("[TestResult:FAIL]\n"))

#endif /* _TEST_IO_H_ */
