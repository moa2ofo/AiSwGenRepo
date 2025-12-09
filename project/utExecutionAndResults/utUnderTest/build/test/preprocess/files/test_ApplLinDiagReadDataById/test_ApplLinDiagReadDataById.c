// CEEDLING NOTICE: This generated file only to be consumed for test runner creation

#include "utExecutionAndResults/utUnderTest/build/vendor/unity/src/unity.h"
#include "utExecutionAndResults/utUnderTest/src/ApplLinDiagReadDataById.h"
#include "mock_diagnostic_cfg.h"
#include "mock_diagnostic_priv.h"

void CurrentNad_Callback(uint8 currentNad, Std_ReturnType* result, int cmock_num_calls)
{
    (void)cmock_num_calls;

    if (currentNad == 0u)
{
        *result = ((Std_ReturnType)0x00u);
    }
    else
    {
        *result = ((Std_ReturnType)0x01u);
    }
}

void MsgDataLength_Callback(uint16_t dataLength, Std_ReturnType* result, int cmock_num_calls)
{
    (void)dataLength;
    (void)cmock_num_calls;
    *result = ((Std_ReturnType)0x00u);
}

Std_ReturnType getHandlersForReadDataById_Callback(uint8 l_errCode_u8,
                                                   uint16 l_did_cu16,
                                                   uint8 *l_diagBufSize_u8,
                                                   Std_ReturnType *l_didSupported_,
                                                   uint8 *l_diagBuf_pu8,
                                                   int cmock_num_calls)
{
    Std_ReturnType l_result_ = ((Std_ReturnType)0x00u);
    (void)l_errCode_u8;
    (void)l_diagBuf_pu8;
    (void)cmock_num_calls;

    switch (l_did_cu16)
{
        case 0xF308:
            *l_diagBufSize_u8 = 4;
            break;

        default:
            *l_didSupported_ = ((Std_ReturnType)0x01u);
            l_result_ = ((Std_ReturnType)0x01u);

            break;
    }

    return l_result_;
}

static void CurrentNad_Fail_Callback(uint8 currentNad, Std_ReturnType* result, int cmock_num_calls)
{
    (void)currentNad;
    (void)cmock_num_calls;
    *result = ((Std_ReturnType)0x01u);
}

static void MsgDataLength_Fail_Callback(uint16_t dataLength, Std_ReturnType* result, int cmock_num_calls)
{
    (void)dataLength;
    (void)cmock_num_calls;
    *result = ((Std_ReturnType)0x01u);
}

static Std_ReturnType getHandlersForReadDataById_Fail_Callback(uint8 l_errCode_u8,
                                                               uint16 l_did_cu16,
                                                               uint8 *l_diagBufSize_u8,
                                                               Std_ReturnType *l_didSupported_,
                                                               uint8 *l_diagBuf_pu8,
                                                               int cmock_num_calls)
{
    (void)l_errCode_u8;
    (void)l_did_cu16;
    (void)l_diagBufSize_u8;
    (void)l_didSupported_;
    (void)l_diagBuf_pu8;
    (void)cmock_num_calls;

    return ((Std_ReturnType)0x01u);
}

void setUp(void)
{
    checkCurrentNad_Stub(CurrentNad_Callback);
    checkMsgDataLength_Stub(MsgDataLength_Callback);
    getHandlersForReadDataById_Stub(getHandlersForReadDataById_Callback);

    extern uint8_t pbLinDiagBuffer[32];
    extern uint16_t g_linDiagDataLength;

    memset(pbLinDiagBuffer, 0, sizeof(pbLinDiagBuffer));
    g_linDiagDataLength = 0;
}

void tearDown(void)
{
}

void test_ApplLinDiagReadDataById_DidNotSupported(void)
{
    extern uint8_t pbLinDiagBuffer[32];
    extern uint16_t g_linDiagDataLength;

    pbLinDiagBuffer[1] = 0x12u;
    pbLinDiagBuffer[2] = 0x34u;

    g_linDiagDataLength = 0u;

    LinDiagSendNegResponse_CMockExpect(133, 0);

    ApplLinDiagReadDataById();

    UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT16)((0u)), (UNITY_INT)(UNITY_UINT16)((g_linDiagDataLength)), (
   ((void *)0)
   ), (UNITY_UINT)(139), UNITY_DISPLAY_STYLE_UINT16);
}

void test_ApplLinDiagReadDataById_SuccessfulExecution_DidSupported(void)
{
    extern uint8_t pbLinDiagBuffer[32];
    extern uint16_t g_linDiagDataLength;

    pbLinDiagBuffer[1] = 0xF3u;
    pbLinDiagBuffer[2] = 0x08u;

    g_linDiagDataLength = 0u;

    LinDiagSendPosResponse_CMockExpect(157);

    ApplLinDiagReadDataById();

    UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT16)((4 + 2u)), (UNITY_INT)(UNITY_UINT16)((g_linDiagDataLength)), (
   ((void *)0)
   ), (UNITY_UINT)(163), UNITY_DISPLAY_STYLE_UINT16);
}

void test_ApplLinDiagReadDataById_WrongNad_Fails(void)
{
    extern uint8_t pbLinDiagBuffer[32];
    extern uint16_t g_linDiagDataLength;

    pbLinDiagBuffer[1] = 0xF3u;
    pbLinDiagBuffer[2] = 0x08u;

    g_linDiagDataLength = 0u;

    checkCurrentNad_Stub(CurrentNad_Fail_Callback);

    LinDiagSendNegResponse_CMockExpect(184, 0);

    ApplLinDiagReadDataById();

    UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT16)((0u)), (UNITY_INT)(UNITY_UINT16)((g_linDiagDataLength)), (
   ((void *)0)
   ), (UNITY_UINT)(190), UNITY_DISPLAY_STYLE_UINT16);
}

void test_ApplLinDiagReadDataById_MsgDataLength_Fails(void)
{
    extern uint8_t pbLinDiagBuffer[32];
    extern uint16_t g_linDiagDataLength;

    pbLinDiagBuffer[1] = 0xF3u;
    pbLinDiagBuffer[2] = 0x08u;

    g_linDiagDataLength = 10u;

    checkCurrentNad_Stub(CurrentNad_Callback);
    checkMsgDataLength_Stub(MsgDataLength_Fail_Callback);

    LinDiagSendNegResponse_CMockExpect(211, 0);

    ApplLinDiagReadDataById();

    UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT16)((10u)), (UNITY_INT)(UNITY_UINT16)((g_linDiagDataLength)), (
   ((void *)0)
   ), (UNITY_UINT)(217), UNITY_DISPLAY_STYLE_UINT16);
}

void test_ApplLinDiagReadDataById_HandlerFails(void)
{
    extern uint8_t pbLinDiagBuffer[32];
    extern uint16_t g_linDiagDataLength;

    pbLinDiagBuffer[1] = 0xF3u;
    pbLinDiagBuffer[2] = 0x08u;

    g_linDiagDataLength = 5u;

    checkCurrentNad_Stub(CurrentNad_Callback);
    checkMsgDataLength_Stub(MsgDataLength_Callback);
    getHandlersForReadDataById_Stub(getHandlersForReadDataById_Fail_Callback);

    LinDiagSendNegResponse_CMockExpect(239, 0);

    ApplLinDiagReadDataById();

    UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT16)((5u)), (UNITY_INT)(UNITY_UINT16)((g_linDiagDataLength)), (
   ((void *)0)
   ), (UNITY_UINT)(245), UNITY_DISPLAY_STYLE_UINT16);
}