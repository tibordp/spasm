template<typename T>
void every_sib(T callback) {
	for (auto j : cpu_registers::all())
	for (auto k : cpu_registers::all())
	for (int multiplier = 1; multiplier <= 8; multiplier *= 2)
	for (int32_t displacement : { 0, 0x15, 0x1515 })
	{
		sib_specifier sib({ multiplier, j, k, displacement });
		callback(sib);
	}
}

template<typename T>
void every_sib_reg(T callback) {
	for (auto i : cpu_registers::all())
	for (auto j : cpu_registers::all())
	for (auto k : cpu_registers::all())
	for (int multiplier = 1; multiplier <= 8; multiplier *= 2)
	for (int32_t displacement : { 0, 0x15, 0x1515 })
	{
		sib_specifier sib({ multiplier, j, k, displacement });
		callback(i, sib);
	}
}


#include "tests/mov_test.h"
#include "tests/executable_test.h"

#ifndef ATTACH_TEST
#define XATTACH_TEST(name) name##_test 
#define ATTACH_TEST(name) XATTACH_TEST(name) 
#define XSTRINGIFY(name) #name
#define STRINGIFY(name) XSTRINGIFY(name)
#endif

#define T_COMMON add
#include "tests/add_etc_test.h"
#undef T_COMMON

#define T_COMMON or_
#include "tests/add_etc_test.h"
#undef T_COMMON

#define T_COMMON adc
#include "tests/add_etc_test.h"
#undef T_COMMON

#define T_COMMON sbb
#include "tests/add_etc_test.h"
#undef T_COMMON

#define T_COMMON and_
#include "tests/add_etc_test.h"
#undef T_COMMON

#define T_COMMON sub
#include "tests/add_etc_test.h"
#undef T_COMMON

#define T_COMMON xor_
#include "tests/add_etc_test.h"
#undef T_COMMON

#define T_COMMON cmp
#include "tests/add_etc_test.h"
#undef T_COMMON

// -------

#define T_COMMON rol
#include "tests/sal_etc_test.h"
#undef T_COMMON

#define T_COMMON ror
#include "tests/sal_etc_test.h"
#undef T_COMMON

#define T_COMMON rcl
#include "tests/sal_etc_test.h"
#undef T_COMMON

#define T_COMMON rcr
#include "tests/sal_etc_test.h"
#undef T_COMMON

#define T_COMMON shl
#include "tests/sal_etc_test.h"
#undef T_COMMON

#define T_COMMON sal
#include "tests/sal_etc_test.h"
#undef T_COMMON

#define T_COMMON shr
#include "tests/sal_etc_test.h"
#undef T_COMMON

#define T_COMMON sar
#include "tests/sal_etc_test.h"
#undef T_COMMON

// ---------

#define T_COMMON inc
#include "tests/inc_dec_test.h"
#undef T_COMMON

#define T_COMMON dec
#include "tests/inc_dec_test.h"
#undef T_COMMON

#include "tests/jcc_test.h"
#include "tests/various_test.h"
