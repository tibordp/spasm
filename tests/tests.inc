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

// -------

// Test is special
// #define T_COMMON test
// #include "tests/test_etc_test.h"
// #undef T_COMMON

#define T_COMMON not_
#include "tests/test_etc_test.h"
#undef T_COMMON

#define T_COMMON neg
#include "tests/test_etc_test.h"
#undef T_COMMON

#define T_COMMON mul
#include "tests/test_etc_test.h"
#undef T_COMMON

#define T_COMMON imul
#include "tests/test_etc_test.h"
#undef T_COMMON

#define T_COMMON div
#include "tests/test_etc_test.h"
#undef T_COMMON

#define T_COMMON idiv
#include "tests/test_etc_test.h"
#undef T_COMMON

// ---------

#define T_COMMON addpd
#include "tests/sqrtps_etc_test.h"
#undef T_COMMON

#define T_COMMON addps
#include "tests/sqrtps_etc_test.h"
#undef T_COMMON

#define T_COMMON addsd
#include "tests/sqrtps_etc_test.h"
#undef T_COMMON

#define T_COMMON addss
#include "tests/sqrtps_etc_test.h"
#undef T_COMMON

#define T_COMMON addsubpd
#include "tests/sqrtps_etc_test.h"
#undef T_COMMON

#define T_COMMON addsubps
#include "tests/sqrtps_etc_test.h"
#undef T_COMMON

#define T_COMMON andnpd
#include "tests/sqrtps_etc_test.h"
#undef T_COMMON

#define T_COMMON andnps
#include "tests/sqrtps_etc_test.h"
#undef T_COMMON

#define T_COMMON andpd
#include "tests/sqrtps_etc_test.h"
#undef T_COMMON

#define T_COMMON andps
#include "tests/sqrtps_etc_test.h"
#undef T_COMMON

#define T_COMMON cvtdq2pd
#include "tests/sqrtps_etc_test.h"
#undef T_COMMON

#define T_COMMON cvtdq2ps
#include "tests/sqrtps_etc_test.h"
#undef T_COMMON

#define T_COMMON cvtpd2dq
#include "tests/sqrtps_etc_test.h"
#undef T_COMMON

#define T_COMMON cvtpd2ps
#include "tests/sqrtps_etc_test.h"
#undef T_COMMON

#define T_COMMON cvtps2dq
#include "tests/sqrtps_etc_test.h"
#undef T_COMMON

#define T_COMMON cvtps2pd
#include "tests/sqrtps_etc_test.h"
#undef T_COMMON

#define T_COMMON cvtsd2ss
#include "tests/sqrtps_etc_test.h"
#undef T_COMMON

#define T_COMMON cvtss2sd
#include "tests/sqrtps_etc_test.h"
#undef T_COMMON

#define T_COMMON cvttpd2dq
#include "tests/sqrtps_etc_test.h"
#undef T_COMMON

#define T_COMMON cvttps2dq
#include "tests/sqrtps_etc_test.h"
#undef T_COMMON

#define T_COMMON divpd
#include "tests/sqrtps_etc_test.h"
#undef T_COMMON

#define T_COMMON divps
#include "tests/sqrtps_etc_test.h"
#undef T_COMMON

#define T_COMMON divsd
#include "tests/sqrtps_etc_test.h"
#undef T_COMMON

#define T_COMMON divss
#include "tests/sqrtps_etc_test.h"
#undef T_COMMON

#define T_COMMON haddpd
#include "tests/sqrtps_etc_test.h"
#undef T_COMMON

#define T_COMMON haddps
#include "tests/sqrtps_etc_test.h"
#undef T_COMMON

#define T_COMMON hsubpd
#include "tests/sqrtps_etc_test.h"
#undef T_COMMON

#define T_COMMON hsubps
#include "tests/sqrtps_etc_test.h"
#undef T_COMMON

#define T_COMMON maxpd
#include "tests/sqrtps_etc_test.h"
#undef T_COMMON

#define T_COMMON maxps
#include "tests/sqrtps_etc_test.h"
#undef T_COMMON

#define T_COMMON maxsd
#include "tests/sqrtps_etc_test.h"
#undef T_COMMON

#define T_COMMON maxss
#include "tests/sqrtps_etc_test.h"
#undef T_COMMON

#define T_COMMON minpd
#include "tests/sqrtps_etc_test.h"
#undef T_COMMON

#define T_COMMON minps
#include "tests/sqrtps_etc_test.h"
#undef T_COMMON

#define T_COMMON minsd
#include "tests/sqrtps_etc_test.h"
#undef T_COMMON

#define T_COMMON minss
#include "tests/sqrtps_etc_test.h"
#undef T_COMMON

#define T_COMMON mulpd
#include "tests/sqrtps_etc_test.h"
#undef T_COMMON

#define T_COMMON mulps
#include "tests/sqrtps_etc_test.h"
#undef T_COMMON

#define T_COMMON mulsd
#include "tests/sqrtps_etc_test.h"
#undef T_COMMON

#define T_COMMON mulss
#include "tests/sqrtps_etc_test.h"
#undef T_COMMON

#define T_COMMON orpd
#include "tests/sqrtps_etc_test.h"
#undef T_COMMON

#define T_COMMON orps
#include "tests/sqrtps_etc_test.h"
#undef T_COMMON

#define T_COMMON packssdw
#include "tests/sqrtps_etc_test.h"
#undef T_COMMON

#define T_COMMON packsswb
#include "tests/sqrtps_etc_test.h"
#undef T_COMMON

#define T_COMMON packuswb
#include "tests/sqrtps_etc_test.h"
#undef T_COMMON

#define T_COMMON paddb
#include "tests/sqrtps_etc_test.h"
#undef T_COMMON

#define T_COMMON paddd
#include "tests/sqrtps_etc_test.h"
#undef T_COMMON

#define T_COMMON paddq
#include "tests/sqrtps_etc_test.h"
#undef T_COMMON

#define T_COMMON paddsb
#include "tests/sqrtps_etc_test.h"
#undef T_COMMON

#define T_COMMON paddsw
#include "tests/sqrtps_etc_test.h"
#undef T_COMMON

#define T_COMMON paddusb
#include "tests/sqrtps_etc_test.h"
#undef T_COMMON

#define T_COMMON paddusw
#include "tests/sqrtps_etc_test.h"
#undef T_COMMON

#define T_COMMON paddw
#include "tests/sqrtps_etc_test.h"
#undef T_COMMON

#define T_COMMON pand
#include "tests/sqrtps_etc_test.h"
#undef T_COMMON

#define T_COMMON pandn
#include "tests/sqrtps_etc_test.h"
#undef T_COMMON

#define T_COMMON pavgb
#include "tests/sqrtps_etc_test.h"
#undef T_COMMON

#define T_COMMON pavgw
#include "tests/sqrtps_etc_test.h"
#undef T_COMMON

#define T_COMMON pcmpgtb
#include "tests/sqrtps_etc_test.h"
#undef T_COMMON

#define T_COMMON pcmpgtd
#include "tests/sqrtps_etc_test.h"
#undef T_COMMON

#define T_COMMON pcmpgtw
#include "tests/sqrtps_etc_test.h"
#undef T_COMMON

#define T_COMMON pmaddwd
#include "tests/sqrtps_etc_test.h"
#undef T_COMMON

#define T_COMMON pmaxsw
#include "tests/sqrtps_etc_test.h"
#undef T_COMMON

#define T_COMMON pmaxub
#include "tests/sqrtps_etc_test.h"
#undef T_COMMON

#define T_COMMON pminsw
#include "tests/sqrtps_etc_test.h"
#undef T_COMMON

#define T_COMMON pminub
#include "tests/sqrtps_etc_test.h"
#undef T_COMMON

#define T_COMMON pmulhuw
#include "tests/sqrtps_etc_test.h"
#undef T_COMMON

#define T_COMMON pmulhw
#include "tests/sqrtps_etc_test.h"
#undef T_COMMON

#define T_COMMON pmullw
#include "tests/sqrtps_etc_test.h"
#undef T_COMMON

#define T_COMMON pmuludq
#include "tests/sqrtps_etc_test.h"
#undef T_COMMON

#define T_COMMON por
#include "tests/sqrtps_etc_test.h"
#undef T_COMMON

#define T_COMMON psadbw
#include "tests/sqrtps_etc_test.h"
#undef T_COMMON

#define T_COMMON pslld
#include "tests/sqrtps_etc_test.h"
#undef T_COMMON

#define T_COMMON psllq
#include "tests/sqrtps_etc_test.h"
#undef T_COMMON

#define T_COMMON psllw
#include "tests/sqrtps_etc_test.h"
#undef T_COMMON

#define T_COMMON psrad
#include "tests/sqrtps_etc_test.h"
#undef T_COMMON

#define T_COMMON psraw
#include "tests/sqrtps_etc_test.h"
#undef T_COMMON

#define T_COMMON psrld
#include "tests/sqrtps_etc_test.h"
#undef T_COMMON

#define T_COMMON psrlq
#include "tests/sqrtps_etc_test.h"
#undef T_COMMON

#define T_COMMON psrlw
#include "tests/sqrtps_etc_test.h"
#undef T_COMMON

#define T_COMMON psubb
#include "tests/sqrtps_etc_test.h"
#undef T_COMMON

#define T_COMMON psubd
#include "tests/sqrtps_etc_test.h"
#undef T_COMMON

#define T_COMMON psubq
#include "tests/sqrtps_etc_test.h"
#undef T_COMMON

#define T_COMMON psubsb
#include "tests/sqrtps_etc_test.h"
#undef T_COMMON

#define T_COMMON psubsw
#include "tests/sqrtps_etc_test.h"
#undef T_COMMON

#define T_COMMON psubusb
#include "tests/sqrtps_etc_test.h"
#undef T_COMMON

#define T_COMMON psubusw
#include "tests/sqrtps_etc_test.h"
#undef T_COMMON

#define T_COMMON psubw
#include "tests/sqrtps_etc_test.h"
#undef T_COMMON

#define T_COMMON punpckhbw
#include "tests/sqrtps_etc_test.h"
#undef T_COMMON

#define T_COMMON punpckhdq
#include "tests/sqrtps_etc_test.h"
#undef T_COMMON

#define T_COMMON punpckhqdq
#include "tests/sqrtps_etc_test.h"
#undef T_COMMON

#define T_COMMON punpckhwd
#include "tests/sqrtps_etc_test.h"
#undef T_COMMON

#define T_COMMON punpcklbw
#include "tests/sqrtps_etc_test.h"
#undef T_COMMON

#define T_COMMON punpckldq
#include "tests/sqrtps_etc_test.h"
#undef T_COMMON

#define T_COMMON punpcklqdq
#include "tests/sqrtps_etc_test.h"
#undef T_COMMON

#define T_COMMON punpcklwd
#include "tests/sqrtps_etc_test.h"
#undef T_COMMON

#define T_COMMON pxor
#include "tests/sqrtps_etc_test.h"
#undef T_COMMON

#define T_COMMON rcpps
#include "tests/sqrtps_etc_test.h"
#undef T_COMMON

#define T_COMMON rcpss
#include "tests/sqrtps_etc_test.h"
#undef T_COMMON

#define T_COMMON rsqrtps
#include "tests/sqrtps_etc_test.h"
#undef T_COMMON

#define T_COMMON rsqrtss
#include "tests/sqrtps_etc_test.h"
#undef T_COMMON

#define T_COMMON sqrtpd
#include "tests/sqrtps_etc_test.h"
#undef T_COMMON

#define T_COMMON sqrtps
#include "tests/sqrtps_etc_test.h"
#undef T_COMMON

#define T_COMMON sqrtsd
#include "tests/sqrtps_etc_test.h"
#undef T_COMMON

#define T_COMMON sqrtss
#include "tests/sqrtps_etc_test.h"
#undef T_COMMON

#define T_COMMON subpd
#include "tests/sqrtps_etc_test.h"
#undef T_COMMON

#define T_COMMON subps
#include "tests/sqrtps_etc_test.h"
#undef T_COMMON

#define T_COMMON subsd
#include "tests/sqrtps_etc_test.h"
#undef T_COMMON

#define T_COMMON subss
#include "tests/sqrtps_etc_test.h"
#undef T_COMMON

#define T_COMMON xorpd
#include "tests/sqrtps_etc_test.h"
#undef T_COMMON

#define T_COMMON xorps
#include "tests/sqrtps_etc_test.h"
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
#include "tests/sse_test.h"