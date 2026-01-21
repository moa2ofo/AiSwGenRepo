#include "VoltMon_ReadVoltageProject_mV.h"
#include "unity.h"

/* Extern declarations for static variables from the implementation */
extern uint16_t supplyDcFiler_u16;
extern uint16_t supplyDcNotFiler_u16;
extern const uint16_t lowerVoltMonCfg_cu16;
extern const uint16_t higherVoltMonCfg_cu16;
extern const uint16_t middleLowerVoltMonCfg_cu16;
extern const uint16_t middleHigherVoltMonCfg_cu16;

/* ============================================================================
 * Setup and Teardown
 * ============================================================================ */

void setUp(void) {
  /* Reset static variables before each test */
  supplyDcFiler_u16 = 0u;
  supplyDcNotFiler_u16 = 0u;
}

void tearDown(void) { /* Cleanup after each test if needed */ }

/* ============================================================================
 * Test Cases for LOW BAND
 * Range: 0 < unfiltered < 5000
 * Expected: Return unfiltered value
 * ============================================================================ */

/**
 * @test LOW BAND: Voltage in low range (lower boundary + 1)
 * Input: unfiltered = 1 (just above lower limit)
 * Expected: Return 1
 */
void test_VoltMon_ReadVoltageProject_mV_LowBand_LowerBoundaryPlus1(void) {
  supplyDcNotFiler_u16 = 1u;
  supplyDcFiler_u16 = 5000u; /* Different value to verify unfiltered is used */

  uint16_t result = VoltMon_ReadVoltageProject_mV();

  TEST_ASSERT_EQUAL_UINT16(1u, result);
}

/**
 * @test LOW BAND: Voltage in low range (mid value)
 * Input: unfiltered = 2500
 * Expected: Return 2500
 */
void test_VoltMon_ReadVoltageProject_mV_LowBand_MidValue(void) {
  supplyDcNotFiler_u16 = 2500u;
  supplyDcFiler_u16 = 8000u; /* Different value to verify unfiltered is used */

  uint16_t result = VoltMon_ReadVoltageProject_mV();

  TEST_ASSERT_EQUAL_UINT16(2500u, result);
}

/**
 * @test LOW BAND: Voltage in low range (upper boundary - 1)
 * Input: unfiltered = 4999 (just below middle-lower limit)
 * Expected: Return 4999
 */
void test_VoltMon_ReadVoltageProject_mV_LowBand_UpperBoundaryMinus1(void) {
  supplyDcNotFiler_u16 = 4999u;
  supplyDcFiler_u16 = 10000u; /* Different value to verify unfiltered is used */

  uint16_t result = VoltMon_ReadVoltageProject_mV();

  TEST_ASSERT_EQUAL_UINT16(4999u, result);
}

/* ============================================================================
 * Test Cases for MIDDLE BAND
 * Range: 5000 <= unfiltered <= 10000
 * Expected: Return average of (filtered + unfiltered) >> 1
 * ============================================================================ */

/**
 * @test MIDDLE BAND: At lower boundary (5000)
 * Input: unfiltered = 5000, filtered = 6000
 * Expected: Return (6000 + 5000) >> 1 = 5500
 */
void test_VoltMon_ReadVoltageProject_mV_MiddleBand_LowerBoundary(void) {
  supplyDcNotFiler_u16 = 5000u;
  supplyDcFiler_u16 = 6000u;

  uint16_t result = VoltMon_ReadVoltageProject_mV();

  uint16_t expected = (6000u + 5000u) >> 1; /* 5500 */
  TEST_ASSERT_EQUAL_UINT16(expected, result);
}

/**
 * @test MIDDLE BAND: At mid value
 * Input: unfiltered = 7500, filtered = 8000
 * Expected: Return (8000 + 7500) >> 1 = 7750
 */
void test_VoltMon_ReadVoltageProject_mV_MiddleBand_MidValue(void) {
  supplyDcNotFiler_u16 = 7500u;
  supplyDcFiler_u16 = 8000u;

  uint16_t result = VoltMon_ReadVoltageProject_mV();

  uint16_t expected = (8000u + 7500u) >> 1; /* 7750 */
  TEST_ASSERT_EQUAL_UINT16(expected, result);
}

/**
 * @test MIDDLE BAND: At upper boundary (10000)
 * Input: unfiltered = 10000, filtered = 9500
 * Expected: Return (9500 + 10000) >> 1 = 9750
 */
void test_VoltMon_ReadVoltageProject_mV_MiddleBand_UpperBoundary(void) {
  supplyDcNotFiler_u16 = 10000u;
  supplyDcFiler_u16 = 9500u;

  uint16_t result = VoltMon_ReadVoltageProject_mV();

  uint16_t expected = (9500u + 10000u) >> 1; /* 9750 */
  TEST_ASSERT_EQUAL_UINT16(expected, result);
}

/**
 * @test MIDDLE BAND: Both filtered and unfiltered same value
 * Input: unfiltered = 7500, filtered = 7500
 * Expected: Return (7500 + 7500) >> 1 = 7500
 */
void test_VoltMon_ReadVoltageProject_mV_MiddleBand_SameValues(void) {
  supplyDcNotFiler_u16 = 7500u;
  supplyDcFiler_u16 = 7500u;

  uint16_t result = VoltMon_ReadVoltageProject_mV();

  uint16_t expected = (7500u + 7500u) >> 1; /* 7500 */
  TEST_ASSERT_EQUAL_UINT16(expected, result);
}

/* ============================================================================
 * Test Cases for HIGH BAND
 * Range: 10000 < unfiltered < 20000
 * Expected: Return average of (filtered + unfiltered) >> 1
 * ============================================================================ */

/**
 * @test HIGH BAND: Just above middle-higher limit (10001)
 * Input: unfiltered = 10001, filtered = 12000
 * Expected: Return (12000 + 10001) >> 1 = 11000
 */
void test_VoltMon_ReadVoltageProject_mV_HighBand_LowerBoundaryPlus1(void) {
  supplyDcNotFiler_u16 = 10001u;
  supplyDcFiler_u16 = 12000u;

  uint16_t result = VoltMon_ReadVoltageProject_mV();

  uint16_t expected = (12000u + 10001u) >> 1; /* 11000 */
  TEST_ASSERT_EQUAL_UINT16(expected, result);
}

/**
 * @test HIGH BAND: Mid value
 * Input: unfiltered = 15000, filtered = 14000
 * Expected: Return (14000 + 15000) >> 1 = 14500
 */
void test_VoltMon_ReadVoltageProject_mV_HighBand_MidValue(void) {
  supplyDcNotFiler_u16 = 15000u;
  supplyDcFiler_u16 = 14000u;

  uint16_t result = VoltMon_ReadVoltageProject_mV();

  uint16_t expected = (14000u + 15000u) >> 1; /* 14500 */
  TEST_ASSERT_EQUAL_UINT16(expected, result);
}

/**
 * @test HIGH BAND: Just below upper limit (19999)
 * Input: unfiltered = 19999, filtered = 18000
 * Expected: Return (18000 + 19999) >> 1 = 18999
 */
void test_VoltMon_ReadVoltageProject_mV_HighBand_UpperBoundaryMinus1(void) {
  supplyDcNotFiler_u16 = 19999u;
  supplyDcFiler_u16 = 18000u;

  uint16_t result = VoltMon_ReadVoltageProject_mV();

  uint16_t expected = (18000u + 19999u) >> 1; /* 18999 */
  TEST_ASSERT_EQUAL_UINT16(expected, result);
}

/* ============================================================================
 * Test Cases for OUT OF RANGE
 * Conditions: unfiltered <= 0 OR unfiltered >= 20000
 * Expected: Return 0
 * ============================================================================ */

/**
 * @test OUT OF RANGE: At lower boundary (0)
 * Input: unfiltered = 0
 * Expected: Return 0
 */
void test_VoltMon_ReadVoltageProject_mV_OutOfRange_AtLowerBoundary(void) {
  supplyDcNotFiler_u16 = 0u;
  supplyDcFiler_u16 = 5000u;

  uint16_t result = VoltMon_ReadVoltageProject_mV();

  TEST_ASSERT_EQUAL_UINT16(0u, result);
}

/**
 * @test OUT OF RANGE: At upper boundary (20000)
 * Input: unfiltered = 20000
 * Expected: Return 0
 */
void test_VoltMon_ReadVoltageProject_mV_OutOfRange_AtUpperBoundary(void) {
  supplyDcNotFiler_u16 = 20000u;
  supplyDcFiler_u16 = 15000u;

  uint16_t result = VoltMon_ReadVoltageProject_mV();

  TEST_ASSERT_EQUAL_UINT16(0u, result);
}

/**
 * @test OUT OF RANGE: Above upper boundary (20001)
 * Input: unfiltered = 20001
 * Expected: Return 0
 */
void test_VoltMon_ReadVoltageProject_mV_OutOfRange_AboveUpperBoundary(void) {
  supplyDcNotFiler_u16 = 20001u;
  supplyDcFiler_u16 = 15000u;

  uint16_t result = VoltMon_ReadVoltageProject_mV();

  TEST_ASSERT_EQUAL_UINT16(0u, result);
}

/**
 * @test OUT OF RANGE: Maximum uint16 value
 * Input: unfiltered = 65535
 * Expected: Return 0
 */
void test_VoltMon_ReadVoltageProject_mV_OutOfRange_MaxUint16(void) {
  supplyDcNotFiler_u16 = 65535u;
  supplyDcFiler_u16 = 10000u;

  uint16_t result = VoltMon_ReadVoltageProject_mV();

  TEST_ASSERT_EQUAL_UINT16(0u, result);
}

/* ============================================================================
 * Test Cases for Edge Cases and Special Scenarios
 * ============================================================================ */

/**
 * @test Edge Case: Filtered value is 0 in middle band
 * Input: unfiltered = 7500, filtered = 0
 * Expected: Return (0 + 7500) >> 1 = 3750
 */
void test_VoltMon_ReadVoltageProject_mV_MiddleBand_FilteredZero(void) {
  supplyDcNotFiler_u16 = 7500u;
  supplyDcFiler_u16 = 0u;

  uint16_t result = VoltMon_ReadVoltageProject_mV();

  uint16_t expected = (0u + 7500u) >> 1; /* 3750 */
  TEST_ASSERT_EQUAL_UINT16(expected, result);
}

/**
 * @test Edge Case: Both values at maximum in middle band
 * Input: unfiltered = 10000, filtered = 65535
 * Expected: Return (65535 + 10000) >> 1 = 37767
 */
void test_VoltMon_ReadVoltageProject_mV_MiddleBand_FilteredMaxValue(void) {
  supplyDcNotFiler_u16 = 10000u;
  supplyDcFiler_u16 = 65535u;

  uint16_t result = VoltMon_ReadVoltageProject_mV();

  uint16_t expected = (65535u + 10000u) >> 1; /* 37767 */
  TEST_ASSERT_EQUAL_UINT16(expected, result);
}

/**
 * @test Boundary Transition: From low band to middle band
 * Verify that unfiltered = 5000 is in middle band (not low band)
 */
void test_VoltMon_ReadVoltageProject_mV_BoundaryTransition_5000(void) {
  supplyDcNotFiler_u16 = 5000u;
  supplyDcFiler_u16 = 6000u;

  uint16_t result = VoltMon_ReadVoltageProject_mV();

  /* Should use average (middle band), not just unfiltered (low band) */
  uint16_t expected = (6000u + 5000u) >> 1; /* 5500 */
  TEST_ASSERT_EQUAL_UINT16(expected, result);
}

/**
 * @test Boundary Transition: From middle band to high band
 * Verify that unfiltered = 10000 is in middle band (not high band)
 */
void test_VoltMon_ReadVoltageProject_mV_BoundaryTransition_10000(void) {
  supplyDcNotFiler_u16 = 10000u;
  supplyDcFiler_u16 = 9000u;

  uint16_t result = VoltMon_ReadVoltageProject_mV();

  /* Should use average (middle band), not high band behavior */
  uint16_t expected = (9000u + 10000u) >> 1; /* 9500 */
  TEST_ASSERT_EQUAL_UINT16(expected, result);
}
