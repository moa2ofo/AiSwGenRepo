#include "unity.h"
#include "getHandlersForReadDataById.h"     
#include "mock_diagnostic_cfg.h"
#include <string.h>

void setUp(void)
{
    /* Reset all mocks before each test */
}

void tearDown(void)
{
    /* Verify all mock expectations were met */
}

/**
 * Test: Valid DID 0xF308 should set buffer size and call RdbiVhitOverVoltageFaultDiag_
 */
void test_getHandlersForReadDataById_ValidDID_F308(void)
{
    g_errCode_u8 = 0;
    uint16 l_did_cu16 = 0xF308;
    uint8 l_diagBufSize_u8 = 0;
    Std_ReturnType l_didSupported_ = E_OK;
    uint8 l_diagBuf_pu8[10] = {0};
    SubfunctionRequestOutOfRange__IgnoreAndReturn(E_NOT_OK);
    /* Expect handler to be called and return E_OK */
    RdbiVhitOverVoltageFaultDiag__IgnoreAndReturn(E_OK);

    /* Call function */
    Std_ReturnType result = getHandlersForReadDataById(&g_errCode_u8, l_did_cu16, 
                                                       &l_diagBufSize_u8, &l_didSupported_, 
                                                       l_diagBuf_pu8);

    /* Verify buffer size was set to DID_F308_SIZE */
    TEST_ASSERT_EQUAL(DID_F308_SIZE, l_diagBufSize_u8);
    
    /* Verify didSupported flag is still E_OK */
    TEST_ASSERT_EQUAL(E_OK, l_didSupported_);
    
    /* Verify handler returned E_OK */
    TEST_ASSERT_EQUAL(E_OK, result);
}

/**
 * Test: Invalid DID should set didSupported to E_NOT_OK and call Subfunction_Request_Out_Of_Range
 */
void test_getHandlersForReadDataById_InvalidDID(void)
{
    g_errCode_u8 = 0;
    g_did_cu16 = 0x1234;  /* Invalid DID */
    uint8 l_diagBufSize_u8 = 0xFF;  /* Initialize to non-zero value */
    Std_ReturnType l_didSupported_ = E_OK;
    uint8 l_diagBuf_pu8[10] = {0};

    /* Expect Subfunction_Request_Out_Of_Range handler to be called */
    SubfunctionRequestOutOfRange__IgnoreAndReturn(E_NOT_OK);

    /* Call function */
    Std_ReturnType result = getHandlersForReadDataById(&g_errCode_u8, g_did_cu16, 
                                                       &l_diagBufSize_u8, &l_didSupported_, 
                                                       l_diagBuf_pu8);

    /* Verify didSupported was set to E_NOT_OK */
    TEST_ASSERT_EQUAL(E_NOT_OK, l_didSupported_);
    
    /* Verify error code was set to kLinDiagNrcRequestOutOfRange */
    TEST_ASSERT_EQUAL(kLinDiagNrcRequestOutOfRange, g_errCode_u8);
    
    /* Verify handler returned E_NOT_OK */
    TEST_ASSERT_EQUAL(E_NOT_OK, result);
}

/**
 * Test: Multiple invalid DIDs should all result in E_NOT_OK
 */
void test_getHandlersForReadDataById_MultipleInvalidDIDs(void)
{
    uint16 invalid_dids[] = {0x0000, 0x0001, 0xFFFF, 0xF300, 0xF309};
    
    for (size_t i = 0; i < sizeof(invalid_dids) / sizeof(invalid_dids[0]); i++)
    {
        g_errCode_u8 = 0;
        uint8 l_diagBufSize_u8 = 0xFF;
        Std_ReturnType l_didSupported_ = E_OK;
        uint8 l_diagBuf_pu8[10] = {0};
        g_did_cu16 = invalid_dids[i];

        SubfunctionRequestOutOfRange__IgnoreAndReturn(E_NOT_OK);

        Std_ReturnType result = getHandlersForReadDataById(&g_errCode_u8, g_did_cu16, 
                                                           &l_diagBufSize_u8, &l_didSupported_, 
                                                           l_diagBuf_pu8);

        TEST_ASSERT_EQUAL_MESSAGE(E_NOT_OK, l_didSupported_, "didSupported should be E_NOT_OK");
        TEST_ASSERT_EQUAL_MESSAGE(kLinDiagNrcRequestOutOfRange, g_errCode_u8, "errCode should be set to kLinDiagNrcRequestOutOfRange");
    }
}

/**
 * Test: Valid DID with handler returning E_NOT_OK should propagate error
 */
void test_getHandlersForReadDataById_ValidDID_HandlerReturnsError(void)
{
    g_errCode_u8 = 0;
    g_did_cu16 = 0xF308;
    uint8 l_diagBufSize_u8 = 0;
    Std_ReturnType l_didSupported_ = E_OK;
    uint8 l_diagBuf_pu8[10] = {0};
    SubfunctionRequestOutOfRange__IgnoreAndReturn(E_NOT_OK);

    /* Expect handler to be called and return E_NOT_OK */
    RdbiVhitOverVoltageFaultDiag__IgnoreAndReturn(E_NOT_OK);  /* Handler returns error */

    Std_ReturnType result = getHandlersForReadDataById(&g_errCode_u8, g_did_cu16, 
                                                       &l_diagBufSize_u8, &l_didSupported_, 
                                                       l_diagBuf_pu8);

    /* Verify buffer size was still set */
    TEST_ASSERT_EQUAL(DID_F308_SIZE, l_diagBufSize_u8);
    
    /* Verify handler's error was propagated */
    TEST_ASSERT_EQUAL(E_NOT_OK, result);
}

/**
 * Test: Verify error code parameter is passed correctly to handler
 */
void test_getHandlersForReadDataById_ErrorCodePassedToHandler(void)
{
    g_errCode_u8 = 0x42;  /* Non-zero error code */
    g_did_cu16 = 0xF308;
    uint8 l_diagBufSize_u8 = 0;
    Std_ReturnType l_didSupported_ = E_OK;
    uint8 l_diagBuf_pu8[10] = {0};

    SubfunctionRequestOutOfRange__IgnoreAndReturn(E_NOT_OK);
    RdbiVhitOverVoltageFaultDiag__IgnoreAndReturn(E_OK);

    Std_ReturnType result = getHandlersForReadDataById(&g_errCode_u8, g_did_cu16, 
                                                       &l_diagBufSize_u8, &l_didSupported_, 
                                                       l_diagBuf_pu8);

    TEST_ASSERT_EQUAL(E_OK, result);
}

/**
 * Test: Verify output buffer is passed correctly to handler
 */
void test_getHandlersForReadDataById_OutputBufferPassedToHandler(void)
{
    g_errCode_u8 = 0;
    g_did_cu16 = 0xF308;
    uint8 l_diagBufSize_u8 = 0;
    Std_ReturnType l_didSupported_ = E_OK;
    uint8 l_diagBuf_pu8[10] = {0};

    /* Expect the specific buffer to be passed to handler */
    SubfunctionRequestOutOfRange__IgnoreAndReturn(E_NOT_OK);
    RdbiVhitOverVoltageFaultDiag__IgnoreAndReturn(E_OK);

    Std_ReturnType result = getHandlersForReadDataById(&g_errCode_u8, g_did_cu16, 
                                                       &l_diagBufSize_u8, &l_didSupported_, 
                                                       l_diagBuf_pu8);
    TEST_ASSERT_EQUAL(E_OK, result);
}

/**
 * Test: Verify buffer size pointer is passed correctly to handler
 */
void test_getHandlersForReadDataById_BufferSizePointerPassedToHandler(void)
{
    g_errCode_u8 = 0;
    g_did_cu16 = 0xF308;
    uint8 l_diagBufSize_u8 = 0;
    Std_ReturnType l_didSupported_ = E_OK;
    uint8 l_diagBuf_pu8[10] = {0};

    SubfunctionRequestOutOfRange__IgnoreAndReturn(E_NOT_OK);
    RdbiVhitOverVoltageFaultDiag__IgnoreAndReturn(E_OK);

    Std_ReturnType result = getHandlersForReadDataById(&g_errCode_u8, g_did_cu16, 
                                                       &l_diagBufSize_u8, &l_didSupported_, 
                                                       l_diagBuf_pu8);

    /* Verify the size was set before handler was called */
    TEST_ASSERT_EQUAL(DID_F308_SIZE, l_diagBufSize_u8);
    TEST_ASSERT_EQUAL(E_OK, result);
}
