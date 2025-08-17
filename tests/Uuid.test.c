#define UNIT_TESTING TRUE
#include "Uuid.SFIX.h"
#include "Integrity.SFIX.h"
#include <criterion/criterion.h>
#define T SFIX_Uuid

static void setup(void) {
  /*#region*/
  // factory = SFIX_orderFactory_constructor("USD");
  /*#endregion*/
}

static void teardown(void) {
  /*#region*/
  // memset(msg, 0, 2048);
  /*#endregion*/
}

Test(T, default_constructor, .fini = teardown, .init = setup) {
  /*#region*/
  T id = SFIX_uuid_constructor();
  char value[SFIX_UUID_LENGTH] = {0};
  id.get(id,value);
  cr_expect_eq(__match_string(value, "[a-z0-9-]+"), true);
  cr_expect_eq(strlen(value), SFIX_UUID_LENGTH -1);
  /*#endregion*/
}

Test(T, by_string_constructor, .fini = teardown, .init = setup) {
  /*#region*/
  char pattern[SFIX_UUID_LENGTH -1] = "aaaa-bbbb-cccc-dddd-eeee-ffffff";
  T id = SFIX_uuid_by_string_constructor(pattern);
  char value[SFIX_UUID_LENGTH] = {0};
  id.get(id,value);
  cr_expect_eq(__match_string(value, "[a-z0-9-]+"), true);
  cr_expect_eq(strlen(value), SFIX_UUID_LENGTH -1);
  /*#endregion*/
}
