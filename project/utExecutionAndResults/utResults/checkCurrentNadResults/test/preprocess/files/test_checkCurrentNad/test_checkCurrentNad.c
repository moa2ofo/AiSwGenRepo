// CEEDLING NOTICE: This generated file only to be consumed for test runner creation

#include "utExecutionAndResults/utUnderTest/build/vendor/unity/src/unity.h"
#include "utExecutionAndResults/utUnderTest/src/checkCurrentNad.h"

void setUp(void)
{
}

void tearDown(void)
{
}

void test_checkCurrentNad_WithInvalidNad_0x7F_ReturnsNotOk(void)
{
    Std_ReturnType result = ((Std_ReturnType)0x00u);

    checkCurrentNad(0x7F, &result);

    UnityAssertEqualNumber((UNITY_INT)((((Std_ReturnType)0x01u))), (UNITY_INT)((result)), (
   ((void *)0)
   ), (UNITY_UINT)(24), UNITY_DISPLAY_STYLE_INT);
}

void test_checkCurrentNad_WithInvalidNad_0x7E_ReturnsNotOk(void)
{
    Std_ReturnType result = ((Std_ReturnType)0x00u);

    checkCurrentNad(0x7E, &result);

    UnityAssertEqualNumber((UNITY_INT)((((Std_ReturnType)0x01u))), (UNITY_INT)((result)), (
   ((void *)0)
   ), (UNITY_UINT)(37), UNITY_DISPLAY_STYLE_INT);
}

void test_checkCurrentNad_WithValidNad_0x00_ReturnsOk(void)
{
    Std_ReturnType result = ((Std_ReturnType)0x01u);

    checkCurrentNad(0x00, &result);

    UnityAssertEqualNumber((UNITY_INT)((((Std_ReturnType)0x00u))), (UNITY_INT)((result)), (
   ((void *)0)
   ), (UNITY_UINT)(50), UNITY_DISPLAY_STYLE_INT);
}

void test_checkCurrentNad_WithValidNad_0x01_ReturnsOk(void)
{
    Std_ReturnType result = ((Std_ReturnType)0x01u);

    checkCurrentNad(0x01, &result);

    UnityAssertEqualNumber((UNITY_INT)((((Std_ReturnType)0x00u))), (UNITY_INT)((result)), (
   ((void *)0)
   ), (UNITY_UINT)(63), UNITY_DISPLAY_STYLE_INT);
}

void test_checkCurrentNad_WithValidNad_0x7D_ReturnsOk(void)
{
    Std_ReturnType result = ((Std_ReturnType)0x00u);

    checkCurrentNad(0x7D, &result);

    UnityAssertEqualNumber((UNITY_INT)((((Std_ReturnType)0x00u))), (UNITY_INT)((result)), (
   ((void *)0)
   ), (UNITY_UINT)(76), UNITY_DISPLAY_STYLE_INT);
}

void test_checkCurrentNad_WithValidNad_0xFF_ReturnsOk(void)
{
    Std_ReturnType result = ((Std_ReturnType)0x01u);

    checkCurrentNad(0xFF, &result);

    UnityAssertEqualNumber((UNITY_INT)((((Std_ReturnType)0x00u))), (UNITY_INT)((result)), (
   ((void *)0)
   ), (UNITY_UINT)(89), UNITY_DISPLAY_STYLE_INT);
}

void test_checkCurrentNad_WithValidNad_0x80_ReturnsOk(void)
{
    Std_ReturnType result = ((Std_ReturnType)0x01u);

    checkCurrentNad(0x80, &result);

    UnityAssertEqualNumber((UNITY_INT)((((Std_ReturnType)0x00u))), (UNITY_INT)((result)), (
   ((void *)0)
   ), (UNITY_UINT)(102), UNITY_DISPLAY_STYLE_INT);
}