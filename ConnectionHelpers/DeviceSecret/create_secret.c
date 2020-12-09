//
// Copyright 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
// These materials are licensed under the Amazon Software License in connection with the Alexa Gadgets Program.
// The Agreement is available at https://aws.amazon.com/asl/.
// See the Agreement for the specific terms and conditions of the Agreement.
// Capitalized terms not defined in this file have the meanings given to them in the Agreement.
//

#include "stdio.h"
#include "string.h"
#include "mbedtls/sha256.h"

typedef unsigned char uint8_t;
static void convert_to_utf8(char *dst, uint8_t *src, int size);
static int sha256(uint8_t* source, uint8_t len, uint8_t* dest);

int main()
{
    //Device Token Recevied from the dev-portal at gadget registration
    char* device_secret = "76AB4E896EE2A081BCAEA241058A2EEC2D016C250CF355451D7A7009A560B3F2";

    //Gadget ID is the endpointId (device serial number) that the gadget sends the Echo device in the Alexa.Discovery.Discover.Response event
    char* gadgetId = "G0A0B0C0D0";

    //The concatenated result of gadgetID and the deviceType
    char secret[200] = {0};
    strcat(secret, gadgetId);
    strcat(secret, device_secret);

    printf("Input for Device Token SHA256 calculation: \n%s, \nlength = %lu\n", secret, strlen(secret));

    //Destination buffer for device Token
    uint8_t token[32];

    //Destination buffer for utf-8 representation of the 32 byte sha (32*2 + 1 byte for escape)
    char dst[65];

    int sha_result = sha256((uint8_t*)secret, strlen(secret), token);

    if (sha_result < 0)
    {
        printf("Error calculating sha for device Token\n");
        return sha_result;
    }

    convert_to_utf8(dst, token, 32);
    printf("device token:%s\n", dst);
}


static int sha256(uint8_t* source, uint8_t len, uint8_t* dest)
{
    mbedtls_sha256_context ctx;
    
    mbedtls_sha256_init(&ctx);

    if(mbedtls_sha256_starts_ret (&ctx, 0) != 0) {
        printf("Error initializing sha-256 context\n");
        return -1;
    }

    if(mbedtls_sha256_update_ret (&ctx, (uint8_t *) source, len) != 0){
        printf("Error updating sha-256 context\n");
        return -1;
    }

    if(mbedtls_sha256_finish_ret (&ctx, (uint8_t *) dest) != 0) {
        printf("Error finalizing and calculating sha-256 context\n");
        return -1;
    }
    return 0;
}

static void convert_to_utf8(char *dst, uint8_t *src, int size)
{
    uint8_t index;
    for (index = 0; index < size; index++)
    {
        printf("0x%02x ", src[index]);
        //Upper nibble
        uint8_t value = (src[index] >> 4) & 0x0f;
        if (value > 9)
        {
            *dst++ = 'a' + value - 10;
        }
        else
        {
            *dst++ = '0' + value;
        }

        //Lower nibble
        value = src[index] & 0x0f;

        if (value > 9)
        {
            *dst++ = 'a' + value - 10;
        }
        else
        {
            *dst++ = '0' + value;
        }
    }
    *dst++ = 0; //null terminate
    printf("\n");
}

