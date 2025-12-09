#include "unity.h"
#include "ApplLinDiagReadDataById.h"
#include "mock_diagnostic_cfg.h"
#include "mock_diagnostic_priv.h"
#include <string.h>

#define MOCK_DID_F308_SIZE 4

void CurrentNad_Callback(uint8 currentNad, Std_ReturnType* result, int cmock_num_calls)
{
    *result = E_OK;
}

void MsgDataLength_Callback(uint16_t dataLength, Std_ReturnType* result, int cmock_num_calls)
{
    *result = E_OK;
}

Std_ReturnType getHandlersForReadDataById_Callback(uint8 l_errCode_u8, uint16 l_did_cu16,  uint8 *l_diagBufSize_u8
    , Std_ReturnType *l_didSupported_,  uint8 *l_diagBuf_pu8, int cmock_num_calls)
{
    Std_ReturnType l_result_ = E_OK;
    (void)l_diagBufSize_u8;
    
    switch (l_did_cu16)
    {
    /* IS_OVERVOLT_FLAG */
    case 0xF308:
        *l_diagBufSize_u8 = MOCK_DID_F308_SIZE;
        break;

    default:
        *l_didSupported_  = E_NOT_OK;
        l_result_ = E_NOT_OK;
        l_errCode_u8 = kLinDiagNrcRequestOutOfRange;
        break;
    }
    
    return l_result_;
}


/* Test setup and teardown */
void setUp(void)
{
    checkCurrentNad_StubWithCallback(CurrentNad_Callback);
    checkMsgDataLength_StubWithCallback(MsgDataLength_Callback);
    getHandlersForReadDataById_StubWithCallback(getHandlersForReadDataById_Callback);
    /* Initialize global buffers before each test */
    extern uint8_t pbLinDiagBuffer[32];
    extern uint16_t g_linDiagDataLength;
    
    memset(pbLinDiagBuffer, 0, sizeof(pbLinDiagBuffer));
    g_linDiagDataLength = 0;
}

void tearDown(void)
{
}

/* ============================================================================
 * TEST GROUP: ApplLinDiagReadDataById - Successful Execution
 * ============================================================================ */
void test_ApplLinDiagReadDataById_DidNotSupported(void)
{
    extern uint8_t pbLinDiagBuffer[32];
    extern uint16_t g_linDiagDataLength;

    /* Setup: Configure buffer with valid DID */
    pbLinDiagBuffer[1] = 0x12;  /* DID high byte */
    pbLinDiagBuffer[2] = 0x34;  /* DID low byte */
    pbLinDiagBuffer[3] = 0xAA;  /* Diagnostic data start */
    pbLinDiagBuffer[4] = 0xBB;
    pbLinDiagBuffer[5] = 0xCC;

    const uint16_t l_did_cu16 = ((uint16_t)(pbLinDiagBuffer[1] << 8) & 0xFF00) |
                    ((uint16_t)pbLinDiagBuffer[2] & 0x00FF);
    Std_ReturnType l_result_ = E_OK;
    uint8_t l_errCode_u8 = 0;
    uint8_t * const l_diagBuf_pu8 = &pbLinDiagBuffer[3];
    uint8_t l_diagBufSize_u8 = 0;
    Std_ReturnType l_didSupported_ = E_OK;

    CurrentNad_Callback(0, &l_result_, 1);
    MsgDataLength_Callback(0, &l_result_, 1);   
    getHandlersForReadDataById_Callback(l_errCode_u8, l_did_cu16,  &l_diagBufSize_u8, &l_didSupported_, 
        l_diagBuf_pu8, 1);


    //LinDiagSendPosResponse_Expect();
    LinDiagSendNegResponse_Expect(l_errCode_u8);

    /* Execute */
    ApplLinDiagReadDataById();

    /* Verify - data length should be updated */
    TEST_ASSERT_EQUAL(g_linDiagDataLength , 0);
}



void test_ApplLinDiagReadDataById_SuccessfulExecution_DidSupported(void)
{
    extern uint8_t pbLinDiagBuffer[32];
    extern uint16_t g_linDiagDataLength;

    /* Setup: Configure buffer with valid DID */
    pbLinDiagBuffer[1] = 0xF3;  /* DID high byte */
    pbLinDiagBuffer[2] = 0x08;  /* DID low byte */
    pbLinDiagBuffer[3] = 0xAA;  /* Diagnostic data start */
    pbLinDiagBuffer[4] = 0xBB;
    pbLinDiagBuffer[5] = 0xCC;

    const uint16_t l_did_cu16 = ((uint16_t)(pbLinDiagBuffer[1] << 8) & 0xFF00) |
                    ((uint16_t)pbLinDiagBuffer[2] & 0x00FF);
    Std_ReturnType l_result_ = E_OK;
    uint8_t l_errCode_u8 = 0;
    uint8_t * const l_diagBuf_pu8 = &pbLinDiagBuffer[3];
    uint8_t l_diagBufSize_u8 = 0;
    Std_ReturnType l_didSupported_ = E_OK;

    CurrentNad_Callback(0, &l_result_, 1);
    MsgDataLength_Callback(0, &l_result_, 1);   
    getHandlersForReadDataById_Callback(l_errCode_u8, l_did_cu16,  &l_diagBufSize_u8, &l_didSupported_, 
        l_diagBuf_pu8, 1);


    //LinDiagSendPosResponse_Expect();
    LinDiagSendPosResponse_Expect();

    /* Execute */
    ApplLinDiagReadDataById();

    /* Verify - data length should be updated */
    TEST_ASSERT_EQUAL(g_linDiagDataLength , MOCK_DID_F308_SIZE + 2);
}




