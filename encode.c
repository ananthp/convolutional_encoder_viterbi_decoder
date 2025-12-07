#include "fec.h"
#include "colors.h"
#include "colordlg.h"
#define CUR_FILE "ENCODE.C"

size_t encode(FEC _fec, uint8_t *msg, size_t sz_msg, uint8_t *enc_msg)
{
    size_t sz_enc = 0;
    if(sz_msg <= 0)
    {
        printf("\n[%s][%d]: invalid input size(%d) to encode\n", CUR_FILE, __LINE__, sz_msg);
        return 0;
    }
    if(sz_msg % 8 != 0)
    {
        printf( REDB "raw msg size is not exact bytes. continuing anyway..!" reset "\n");
    }

    unsigned int state_reg = 0;
    unsigned int full_state = pow(2,_fec.cl-1) - 1;

    if(_fec.enable_fulltail_biting)
    {
        uint8_t temp[20] = {0};
        for(size_t i = 0; i < (_fec.cl-1); i++)
        {
            temp[i]=msg[i];
            state_reg = (state_reg & full_state);
            state_reg = state_reg | ((msg[i])<< (_fec.cl-1));
            state_reg = state_reg>>1;
        }

        for(size_t i = (_fec.cl-1); i < sz_msg; i++)
        {
            state_reg = state_reg & full_state;
            state_reg = state_reg | (msg[i] << (_fec.cl-1));

            unsigned int ones_cnt = 0;
            uint8_t G0 = 0, G1 = 0;
            ones_cnt = __builtin_popcount(state_reg & _fec.msb);
            G1 = ones_cnt % 2;
            ones_cnt = __builtin_popcount(state_reg & _fec.lsb);
            G0 = ones_cnt % 2;

            enc_msg[sz_enc++] = G1;
            enc_msg[sz_enc++] = G0;

            state_reg = state_reg >> 1;
        }

        for(size_t i = 0; i < (_fec.cl-1); i++)
        {
            state_reg = state_reg & full_state;
            state_reg = state_reg | (temp[i] << (_fec.cl-1));

            unsigned int ones_cnt = 0;
            uint8_t G0 = 0, G1 = 0;
            ones_cnt = __builtin_popcount(state_reg & _fec.msb);
            G1 = ones_cnt % 2;
            ones_cnt = __builtin_popcount(state_reg & _fec.lsb);
            G0 = ones_cnt % 2;

            enc_msg[sz_enc++] = G1;
            enc_msg[sz_enc++] = G0;

            state_reg = state_reg >> 1;
        }
    }
    else
    {
        for(size_t i = 0; i < sz_msg; i++)
        {
            state_reg = state_reg & full_state;
            state_reg = state_reg | (msg[i] << (_fec.cl-1));

            unsigned int ones_cnt = 0;
            uint8_t G0 = 0, G1 = 0;
            ones_cnt = __builtin_popcount(state_reg & _fec.msb);
            G1 = ones_cnt % 2;
            ones_cnt = __builtin_popcount(state_reg & _fec.lsb);
            G0 = ones_cnt % 2;

            enc_msg[sz_enc++] = G1;
            enc_msg[sz_enc++] = G0;

            state_reg = state_reg >> 1;
            // printf("\nstate reg = %d", state_reg & full_state);
        }
    }

    print_array(enc_msg, sz_enc, "encoded msg");    

    if(_fec.sz_puncturing_pattern > 0)
    {
        // typeof(enc_msg) *tmp = calloc(sz_enc, sizeof(enc_msg[0]));
        uint8_t *tmp = calloc(sz_enc, sizeof(enc_msg[0]));
        size_t sz_out = 0;
        for(int i = 0; i < sz_enc; i++)
        {
            bool puncture = !(_fec.puncturing_pattern[(i % _fec.sz_puncturing_pattern)]);

            if(!puncture)
            {
                tmp[sz_out] = enc_msg[i];
                enc_msg[sz_out] =  tmp[sz_out];
                sz_out++;
            }
        }
        
        // memcpy(enc_msg, tmp, sz_out);
        
        sz_enc = sz_out;
        
        free(tmp);
    }

    print_array(enc_msg, sz_enc, "punctured msg");
    return sz_enc;
}
