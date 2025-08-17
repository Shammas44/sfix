#include "Integrity.SFIX.h"
#include <criterion/criterion.h>
#define T Integrity

static void setup(void) {
  /*#region*/
  /*#endregion*/
}

static void teardown(void) {
  /*#region*/
  /*#endregion*/
}

Test(T, is_valid_integer, .fini = teardown, .init = setup) {
  /*#region*/
  bool (*fn)(const char[SFIX_MAX_VALUE_LEN]) = SFIX_is_valid_integer;
  // valide values
  cr_expect_eq(fn("1"), true);
  cr_expect_eq(fn("10"), true);
  cr_expect_eq(fn("-1"), true);
  cr_expect_eq(fn("0"), true);
  // unvalid values
  cr_expect_eq(fn("0.1"), false);
  cr_expect_eq(fn("hello"), false);
  cr_expect_eq(fn("11111b"), false);
  cr_expect_eq(fn("11111.0"), false);
  cr_expect_eq(fn(NULL), false);
  /*#endregion*/
}

Test(T, is_valid_double, .fini = teardown, .init = setup) {
  /*#region*/
  bool (*fn)(const char[SFIX_MAX_VALUE_LEN]) = SFIX_is_valid_double;
  // valide values
  cr_expect_eq(fn("0.1"), true);
  cr_expect_eq(fn("-10.1"), true);
  cr_expect_eq(fn("100.234"), true);
  cr_expect_eq(fn("100.000"), true);
  cr_expect_eq(fn("1"), true);
  cr_expect_eq(fn("0"), true);
  cr_expect_eq(fn("-1"), true);
  // unvalid values
  cr_expect_eq(fn("hello"), false);
  cr_expect_eq(fn("10,9"), false);
  cr_expect_eq(fn("11111b"), false);
  cr_expect_eq(fn(NULL), false);
  /*#endregion*/
}
