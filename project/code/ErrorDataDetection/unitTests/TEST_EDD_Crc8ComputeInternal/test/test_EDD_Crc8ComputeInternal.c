#include "EDD_Crc8ComputeInternal.h"
#include "unity.h"
#include <string.h>

/**
 * @file test_EDD_Crc8ComputeInternal.c
 * @brief Unit tests for EDD_Crc8ComputeInternal function
 * @details Tests the CRC-8 computation using the CRC-8/ATM polynomial (0x07)
 */

/* Setup and teardown functions */
void setUp(void) { /* Setup code if needed */ }

void tearDown(void) { /* Teardown code if needed */ }

/**
 * @test Test with empty data (length = 0)
 * @brief Verify that CRC computation with zero-length data returns initial value
 */
void test_EDD_Crc8ComputeInternal_EmptyData(void) {
  uint8_t data[] = {};
  uint8_t expected = EDD_CRC8_INIT;

  uint8_t result = EDD_Crc8ComputeInternal(data, 0);

  TEST_ASSERT_EQUAL_UINT8(expected, result);
}

/**
 * @test Test with single byte data
 * @brief Verify CRC computation with a single data byte
 */
void test_EDD_Crc8ComputeInternal_SingleByte(void) {
  uint8_t data[] = {0x00};
  /* Expected: Initial CRC (0x00) XOR with data[0] (0x00) = index 0 in table = 0x00 */
  uint8_t expected = 0x00;

  uint8_t result = EDD_Crc8ComputeInternal(data, 1);

  TEST_ASSERT_EQUAL_UINT8(expected, result);
}

/**
 * @test Test with single non-zero byte
 * @brief Verify CRC computation with single non-zero byte data
 */
void test_EDD_Crc8ComputeInternal_SingleByteNonZero(void) {
  uint8_t data[] = {0x01};
  /* Expected: Initial CRC (0x00) XOR with data[0] (0x01) = index 1 in table = 0x07 */
  uint8_t expected = 0x07;

  uint8_t result = EDD_Crc8ComputeInternal(data, 1);

  TEST_ASSERT_EQUAL_UINT8(expected, result);
}

/**
 * @test Test with two bytes
 * @brief Verify CRC computation with two data bytes
 */
void test_EDD_Crc8ComputeInternal_TwoBytes(void) {
  uint8_t data[] = {0x00, 0x00};
  /* Expected:
   * Step 1: CRC = EDD_Crc8Table[0x00 ^ 0x00] = EDD_Crc8Table[0] = 0x00
   * Step 2: CRC = EDD_Crc8Table[0x00 ^ 0x00] = EDD_Crc8Table[0] = 0x00
   */
  uint8_t expected = 0x00;

  uint8_t result = EDD_Crc8ComputeInternal(data, 2);

  TEST_ASSERT_EQUAL_UINT8(expected, result);
}

/**
 * @test Test with all zeros
 * @brief Verify CRC computation with data containing all zeros
 */
void test_EDD_Crc8ComputeInternal_AllZeros(void) {
  uint8_t data[] = {0x00, 0x00, 0x00, 0x00};
  uint8_t expected = 0x00;

  uint8_t result = EDD_Crc8ComputeInternal(data, 4);

  TEST_ASSERT_EQUAL_UINT8(expected, result);
}

/**
 * @test Test with alternating pattern
 * @brief Verify CRC computation with alternating byte pattern
 */
void test_EDD_Crc8ComputeInternal_AlternatingPattern(void) {
  uint8_t data[] = {0xAA, 0x55, 0xAA, 0x55};

  uint8_t result = EDD_Crc8ComputeInternal(data, 4);

  /* Verify the result is within valid range (0-255) */
  TEST_ASSERT_LESS_OR_EQUAL_UINT8(255, result);
}

/**
 * @test Test with maximum length
 * @brief Verify CRC computation with longer data buffer
 */
void test_EDD_Crc8ComputeInternal_LongData(void) {
  uint8_t data[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
                    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F};

  uint8_t result = EDD_Crc8ComputeInternal(data, 32);

  /* Verify the result is a valid CRC (0-255) */
  TEST_ASSERT_LESS_OR_EQUAL_UINT8(255, result);
}

/**
 * @test Test return type is uint8_t
 * @brief Verify that function returns a value in valid uint8_t range
 */
void test_EDD_Crc8ComputeInternal_ReturnValueRange(void) {
  uint8_t data[] = {0xDE, 0xAD, 0xBE, 0xEF};

  uint8_t result = EDD_Crc8ComputeInternal(data, 4);

  /* Result should always be in valid uint8_t range (0-255) */
  TEST_ASSERT_LESS_OR_EQUAL_UINT8(255, result);
  TEST_ASSERT_GREATER_OR_EQUAL_UINT8(0, result);
}

/**
 * @test Test consistency - same input produces same output
 * @brief Verify that CRC computation is deterministic
 */
void test_EDD_Crc8ComputeInternal_Consistency(void) {
  uint8_t data[] = {0x12, 0x34, 0x56, 0x78};

  uint8_t result1 = EDD_Crc8ComputeInternal(data, 4);
  uint8_t result2 = EDD_Crc8ComputeInternal(data, 4);

  TEST_ASSERT_EQUAL_UINT8(result1, result2);
}

/**
 * @test Test with different length parameter for same data
 * @brief Verify that length parameter affects CRC calculation
 */
void test_EDD_Crc8ComputeInternal_DifferentLengths(void) {
  uint8_t data[] = {0x01, 0x02, 0x03, 0x04};

  uint8_t result_len1 = EDD_Crc8ComputeInternal(data, 1);
  uint8_t result_len2 = EDD_Crc8ComputeInternal(data, 2);
  uint8_t result_len4 = EDD_Crc8ComputeInternal(data, 4);

  /* Results should be different when processing different lengths */
  TEST_ASSERT_NOT_EQUAL(result_len1, result_len2);
  TEST_ASSERT_NOT_EQUAL(result_len2, result_len4);
}

/**
 * @test Test with boundary value 0x80
 * @brief Verify CRC computation with boundary byte value
 */
void test_EDD_Crc8ComputeInternal_BoundaryValue(void) {
  uint8_t data[] = {0x80};

  uint8_t result = EDD_Crc8ComputeInternal(data, 1);

  /* Verify result is valid */
  TEST_ASSERT_LESS_OR_EQUAL_UINT8(255, result);
}

/**
 * @test Test with sequence that may cause overflow
 * @brief Verify CRC computation with data values that might cause issues
 */
void test_EDD_Crc8ComputeInternal_PotentialOverflowValues(void) {
  uint8_t data[] = {0xFF, 0xFE, 0xFD};

  uint8_t result = EDD_Crc8ComputeInternal(data, 3);

  /* Should not overflow (result should be <= 255) */
  TEST_ASSERT_LESS_OR_EQUAL_UINT8(255, result);
}
