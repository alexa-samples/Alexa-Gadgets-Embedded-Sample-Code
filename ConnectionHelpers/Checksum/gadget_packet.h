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

#ifndef _GADGET_PACKET_H
#define _GADGET_PACKET_H


typedef unsigned char uint8_t;
typedef unsigned short uint16_t;

#define SOP 0xF0
#define EOP 0xF1
#define ESP 0xF2


typedef struct
{
    uint8_t sof;
    uint8_t cmd;
    uint8_t err;
    uint8_t seqId;
} __attribute__((packed)) spp_packet_directive_header;

typedef struct
{
    uint16_t checksum;
    uint8_t eof;
}__attribute__((packed)) spp_packet_directive_trailer;

typedef struct
{
    spp_packet_directive_header header;
    uint8_t data[];
}__attribute__((packed))spp_directive_response_packet;

#endif
