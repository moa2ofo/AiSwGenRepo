// CEEDLING NOTICE: This generated file only to be consumed for test runner creation

#include "utExecutionAndResults/utUnderTest/build/vendor/unity/src/unity.h"
#include "utExecutionAndResults/utUnderTest/src/EDD_VerifyCrc8.h"
#include "mock_errorDataDetection_priv.h"

void setUp(void)
{
}

void tearDown(void)
{
}
void test_EDD_VerifyCrc8_NullDataPointer(void)
{
    uint8_t result = 0xFF;
    EDD_ReturnType ret;

    ret = EDD_VerifyCrc8(
                        ((void *)0)
                            , 5, 0x12, &result);

    UnityAssertEqualNumber((UNITY_INT)((EDD_NULL_PTR)), (UNITY_INT)((ret)), (
   ((void *)0)
   ), (UNITY_UINT)(38), UNITY_DISPLAY_STYLE_INT);
}

void test_EDD_VerifyCrc8_NullResultPointer(void)
{
    uint8_t data[] = {0x01, 0x02, 0x03};
    EDD_ReturnType ret;

    ret = EDD_VerifyCrc8(data, 3, 0x12,
                                       ((void *)0)
                                           );

    UnityAssertEqualNumber((UNITY_INT)((EDD_NULL_PTR)), (UNITY_INT)((ret)), (
   ((void *)0)
   ), (UNITY_UINT)(52), UNITY_DISPLAY_STYLE_INT);
}

void test_EDD_VerifyCrc8_BothPointersNull(void)
{
    EDD_ReturnType ret;

    ret = EDD_VerifyCrc8(
                        ((void *)0)
                            , 5, 0x12,
                                       ((void *)0)
                                           );

    UnityAssertEqualNumber((UNITY_INT)((EDD_NULL_PTR)), (UNITY_INT)((ret)), (
   ((void *)0)
   ), (UNITY_UINT)(65), UNITY_DISPLAY_STYLE_INT);
}

void test_EDD_VerifyCrc8_ZeroLength(void)
{
    uint8_t data[] = {0x01};
    uint8_t result = 0xFF;
    EDD_ReturnType ret;

    ret = EDD_VerifyCrc8(data, 0, 0x12, &result);

    UnityAssertEqualNumber((UNITY_INT)((EDD_INVALID_LENGTH)), (UNITY_INT)((ret)), (
   ((void *)0)
   ), (UNITY_UINT)(80), UNITY_DISPLAY_STYLE_INT);
}

void test_EDD_VerifyCrc8_ValidCrcMatch(void)
{
    uint8_t data[] = {0x01, 0x02, 0x03, 0x04};
    uint8_t expected_crc = 0xA5;
    uint8_t result = 0xFF;
    EDD_ReturnType ret;

    EDD_Crc8ComputeInternal_CMockExpectAndReturn(95, data, 4, expected_crc);

    ret = EDD_VerifyCrc8(data, 4, expected_crc, &result);

    UnityAssertEqualNumber((UNITY_INT)((EDD_OK)), (UNITY_INT)((ret)), (
   ((void *)0)
   ), (UNITY_UINT)(99), UNITY_DISPLAY_STYLE_INT);
    UnityAssertEqualNumber((UNITY_INT)((1u)), (UNITY_INT)((result)), (
   ((void *)0)
   ), (UNITY_UINT)(100), UNITY_DISPLAY_STYLE_INT);
}

void test_EDD_VerifyCrc8_ValidCrcMismatch(void)
{
    uint8_t data[] = {0x01, 0x02, 0x03, 0x04};
    uint8_t expected_crc = 0xA5;
    uint8_t computed_crc = 0x3C;
    uint8_t result = 0xFF;
    EDD_ReturnType ret;

    EDD_Crc8ComputeInternal_CMockExpectAndReturn(116, data, 4, computed_crc);

    ret = EDD_VerifyCrc8(data, 4, expected_crc, &result);

    UnityAssertEqualNumber((UNITY_INT)((EDD_OK)), (UNITY_INT)((ret)), (
   ((void *)0)
   ), (UNITY_UINT)(120), UNITY_DISPLAY_STYLE_INT);
    UnityAssertEqualNumber((UNITY_INT)((0u)), (UNITY_INT)((result)), (
   ((void *)0)
   ), (UNITY_UINT)(121), UNITY_DISPLAY_STYLE_INT);
}

void test_EDD_VerifyCrc8_SingleByteMatch(void)
{
    uint8_t data[] = {0xAA};
    uint8_t expected_crc = 0x7B;
    uint8_t result = 0xFF;
    EDD_ReturnType ret;

    EDD_Crc8ComputeInternal_CMockExpectAndReturn(135, data, 1, expected_crc);

    ret = EDD_VerifyCrc8(data, 1, expected_crc, &result);

    UnityAssertEqualNumber((UNITY_INT)((EDD_OK)), (UNITY_INT)((ret)), (
   ((void *)0)
   ), (UNITY_UINT)(139), UNITY_DISPLAY_STYLE_INT);
    UnityAssertEqualNumber((UNITY_INT)((1u)), (UNITY_INT)((result)), (
   ((void *)0)
   ), (UNITY_UINT)(140), UNITY_DISPLAY_STYLE_INT);
}

void test_EDD_VerifyCrc8_MaxLengthMatch(void)
{
    uint8_t data[255] = {0};
    uint8_t expected_crc = 0x55;
    uint8_t result = 0xFF;
    EDD_ReturnType ret;

    for (int i = 0; i < 255; i++)
{
        data[i] = (uint8_t)(i & 0xFF);
    }

    EDD_Crc8ComputeInternal_CMockExpectAndReturn(160, data, 255, expected_crc);

    ret = EDD_VerifyCrc8(data, 255, expected_crc, &result);

    UnityAssertEqualNumber((UNITY_INT)((EDD_OK)), (UNITY_INT)((ret)), (
   ((void *)0)
   ), (UNITY_UINT)(164), UNITY_DISPLAY_STYLE_INT);
    UnityAssertEqualNumber((UNITY_INT)((1u)), (UNITY_INT)((result)), (
   ((void *)0)
   ), (UNITY_UINT)(165), UNITY_DISPLAY_STYLE_INT);
}

void test_EDD_VerifyCrc8_LengthOne(void)
{
    uint8_t data[] = {0x42};
    uint8_t expected_crc = 0xE0;
    uint8_t result = 0xFF;
    EDD_ReturnType ret;

    EDD_Crc8ComputeInternal_CMockExpectAndReturn(179, data, 1, expected_crc);

    ret = EDD_VerifyCrc8(data, 1, expected_crc, &result);

    UnityAssertEqualNumber((UNITY_INT)((EDD_OK)), (UNITY_INT)((ret)), (
   ((void *)0)
   ), (UNITY_UINT)(183), UNITY_DISPLAY_STYLE_INT);
    UnityAssertEqualNumber((UNITY_INT)((1u)), (UNITY_INT)((result)), (
   ((void *)0)
   ), (UNITY_UINT)(184), UNITY_DISPLAY_STYLE_INT);
}

void test_EDD_VerifyCrc8_CrcZero(void)
{
    uint8_t data[] = {0x00, 0x00};
    uint8_t expected_crc = 0x00;
    uint8_t result = 0xFF;
    EDD_ReturnType ret;

    EDD_Crc8ComputeInternal_CMockExpectAndReturn(198, data, 2, expected_crc);

    ret = EDD_VerifyCrc8(data, 2, expected_crc, &result);

    UnityAssertEqualNumber((UNITY_INT)((EDD_OK)), (UNITY_INT)((ret)), (
   ((void *)0)
   ), (UNITY_UINT)(202), UNITY_DISPLAY_STYLE_INT);
    UnityAssertEqualNumber((UNITY_INT)((1u)), (UNITY_INT)((result)), (
   ((void *)0)
   ), (UNITY_UINT)(203), UNITY_DISPLAY_STYLE_INT);
}

void test_EDD_VerifyCrc8_CrcMaxValue(void)
{
    uint8_t data[] = {0xFF, 0xFF};
    uint8_t expected_crc = 0xFF;
    uint8_t result = 0xFF;
    EDD_ReturnType ret;

    EDD_Crc8ComputeInternal_CMockExpectAndReturn(217, data, 2, expected_crc);

    ret = EDD_VerifyCrc8(data, 2, expected_crc, &result);

    UnityAssertEqualNumber((UNITY_INT)((EDD_OK)), (UNITY_INT)((ret)), (
   ((void *)0)
   ), (UNITY_UINT)(221), UNITY_DISPLAY_STYLE_INT);
    UnityAssertEqualNumber((UNITY_INT)((1u)), (UNITY_INT)((result)), (
   ((void *)0)
   ), (UNITY_UINT)(222), UNITY_DISPLAY_STYLE_INT);
}