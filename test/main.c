
#include "fctParse.h"
#include "ecParse.h"
#include "ccParse.h"
#include "fctUtils.h"
#include <memory.h>
#include <stdio.h>


const int8_t fcthex[] =   "02016253dfaa7301010087db406ff65cb9dd72a1e99bcd51da5e03b0ccafc237dbf1318a8d7438e22371c892d6868d20f02894db071e2eb38fdc56c697caaeba7dc19bddae2c6e7084cc3120d667b49f";//0155d679fb5b160f00cf5e5d34e5b1855e67e76317ebe35816cb34c86e25803ea90ac83d4011aebf93ee29e9a4b6860a1f023d84770341ae8ab3c2ac6cd9192edc42eb3ac6637badf46536545aebf8f083762bd4ac79ffb378726433020d149f06";
const int8_t echex[] =    "00016227acddfe57cf6740c4f30ae39d71f75710fb4ea9c843d5c01755329a42ccab52034e1f7901d5b8efdb52a15c4007d341eb1193903a021ed7aaa9a3cf4234c32ef8a213de00";

const int8_t entry[] = "000165e2d19bbebe548e2189e84824a90aa900f2d2ede8ff5d65383be3bece06e85f430a07fb1700";

void hextobin(unsigned char *v, unsigned char *s, size_t n) {

    int i;
    char _t[3];

    unsigned char *p = s;

    for (i=0; i<n; ++i) {

        memcpy(_t, p, 2);

        _t[2] = '\0';

        v[i] = (int)strtol(_t, NULL, 16);

        p += 2;

    }

}
int jsmnmain();

void test_varint_decode()
{

    uint64_t test_varint[] = { 0x8FFFFFFF7F, 0x8102 };
    uint64_t test_result[] = { 0xFFFFFFFF, 0x82 };

    for (int i = 0; i < sizeof(test_varint)/sizeof(uint64_t); ++i )
    {
        uint8_t buf[256];
        sprintf(buf, "%lx", test_varint[i] );

        uint8_t binbuf[256];
        uint32_t off = 0;

        int len = sizeof(buf)/2 + sizeof(buf)%2;
        hextobin(binbuf, buf, len);
        uint64_t val = varint_decode(binbuf,len, &off);

        if ( val != test_result[i] )
        {
            fprintf(stderr, "varint_decode test failed %ld != %ld\n", val, test_result[i]);
        }
        else
        {

            fprintf(stderr, "varint_decode test passed %ld == %ld\n", val, test_result[i]);
        }

    }

}



int main ( int argc, char argv[] )
{
    test_varint_decode();

    parserStatus_e ret;
    uint8_t data[MAX_TXN_SIZE];
    uint32_t length;
    txContent_t content;

    length = strlen(fcthex);
    if ( length*2 > MAX_TXN_SIZE )
    {
        printf("[ERROR]: transaction length too long.\n");
        return -1;
    }

    bzero(data, sizeof(data));
    hextobin(data, fcthex, length/2);


    ret = parseTx(data,length, &content);

    char out[512];
    getFctAddressStringFromRCDHash(content.inputs[0].addr.rcdhash,out,PUBLIC_OFFSET_FCT);
    fprintf(stderr, "%s\n", out);
    fprintf(stderr, "%ld\n", content.inputs[0].value);

    getFctAddressStringFromRCDHash(content.outputs[0].addr.rcdhash,out,PUBLIC_OFFSET_FCT);
    fprintf(stderr, "%s\n", out);
    fprintf(stderr, "%ld\n", content.outputs[0].value);


    char maxFee[256];
    fct_print_amount(content.fees, maxFee, sizeof(maxFee));

    fprintf(stderr, "Fee %s\n",maxFee);

    char fullAmount[256];
    for ( int i = 0; i < content.header.outputcount;++i )
    {
        fct_print_amount(content.outputs[i].value, fullAmount, sizeof(fullAmount));

        fprintf(stderr, "Amount %s\n", fullAmount);
    }


    txEcContent_t eccontent;
    bzero(data,sizeof(data));
    hextobin(data,echex,strlen(echex)/2);
    ret = parseEcTx(data,length, &eccontent);

    if ( ret == 0 )
    {

        cx_ecfp_public_key_t pubkey;
        pubkey.W_len = 32;

        os_memmove(pubkey.W,eccontent.ecpubkey,pubkey.W_len);


        getFctAddressStringFromKey(&pubkey,out,PUBLIC_OFFSET_EC);
        fprintf(stderr, "EC PUB KEY %s\n", out);


        //parse check

        int8_t ectest[] = "d5b8efdb52a15c4007d341eb1193903a021ed7aaa9a3cf4234c32ef8a213de00";

        uint8_t n = strlen(ectest);
        hextobin(data,ectest, strlen(ectest)/2);

        pubkey.W_len = 32;

        os_memmove(pubkey.W,data,pubkey.W_len);
        getFctAddressStringFromKey(&pubkey,out,PUBLIC_OFFSET_EC);
        fprintf(stderr, "Test EC PUB KEY %s\n", out);
    }


    //jsmnmain();
    //hextobin(data,entry, strlen(entry)/2);
    txContent_t fatcontent;
    static const char *JSON_STRING =
        "{\"inputs\":{\"FA22de5NSG2FA2HmMaD4h8qSAZAJyztmmnwgLPghCQKoSekwYYct\":150},\"outputs\":{\"FA3nr5r54AKBZ9SLABS3JyRoGcWMVMTkePW9MECKM8shMg2pMagn\":150}}";

    static const char *fattx =
       "3031353639353334303736888888d027c59579fc47a6fc6c4a5c0409c7c39bc38a86cb5fc00699784937627b22696e70757473223a7b22464132326465354e534732464132486d4d61443468387153415a414a797a746d6d6e77674c50676843514b6f53656b7759596374223a3135307d2c226f757470757473223a7b224641336e7235723534414b425a39534c414253334a79526f4763574d564d546b655057394d45434b4d3873684d6732704d61676e223a3135307d7d";

    bzero(data,sizeof(data));
    hextobin(data,fattx,strlen(fattx)/2);


    int iii = toString("150", 3);

    parseFat0Tx(data, strlen(fattx)/2,&content);


    return 0;
}

