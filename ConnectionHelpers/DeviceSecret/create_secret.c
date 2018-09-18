// Copyright 2018 Amazon.com, Inc. or its affiliates. All Rights Reserved.
//
// Licensed under the Amazon Software License (the "License"). You may not use this file except in
// compliance with the License. A copy of the License is located at
//
//    http://aws.amazon.com/asl/
//
// or in the "license" file accompanying this file. This file is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, express or implied. See the License for the specific
// language governing permissions and limitations under the License.

#include "stdio.h"
#include "string.h"
#include "sha.h"

typedef unsigned char uint8_t;
static void convert_to_utf8(char *dst, uint8_t *src, int size);
int main()
{
    char dst[65];
    char* device_token = "76AB4E896EE2A081BCAEA241058A2EEC2D016C250CF355451D7A7009A560B3F2";
    char* gadgetId = "G0A0B0C0D0";

    char secret[200];
    strcat(secret, gadgetId);
    strcat(secret, device_token);

    printf("%s\n", secret);
    uint8_t token[32];
    sha256sum(token, secret, strlen(secret));

    //uint8_t arr[] = {0xAB, 0x08, 0x9C, 0xd5};
    //char dst2[8];
    convert_to_utf8(dst, token, 32);
    printf("secret:%s\n", dst);
    //SHA256_CTX ctx;
    //sha256sum(secret);
}

static void convert_to_utf8(char *dst, uint8_t *src, int size)
{
    uint8_t index;
    for (index = 0; index < size; index++)
    {
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
}

