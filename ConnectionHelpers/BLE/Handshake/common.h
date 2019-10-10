//
// Copyright 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
// These materials are licensed under the Amazon Software License in connection with the Alexa Gadgets Program.
// The Agreement is available at https://aws.amazon.com/asl/.
// See the Agreement for the specific terms and conditions of the Agreement.
// Capitalized terms not defined in this file have the meanings given to them in the Agreement.
//

#ifndef ALEXA_GADGETS_SAMPLE_CODE_COMMON_H
#define ALEXA_GADGETS_SAMPLE_CODE_COMMON_H

#include <stdint.h>

#include "config.h"

#define ADV_DATA_LEN (31U)
#define BLE_AD_TYPE (0x06) // LE General discoverable mode, BT/EDR not supported.
#define CONTROL_PACKET_LENGTH (6)
#define PROTOCOL_IDENTIFIER (0xFE03U)
#define PROTOCOL_VERSION_MAJOR (3U)
#define PROTOCOL_VERSION_MINOR (0U)
#define PROTOCOL_VERSION_PACKET_SIZE (20)
#define SERVICE_UUID (0xFE03)
#define VENDOR_ID (0x0171)

#define ACK_BIT_SHIFT (1U)
#define EXTENDED_LENGTH_BIT_SHIFT (0U)
#define SEQ_NUM_ID_MASK (0x0FU)
#define SEQ_NUM_ID_SHIFT (4U)
#define STREAM_ID_MASK (0x0FU)
#define STREAM_ID_SHIFT (4U)
#define TRANSACTION_ID_MASK (0x0FU)
#define TRANSACTION_ID_SHIFT (0U)
#define TRANSACTION_TYPE_MASK (0x03U)
#define TRANSACTION_TYPE_SHIFT (2U)

typedef enum {
    CONTROL_STREAM = 0,
    OTA_STREAM = 2,
    ALEXA_STREAM = 6
} stream_id_t;

typedef enum {
    TRANSACTION_TYPE_INITIAL = 0,
    TRANSACTION_TYPE_CONTINUE = 1,
    TRANSACTION_TYPE_FINAL = 2,
    TRANSACTION_TYPE_CONTROL = 3
} transaction_type_t;

typedef enum {
    CONTROL_PACKET_RESULT_SUCCESS = 0,
    CONTROL_PACKET_RESULT_FAILURE = 1,
    CONTROL_PACKET_RESULT_UNSUPPORTED = 3
} control_ack_result_t;

typedef uint8_t transaction_id_t;

#endif // ALEXA_GADGETS_SAMPLE_CODE_COMMON_H
