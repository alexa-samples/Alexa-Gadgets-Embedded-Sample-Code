//
// Copyright 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
// These materials are licensed under the Amazon Software License in connection with the Alexa Gadgets Program.
// The Agreement is available at https://aws.amazon.com/asl/.
// See the Agreement for the specific terms and conditions of the Agreement.
// Capitalized terms not defined in this file have the meanings given to them in the Agreement.
//

#ifndef _GADGET_PACKET_H
#define _GADGET_PACKET_H

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef uint8_t bool;

#define SOP 0xF0
#define EOP 0xF1
#define ESP 0xF2

#define TRUE 1
#define FALSE 0

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
