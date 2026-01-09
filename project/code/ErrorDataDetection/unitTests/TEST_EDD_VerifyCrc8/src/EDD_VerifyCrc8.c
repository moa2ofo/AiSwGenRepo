#include "EDD_VerifyCrc8.h"
#include "errorDataDetection_priv.h"

/* FUNCTION TO TEST */


EDD_ReturnType EDD_VerifyCrc8(const uint8_t* data, uint8_t length, uint8_t expected_crc, uint8_t* result_out) {
    uint8_t computed;

    if ((data == NULL) || (result_out == NULL))
    {
        return EDD_NULL_PTR;
    }

    if (length == 0u)
    {
        return EDD_INVALID_LENGTH;
    }

    computed = EDD_Crc8ComputeInternal(data, length);

    *result_out = (computed == expected_crc) ? 1u : 0u;

    return EDD_OK;
}
