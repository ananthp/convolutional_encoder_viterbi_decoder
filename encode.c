#include "fec.h"
#define CUR_FILE "ENCODE.C"

size_t encode(FEC _fec, uint8_t *msg, size_t sz_msg, uint8_t *enc_msg)
{
    size_t sz_enc = 0;
    if(sz_msg <= 0)
    {
        printf("\n[%s][%d]: invalid input size(%d) to encode\n", CUR_FILE, __LINE__, sz_msg);
        return 0;
    }

    unsigned int state_reg = 0;
    unsigned int full_state = pow(2,_fec.cl-1) - 1;
    printf("\nfull state : %x", full_state);

    for(size_t i = 0; i < sz_msg; i++)
    {
        state_reg = state_reg & full_state;
        state_reg = state_reg | (msg[i] << (_fec.cl-1));

        unsigned int ones_cnt = 0;
        uint8_t G0 = 0, G1 = 0;
        ones_cnt = __builtin_popcount(state_reg & _fec.msb);
        G1 = (bool)(ones_cnt % 2);
        ones_cnt = __builtin_popcount(state_reg & _fec.lsb);
        G0 = (bool)(ones_cnt % 2);

        enc_msg[sz_enc++] = G1;
        enc_msg[sz_enc++] = G0;

        state_reg = state_reg >> 1;
        // printf("\nstate : %d, msg : %d", state_reg, msg[i]);
        // printf("\nmsb = %d, lsb = %d", enc_msg[sz_enc-2], enc_msg[sz_enc-1]);

    }

    return sz_enc;
}
