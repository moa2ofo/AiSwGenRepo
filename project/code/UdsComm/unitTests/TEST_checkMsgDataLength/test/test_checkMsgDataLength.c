#include "checkMsgDataLength.h"
#include "unity.h"
#include <string.h>

void setUp(void) {}

void tearDown(void) {}

/* Test: Valid minimum boundary - dataLength = 1 should return E_OK */
void test_checkMsgDataLength_ValidMinimumBoundary(void) {
  Std_ReturnType result;

  checkMsgDataLength(1u, &result);

  TEST_ASSERT_EQUAL(E_OK, result);
}

/* Test: Valid maximum boundary - dataLength = 32 should return E_OK */
void test_checkMsgDataLength_ValidMaximumBoundary(void) {
  Std_ReturnType result;

  checkMsgDataLength(32u, &result);

  TEST_ASSERT_EQUAL(E_OK, result);
}

/* Test: Valid middle value - dataLength = 16 should return E_OK */
void test_checkMsgDataLength_ValidMiddleValue(void) {
  Std_ReturnType result;

  checkMsgDataLength(16u, &result);

  TEST_ASSERT_EQUAL(E_OK, result);
}

/* Test: Invalid - dataLength = 0 should return E_NOT_OK */
void test_checkMsgDataLength_InvalidZero(void) {
  Std_ReturnType result;

  checkMsgDataLength(0u, &result);

  TEST_ASSERT_EQUAL(E_NOT_OK, result);
}

/* Test: Invalid - dataLength > 32 should return E_NOT_OK */
void test_checkMsgDataLength_InvalidAboveMaximum(void) {
  Std_ReturnType result;

  checkMsgDataLength(33u, &result);

  TEST_ASSERT_EQUAL(E_NOT_OK, result);
}

/* Test: Invalid - dataLength with large value should return E_NOT_OK */
void test_checkMsgDataLength_InvalidLargeValue(void) {
  Std_ReturnType result;

  checkMsgDataLength(1000u, &result);

  TEST_ASSERT_EQUAL(E_NOT_OK, result);
}

/* Test: Invalid - dataLength at maximum uint16 should return E_NOT_OK */
void test_checkMsgDataLength_InvalidMaxUint16(void) {
  Std_ReturnType result;

  checkMsgDataLength(0xFFFFu, &result);

  TEST_ASSERT_EQUAL(E_NOT_OK, result);
}