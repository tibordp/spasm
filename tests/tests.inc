#include "tests/mov_test.h"
#include "tests/executable_test.h"

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