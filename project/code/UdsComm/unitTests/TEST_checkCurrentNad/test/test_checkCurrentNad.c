#include "checkCurrentNad.h"
#include "unity.h"

void setUp(void) { /* Reset all mocks before each test */ }

void tearDown(void) { /* Verify all mock expectations were met */ }

/*
 * Test: checkCurrentNad_WithInvalidNad_0x7F_ReturnsNotOk
 * Description: Verify that when currentNad is 0x7F (reserved), E_NOT_OK is returned
 */
void test_checkCurrentNad_WithInvalidNad_0x7F_ReturnsNotOk(void) {
  Std_ReturnType result = E_OK;

  checkCurrentNad(0x7F, &result);

  TEST_ASSERT_EQUAL(E_NOT_OK, result);
}

/*
 * Test: checkCurrentNad_WithInvalidNad_0x7E_ReturnsNotOk
 * Description: Verify that when currentNad is 0x7E (reserved), E_NOT_OK is returned
 */
void test_checkCurrentNad_WithInvalidNad_0x7E_ReturnsNotOk(void) {
  Std_ReturnType result = E_OK;

  checkCurrentNad(0x7E, &result);

  TEST_ASSERT_EQUAL(E_NOT_OK, result);
}

/*
 * Test: checkCurrentNad_WithValidNad_0x00_ReturnsOk
 * Description: Verify that when currentNad is 0x00 (valid), E_OK is returned
 */
void test_checkCurrentNad_WithValidNad_0x00_ReturnsOk(void) {
  Std_ReturnType result = E_NOT_OK;

  checkCurrentNad(0x00, &result);

  TEST_ASSERT_EQUAL(E_OK, result);
}

/*
 * Test: checkCurrentNad_WithValidNad_0x01_ReturnsOk
 * Description: Verify that when currentNad is 0x01 (valid), E_OK is returned
 */
void test_checkCurrentNad_WithValidNad_0x01_ReturnsOk(void) {
  Std_ReturnType result = E_NOT_OK;

  checkCurrentNad(0x01, &result);

  TEST_ASSERT_EQUAL(E_OK, result);
}

/*
 * Test: checkCurrentNad_WithValidNad_0x7D_ReturnsOk
 * Description: Verify that when currentNad is 0x7D (just before reserved range), E_OK is returned
 */
void test_checkCurrentNad_WithValidNad_0x7D_ReturnsOk(void) {
  Std_ReturnType result = E_OK;

  checkCurrentNad(0x7D, &result);

  TEST_ASSERT_EQUAL(E_OK, result);
}

/*
 * Test: checkCurrentNad_WithValidNad_0xFF_ReturnsOk
 * Description: Verify that when currentNad is 0xFF (max value), E_OK is returned
 */
void test_checkCurrentNad_WithValidNad_0xFF_ReturnsOk(void) {
  Std_ReturnType result = E_NOT_OK;

  checkCurrentNad(0xFF, &result);

  TEST_ASSERT_EQUAL(E_OK, result);
}

/*
 * Test: checkCurrentNad_WithValidNad_0x80_ReturnsOk
 * Description: Verify that when currentNad is 0x80 (after reserved range), E_OK is returned
 */
void test_checkCurrentNad_WithValidNad_0x80_ReturnsOk(void) {
  Std_ReturnType result = E_NOT_OK;

  checkCurrentNad(0x80, &result);

  TEST_ASSERT_EQUAL(E_OK, result);
}
