//
// Copyright 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
// These materials are licensed under the Amazon Software License in connection with the Alexa Gadgets Program.
// The Agreement is available at https://aws.amazon.com/asl/.
// See the Agreement for the specific terms and conditions of the Agreement.
// Capitalized terms not defined in this file have the meanings given to them in the Agreement.
//

#ifndef ALEXA_GADGETS_SAMPLE_CODE_RX_H
#define ALEXA_GADGETS_SAMPLE_CODE_RX_H

#include "helpers.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    ROLE_ECHO,
    ROLE_GADGET
} role_t;

packet_list_t *receivePackets(role_t role, packet_list_t const *list);

#ifdef __cplusplus
}
#endif

#endif //ALEXA_GADGETS_SAMPLE_CODE_RX_H
