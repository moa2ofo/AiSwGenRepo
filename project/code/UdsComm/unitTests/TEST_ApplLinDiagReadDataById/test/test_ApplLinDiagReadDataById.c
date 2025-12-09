#include "unity.h"
#include "ApplLinDiagReadDataById.h"
#include "mock_diagnostic_cfg.h"
#include "mock_diagnostic_priv.h"
#include <string.h>

#define MOCK_DID_F308_SIZE 4

/* ============================================================================
 * Callback di default (successo)
 * ============================================================================ */
void CurrentNad_Callback(uint8 currentNad, Std_ReturnType* result, int cmock_num_calls)
{
    (void)cmock_num_calls;

    if (currentNad == 0u)
    {
        *result = E_OK;
    }
    else
    {
        *result = E_NOT_OK;
    }
}

void MsgDataLength_Callback(uint16_t dataLength, Std_ReturnType* result, int cmock_num_calls)
{
    (void)dataLength;
    (void)cmock_num_calls;
    *result = E_OK;
}

Std_ReturnType getHandlersForReadDataById_Callback(uint8 l_errCode_u8,
                                                   uint16 l_did_cu16,
                                                   uint8 *l_diagBufSize_u8,
                                                   Std_ReturnType *l_didSupported_,
                                                   uint8 *l_diagBuf_pu8,
                                                   int cmock_num_calls)
{
    Std_ReturnType l_result_ = E_OK;
    (void)l_errCode_u8;
    (void)l_diagBuf_pu8;
    (void)cmock_num_calls;

    switch (l_did_cu16)
    {
        /* IS_OVERVOLT_FLAG: DID supportato */
        case 0xF308:
            *l_diagBufSize_u8 = MOCK_DID_F308_SIZE;
            break;

        default:
            *l_didSupported_  = E_NOT_OK;
            l_result_ = E_NOT_OK;
            /* l_errCode_u8 è passato per valore, la modifica qui non esce */
            break;
    }

    return l_result_;
}

/* ============================================================================
 * Callback alternative (percorsi di errore)
 * ============================================================================ */
static void CurrentNad_Fail_Callback(uint8 currentNad, Std_ReturnType* result, int cmock_num_calls)
{
    (void)currentNad;
    (void)cmock_num_calls;
    *result = E_NOT_OK;   /* qualunque NAD => fallisce */
}

static void MsgDataLength_Fail_Callback(uint16_t dataLength, Std_ReturnType* result, int cmock_num_calls)
{
    (void)dataLength;
    (void)cmock_num_calls;
    *result = E_NOT_OK;   /* controllo lunghezza fallisce */
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

    return E_NOT_OK;      /* handler fallisce a prescindere dal DID */
}

/* ============================================================================
 * Test setup e teardown
 * ============================================================================ */
void setUp(void)
{
    /* Callback di default (successo) */
    checkCurrentNad_StubWithCallback(CurrentNad_Callback);
    checkMsgDataLength_StubWithCallback(MsgDataLength_Callback);
    getHandlersForReadDataById_StubWithCallback(getHandlersForReadDataById_Callback);

    /* Inizializza i buffer globali prima di ogni test */
    extern uint8_t pbLinDiagBuffer[32];
    extern uint16_t g_linDiagDataLength;

    memset(pbLinDiagBuffer, 0, sizeof(pbLinDiagBuffer));
    g_linDiagDataLength = 0;
}

void tearDown(void)
{
}

/* ============================================================================
 * TEST 1: DID non supportato -> risposta negativa, lunghezza invariata
 * ============================================================================ */
void test_ApplLinDiagReadDataById_DidNotSupported(void)
{
    extern uint8_t pbLinDiagBuffer[32];
    extern uint16_t g_linDiagDataLength;

    /* DID NON supportato dal callback di default (getHandlersForReadDataById_Callback) */
    pbLinDiagBuffer[1] = 0x12u;
    pbLinDiagBuffer[2] = 0x34u;

    g_linDiagDataLength = 0u;

    /* Ci aspettiamo una risposta negativa (codice errore non gestito qui) */
    LinDiagSendNegResponse_Expect(0);

    /* Esecuzione */
    ApplLinDiagReadDataById();

    /* Verifica: la lunghezza rimane invariata */
    TEST_ASSERT_EQUAL_UINT16(0u, g_linDiagDataLength);
}

/* ============================================================================
 * TEST 2: DID supportato -> risposta positiva, lunghezza aggiornata
 * ============================================================================ */
void test_ApplLinDiagReadDataById_SuccessfulExecution_DidSupported(void)
{
    extern uint8_t pbLinDiagBuffer[32];
    extern uint16_t g_linDiagDataLength;

    /* DID 0xF308 supportato dal callback di default */
    pbLinDiagBuffer[1] = 0xF3u;
    pbLinDiagBuffer[2] = 0x08u;

    g_linDiagDataLength = 0u;

    /* Tutti i check OK + handler OK => risposta positiva */
    LinDiagSendPosResponse_Expect();

    /* Esecuzione */
    ApplLinDiagReadDataById();

    /* Verifica: lunghezza = dimensione dati + 2 (per il DID) */
    TEST_ASSERT_EQUAL_UINT16(MOCK_DID_F308_SIZE + 2u, g_linDiagDataLength);
}

/* ============================================================================
 * TEST 3: NAD errato -> risposta negativa, non arriva al handler
 * ============================================================================ */
void test_ApplLinDiagReadDataById_WrongNad_Fails(void)
{
    extern uint8_t pbLinDiagBuffer[32];
    extern uint16_t g_linDiagDataLength;

    /* DID valido, ma NAD fallisce: non deve importare il DID */
    pbLinDiagBuffer[1] = 0xF3u;
    pbLinDiagBuffer[2] = 0x08u;

    g_linDiagDataLength = 0u;

    /* Forzo fallimento NAD per questo test */
    checkCurrentNad_StubWithCallback(CurrentNad_Fail_Callback);

    /* Mi aspetto una risposta negativa (il codice esatto qui non interessa) */
    LinDiagSendNegResponse_Expect(0);

    /* Esecuzione */
    ApplLinDiagReadDataById();

    /* Verifica: lunghezza invariata */
    TEST_ASSERT_EQUAL_UINT16(0u, g_linDiagDataLength);
}

/* ============================================================================
 * TEST 4: MsgDataLength fallisce -> risposta negativa, handler non chiamato
 * ============================================================================ */
void test_ApplLinDiagReadDataById_MsgDataLength_Fails(void)
{
    extern uint8_t pbLinDiagBuffer[32];
    extern uint16_t g_linDiagDataLength;

    /* DID valido, ma fallisce il controllo lunghezza */
    pbLinDiagBuffer[1] = 0xF3u;
    pbLinDiagBuffer[2] = 0x08u;

    g_linDiagDataLength = 10u; /* valore non zero per verificare che resti invariato */

    /* NAD OK, ma controllo lunghezza fallisce */
    checkCurrentNad_StubWithCallback(CurrentNad_Callback);
    checkMsgDataLength_StubWithCallback(MsgDataLength_Fail_Callback);

    LinDiagSendNegResponse_Expect(0);

    /* Esecuzione */
    ApplLinDiagReadDataById();

    /* Verifica: lunghezza invariata */
    TEST_ASSERT_EQUAL_UINT16(10u, g_linDiagDataLength);
}

/* ============================================================================
 * TEST 5: Handler fallisce -> risposta negativa, lunghezza invariata
 * ============================================================================ */
void test_ApplLinDiagReadDataById_HandlerFails(void)
{
    extern uint8_t pbLinDiagBuffer[32];
    extern uint16_t g_linDiagDataLength;

    /* DID che normalmente sarebbe anche valido, ma qui forziamo il fallimento del handler */
    pbLinDiagBuffer[1] = 0xF3u;
    pbLinDiagBuffer[2] = 0x08u;

    g_linDiagDataLength = 5u;

    /* NAD OK + MsgDataLength OK, ma handler fallisce */
    checkCurrentNad_StubWithCallback(CurrentNad_Callback);
    checkMsgDataLength_StubWithCallback(MsgDataLength_Callback);
    getHandlersForReadDataById_StubWithCallback(getHandlersForReadDataById_Fail_Callback);

    LinDiagSendNegResponse_Expect(0);

    /* Esecuzione */
    ApplLinDiagReadDataById();

    /* Verifica: lunghezza invariata */
    TEST_ASSERT_EQUAL_UINT16(5u, g_linDiagDataLength);
}
