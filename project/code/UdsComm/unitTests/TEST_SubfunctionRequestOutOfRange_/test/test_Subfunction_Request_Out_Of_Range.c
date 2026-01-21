#include "Subfunction_Request_Out_Of_Range.h"
#include "unity.h"

void setUp(void) { /* Reset all mocks before each test */ }

void tearDown(void) { /* Verify all mock expectations were met */ }

/**
 * @test Test when errCode_pu8 is NULL
 * @details Verifies that the function returns E_NOT_OK when errCode_pu8 is NULL
 * and does not cause any issues.
 */
void test_Subfunction_Request_Out_Of_Range_WithNullErrCode(void) {
  uint8 output = 0xFF;
  uint8 size = 0x10;
  Std_ReturnType result;

  result = Subfunction_Request_Out_Of_Range(&output, &size, NULL);

  TEST_ASSERT_EQUAL(E_NOT_OK, result);
}

/**
 * @test Test when errCode_pu8 is valid
 * @details Verifies that the function sets errCode to 0x12 and returns E_NOT_OK
 * when a valid errCode_pu8 pointer is provided.
 */
void test_Subfunction_Request_Out_Of_Range_WithValidErrCode(void) {
  uint8 output = 0xFF;
  uint8 size = 0x10;
  uint8 errCode = 0x00;
  Std_ReturnType result;

  result = Subfunction_Request_Out_Of_Range(&output, &size, &errCode);

  TEST_ASSERT_EQUAL(E_NOT_OK, result);
  TEST_ASSERT_EQUAL(0x12, errCode);
}

/**
 * @test Test when output_pu8 is NULL
 * @details Verifies that the function works correctly even when output_pu8 is NULL,
 * since it is unused in the function.
 */
void test_Subfunction_Request_Out_Of_Range_WithNullOutput(void) {
  uint8 size = 0x10;
  uint8 errCode = 0x00;
  Std_ReturnType result;

  result = Subfunction_Request_Out_Of_Range(NULL, &size, &errCode);

  TEST_ASSERT_EQUAL(E_NOT_OK, result);
  TEST_ASSERT_EQUAL(0x12, errCode);
}

/**
 * @test Test when size_pu8 is NULL
 * @details Verifies that the function works correctly even when size_pu8 is NULL,
 * since it is unused in the function.
 */
void test_Subfunction_Request_Out_Of_Range_WithNullSize(void) {
  uint8 output = 0xFF;
  uint8 errCode = 0x00;
  Std_ReturnType result;

  result = Subfunction_Request_Out_Of_Range(&output, NULL, &errCode);

  TEST_ASSERT_EQUAL(E_NOT_OK, result);
  TEST_ASSERT_EQUAL(0x12, errCode);
}

/**
 * @test Test with all pointers valid
 * @details Verifies that the function behaves correctly with all valid pointers.
 */
void test_Subfunction_Request_Out_Of_Range_WithAllValidPointers(void) {
  uint8 output = 0x00;
  uint8 size = 0x05;
  uint8 errCode = 0xFF;
  Std_ReturnType result;

  result = Subfunction_Request_Out_Of_Range(&output, &size, &errCode);

  TEST_ASSERT_EQUAL(E_NOT_OK, result);
  TEST_ASSERT_EQUAL(0x12, errCode);
}

/**
 * @test Test that output and size remain unchanged
 * @details Verifies that output_pu8 and size_pu8 values are not modified
 * by the function, as they are unused.
 */
void test_Subfunction_Request_Out_Of_Range_OutputAndSizeUnchanged(void) {
  uint8 output = 0xAA;
  uint8 size = 0xBB;
  uint8 errCode = 0x00;
  Std_ReturnType result;

  result = Subfunction_Request_Out_Of_Range(&output, &size, &errCode);

  TEST_ASSERT_EQUAL(E_NOT_OK, result);
  TEST_ASSERT_EQUAL(0xAA, output);
  TEST_ASSERT_EQUAL(0xBB, size);
  TEST_ASSERT_EQUAL(0x12, errCode);
}
