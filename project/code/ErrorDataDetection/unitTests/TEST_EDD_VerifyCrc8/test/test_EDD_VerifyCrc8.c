#include "unity.h"
#include "EDD_VerifyCrc8.h"
#include "mock_errorDataDetection_priv.h"


/**
 * @file test_EDD_VerifyCrc8.c
 * @brief Unit tests for EDD_VerifyCrc8 function
 * @details Tests the CRC-8 verification function including error handling and CRC comparison
 */

/* Setup and teardown functions */
void setUp(void)
{
    /* Setup code if needed */
}

void tearDown(void)
{
    /* Teardown code if needed */
}


/* =============================
 *  Test Cases
 * ============================= */

/**
 * @brief Test: NULL data pointer should return EDD_NULL_PTR
 */
void test_EDD_VerifyCrc8_NullDataPointer(void)
{
    uint8_t result = 0xFF;
    EDD_ReturnType ret;

    ret = EDD_VerifyCrc8(NULL, 5, 0x12, &result);

    TEST_ASSERT_EQUAL(EDD_NULL_PTR, ret);
}


/**
 * @brief Test: NULL result_out pointer should return EDD_NULL_PTR
 */
void test_EDD_VerifyCrc8_NullResultPointer(void)
{
    uint8_t data[] = {0x01, 0x02, 0x03};
    EDD_ReturnType ret;

    ret = EDD_VerifyCrc8(data, 3, 0x12, NULL);

    TEST_ASSERT_EQUAL(EDD_NULL_PTR, ret);
}


/**
 * @brief Test: Both data and result_out pointers NULL should return EDD_NULL_PTR
 */
void test_EDD_VerifyCrc8_BothPointersNull(void)
{
    EDD_ReturnType ret;

    ret = EDD_VerifyCrc8(NULL, 5, 0x12, NULL);

    TEST_ASSERT_EQUAL(EDD_NULL_PTR, ret);
}


/**
 * @brief Test: Zero length should return EDD_INVALID_LENGTH
 */
void test_EDD_VerifyCrc8_ZeroLength(void)
{
    uint8_t data[] = {0x01};
    uint8_t result = 0xFF;
    EDD_ReturnType ret;

    ret = EDD_VerifyCrc8(data, 0, 0x12, &result);

    TEST_ASSERT_EQUAL(EDD_INVALID_LENGTH, ret);
}


/**
 * @brief Test: Valid CRC match should set result_out to 1 and return EDD_OK
 */
void test_EDD_VerifyCrc8_ValidCrcMatch(void)
{
    uint8_t data[] = {0x01, 0x02, 0x03, 0x04};
    uint8_t expected_crc = 0xA5;
    uint8_t result = 0xFF;
    EDD_ReturnType ret;

    /* Mock the CRC computation to return the expected value */
    EDD_Crc8ComputeInternal_ExpectAndReturn(data, 4, expected_crc);

    ret = EDD_VerifyCrc8(data, 4, expected_crc, &result);

    TEST_ASSERT_EQUAL(EDD_OK, ret);
    TEST_ASSERT_EQUAL(1u, result);
}


/**
 * @brief Test: Valid CRC mismatch should set result_out to 0 and return EDD_OK
 */
void test_EDD_VerifyCrc8_ValidCrcMismatch(void)
{
    uint8_t data[] = {0x01, 0x02, 0x03, 0x04};
    uint8_t expected_crc = 0xA5;
    uint8_t computed_crc = 0x3C;  /* Different from expected */
    uint8_t result = 0xFF;
    EDD_ReturnType ret;

    /* Mock the CRC computation to return a different value */
    EDD_Crc8ComputeInternal_ExpectAndReturn(data, 4, computed_crc);

    ret = EDD_VerifyCrc8(data, 4, expected_crc, &result);

    TEST_ASSERT_EQUAL(EDD_OK, ret);
    TEST_ASSERT_EQUAL(0u, result);
}


/**
 * @brief Test: Single byte data with matching CRC
 */
void test_EDD_VerifyCrc8_SingleByteMatch(void)
{
    uint8_t data[] = {0xAA};
    uint8_t expected_crc = 0x7B;
    uint8_t result = 0xFF;
    EDD_ReturnType ret;

    EDD_Crc8ComputeInternal_ExpectAndReturn(data, 1, expected_crc);

    ret = EDD_VerifyCrc8(data, 1, expected_crc, &result);

    TEST_ASSERT_EQUAL(EDD_OK, ret);
    TEST_ASSERT_EQUAL(1u, result);
}


/**
 * @brief Test: Maximum length data with matching CRC
 */
void test_EDD_VerifyCrc8_MaxLengthMatch(void)
{
    uint8_t data[255] = {0};
    uint8_t expected_crc = 0x55;
    uint8_t result = 0xFF;
    EDD_ReturnType ret;

    /* Fill data with pattern */
    for (int i = 0; i < 255; i++)
    {
        data[i] = (uint8_t)(i & 0xFF);
    }

    EDD_Crc8ComputeInternal_ExpectAndReturn(data, 255, expected_crc);

    ret = EDD_VerifyCrc8(data, 255, expected_crc, &result);

    TEST_ASSERT_EQUAL(EDD_OK, ret);
    TEST_ASSERT_EQUAL(1u, result);
}


/**
 * @brief Test: Boundary case - length of 1
 */
void test_EDD_VerifyCrc8_LengthOne(void)
{
    uint8_t data[] = {0x42};
    uint8_t expected_crc = 0xE0;
    uint8_t result = 0xFF;
    EDD_ReturnType ret;

    EDD_Crc8ComputeInternal_ExpectAndReturn(data, 1, expected_crc);

    ret = EDD_VerifyCrc8(data, 1, expected_crc, &result);

    TEST_ASSERT_EQUAL(EDD_OK, ret);
    TEST_ASSERT_EQUAL(1u, result);
}


/**
 * @brief Test: CRC value of 0x00 with matching data
 */
void test_EDD_VerifyCrc8_CrcZero(void)
{
    uint8_t data[] = {0x00, 0x00};
    uint8_t expected_crc = 0x00;
    uint8_t result = 0xFF;
    EDD_ReturnType ret;

    EDD_Crc8ComputeInternal_ExpectAndReturn(data, 2, expected_crc);

    ret = EDD_VerifyCrc8(data, 2, expected_crc, &result);

    TEST_ASSERT_EQUAL(EDD_OK, ret);
    TEST_ASSERT_EQUAL(1u, result);
}


/**
 * @brief Test: CRC value of 0xFF with matching data
 */
void test_EDD_VerifyCrc8_CrcMaxValue(void)
{
    uint8_t data[] = {0xFF, 0xFF};
    uint8_t expected_crc = 0xFF;
    uint8_t result = 0xFF;
    EDD_ReturnType ret;

    EDD_Crc8ComputeInternal_ExpectAndReturn(data, 2, expected_crc);

    ret = EDD_VerifyCrc8(data, 2, expected_crc, &result);

    TEST_ASSERT_EQUAL(EDD_OK, ret);
    TEST_ASSERT_EQUAL(1u, result);
}
