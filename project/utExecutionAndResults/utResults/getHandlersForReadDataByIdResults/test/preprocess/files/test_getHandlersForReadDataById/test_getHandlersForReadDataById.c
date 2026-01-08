// CEEDLING NOTICE: This generated file only to be consumed for test runner creation

#include "utExecutionAndResults/utUnderTest/build/vendor/unity/src/unity.h"
#include "utExecutionAndResults/utUnderTest/src/getHandlersForReadDataById.h"
#include "mock_diagnostic_cfg.h"

void setUp(void)
{
}

void tearDown(void)
{
}

void test_getHandlersForReadDataById_ValidDID_F308(void)
{
    g_errCode_u8 = 0;
    uint16 l_did_cu16 = 0xF308;
    uint8 l_diagBufSize_u8 = 0;
    Std_ReturnType l_didSupported_ = ((Std_ReturnType)0x00u);
    uint8 l_diagBuf_pu8[10] = {0};
    SubfunctionRequestOutOfRange__CMockIgnoreAndReturn(26, ((Std_ReturnType)0x01u));

    RdbiVhitOverVoltageFaultDiag__CMockIgnoreAndReturn(28, ((Std_ReturnType)0x00u));

    Std_ReturnType result = getHandlersForReadDataById(&g_errCode_u8, l_did_cu16,
                                                       &l_diagBufSize_u8, &l_didSupported_,
                                                       l_diagBuf_pu8);

    UnityAssertEqualNumber((UNITY_INT)((1U)), (UNITY_INT)((l_diagBufSize_u8)), (
   ((void *)0)
   ), (UNITY_UINT)(36), UNITY_DISPLAY_STYLE_INT);

    UnityAssertEqualNumber((UNITY_INT)((((Std_ReturnType)0x00u))), (UNITY_INT)((l_didSupported_)), (
   ((void *)0)
   ), (UNITY_UINT)(39), UNITY_DISPLAY_STYLE_INT);

    UnityAssertEqualNumber((UNITY_INT)((((Std_ReturnType)0x00u))), (UNITY_INT)((result)), (
   ((void *)0)
   ), (UNITY_UINT)(42), UNITY_DISPLAY_STYLE_INT);
}

void test_getHandlersForReadDataById_InvalidDID(void)
{
    g_errCode_u8 = 0;
    g_did_cu16 = 0x1234;
    uint8 l_diagBufSize_u8 = 0xFF;
    Std_ReturnType l_didSupported_ = ((Std_ReturnType)0x00u);
    uint8 l_diagBuf_pu8[10] = {0};

    SubfunctionRequestOutOfRange__CMockIgnoreAndReturn(57, ((Std_ReturnType)0x01u));

    Std_ReturnType result = getHandlersForReadDataById(&g_errCode_u8, g_did_cu16,
                                                       &l_diagBufSize_u8, &l_didSupported_,
                                                       l_diagBuf_pu8);

    UnityAssertEqualNumber((UNITY_INT)((((Std_ReturnType)0x01u))), (UNITY_INT)((l_didSupported_)), (
   ((void *)0)
   ), (UNITY_UINT)(65), UNITY_DISPLAY_STYLE_INT);

    UnityAssertEqualNumber((UNITY_INT)((((uint8)0x31u))), (UNITY_INT)((g_errCode_u8)), (
   ((void *)0)
   ), (UNITY_UINT)(68), UNITY_DISPLAY_STYLE_INT);

    UnityAssertEqualNumber((UNITY_INT)((((Std_ReturnType)0x01u))), (UNITY_INT)((result)), (
   ((void *)0)
   ), (UNITY_UINT)(71), UNITY_DISPLAY_STYLE_INT);
}

void test_getHandlersForReadDataById_MultipleInvalidDIDs(void)
{
    uint16 invalid_dids[] = {0x0000, 0x0001, 0xFFFF, 0xF300, 0xF309};

    for (size_t i = 0; i < sizeof(invalid_dids) / sizeof(invalid_dids[0]); i++)
{
        g_errCode_u8 = 0;
        uint8 l_diagBufSize_u8 = 0xFF;
        Std_ReturnType l_didSupported_ = ((Std_ReturnType)0x00u);
        uint8 l_diagBuf_pu8[10] = {0};
        g_did_cu16 = invalid_dids[i];

        SubfunctionRequestOutOfRange__CMockIgnoreAndReturn(89, ((Std_ReturnType)0x01u));

        Std_ReturnType result = getHandlersForReadDataById(&g_errCode_u8, g_did_cu16,
                                                           &l_diagBufSize_u8, &l_didSupported_,
                                                           l_diagBuf_pu8);

        UnityAssertEqualNumber((UNITY_INT)((((Std_ReturnType)0x01u))), (UNITY_INT)((l_didSupported_)), (("didSupported should be E_NOT_OK")), (UNITY_UINT)(95), UNITY_DISPLAY_STYLE_INT);
        UnityAssertEqualNumber((UNITY_INT)((((uint8)0x31u))), (UNITY_INT)((g_errCode_u8)), (("errCode should be set to kLinDiagNrcRequestOutOfRange")), (UNITY_UINT)(96), UNITY_DISPLAY_STYLE_INT);
    }
}

void test_getHandlersForReadDataById_ValidDID_HandlerReturnsError(void)
{
    g_errCode_u8 = 0;
    g_did_cu16 = 0xF308;
    uint8 l_diagBufSize_u8 = 0;
    Std_ReturnType l_didSupported_ = ((Std_ReturnType)0x00u);
    uint8 l_diagBuf_pu8[10] = {0};
    SubfunctionRequestOutOfRange__CMockIgnoreAndReturn(110, ((Std_ReturnType)0x01u));

    RdbiVhitOverVoltageFaultDiag__CMockIgnoreAndReturn(113, ((Std_ReturnType)0x01u));

    Std_ReturnType result = getHandlersForReadDataById(&g_errCode_u8, g_did_cu16,
                                                       &l_diagBufSize_u8, &l_didSupported_,
                                                       l_diagBuf_pu8);

    UnityAssertEqualNumber((UNITY_INT)((1U)), (UNITY_INT)((l_diagBufSize_u8)), (
   ((void *)0)
   ), (UNITY_UINT)(120), UNITY_DISPLAY_STYLE_INT);

    UnityAssertEqualNumber((UNITY_INT)((((Std_ReturnType)0x01u))), (UNITY_INT)((result)), (
   ((void *)0)
   ), (UNITY_UINT)(123), UNITY_DISPLAY_STYLE_INT);
}

void test_getHandlersForReadDataById_ErrorCodePassedToHandler(void)
{
    g_errCode_u8 = 0x42;
    g_did_cu16 = 0xF308;
    uint8 l_diagBufSize_u8 = 0;
    Std_ReturnType l_didSupported_ = ((Std_ReturnType)0x00u);
    uint8 l_diagBuf_pu8[10] = {0};

    SubfunctionRequestOutOfRange__CMockIgnoreAndReturn(137, ((Std_ReturnType)0x01u));
    RdbiVhitOverVoltageFaultDiag__CMockIgnoreAndReturn(138, ((Std_ReturnType)0x00u));

    Std_ReturnType result = getHandlersForReadDataById(&g_errCode_u8, g_did_cu16,
                                                       &l_diagBufSize_u8, &l_didSupported_,
                                                       l_diagBuf_pu8);

    UnityAssertEqualNumber((UNITY_INT)((((Std_ReturnType)0x00u))), (UNITY_INT)((result)), (
   ((void *)0)
   ), (UNITY_UINT)(144), UNITY_DISPLAY_STYLE_INT);
}

void test_getHandlersForReadDataById_OutputBufferPassedToHandler(void)
{
    g_errCode_u8 = 0;
    g_did_cu16 = 0xF308;
    uint8 l_diagBufSize_u8 = 0;
    Std_ReturnType l_didSupported_ = ((Std_ReturnType)0x00u);
    uint8 l_diagBuf_pu8[10] = {0};

    SubfunctionRequestOutOfRange__CMockIgnoreAndReturn(159, ((Std_ReturnType)0x01u));
    RdbiVhitOverVoltageFaultDiag__CMockIgnoreAndReturn(160, ((Std_ReturnType)0x00u));

    Std_ReturnType result = getHandlersForReadDataById(&g_errCode_u8, g_did_cu16,
                                                       &l_diagBufSize_u8, &l_didSupported_,
                                                       l_diagBuf_pu8);
    UnityAssertEqualNumber((UNITY_INT)((((Std_ReturnType)0x00u))), (UNITY_INT)((result)), (
   ((void *)0)
   ), (UNITY_UINT)(165), UNITY_DISPLAY_STYLE_INT);
}

void test_getHandlersForReadDataById_BufferSizePointerPassedToHandler(void)
{
    g_errCode_u8 = 0;
    g_did_cu16 = 0xF308;
    uint8 l_diagBufSize_u8 = 0;
    Std_ReturnType l_didSupported_ = ((Std_ReturnType)0x00u);
    uint8 l_diagBuf_pu8[10] = {0};

    SubfunctionRequestOutOfRange__CMockIgnoreAndReturn(179, ((Std_ReturnType)0x01u));
    RdbiVhitOverVoltageFaultDiag__CMockIgnoreAndReturn(180, ((Std_ReturnType)0x00u));

    Std_ReturnType result = getHandlersForReadDataById(&g_errCode_u8, g_did_cu16,
                                                       &l_diagBufSize_u8, &l_didSupported_,
                                                       l_diagBuf_pu8);

    UnityAssertEqualNumber((UNITY_INT)((1U)), (UNITY_INT)((l_diagBufSize_u8)), (
   ((void *)0)
   ), (UNITY_UINT)(187), UNITY_DISPLAY_STYLE_INT);
    UnityAssertEqualNumber((UNITY_INT)((((Std_ReturnType)0x00u))), (UNITY_INT)((result)), (
   ((void *)0)
   ), (UNITY_UINT)(188), UNITY_DISPLAY_STYLE_INT);
}