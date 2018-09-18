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

#ifndef _BLUETOOTH_EIR_H
#define _BLUETOOTH_EIR_H


typedef unsigned char uint8_t;
typedef unsigned short uint16_t;

void construct_gadget_eir(uint8_t* buffer, uint8_t* p_len, const char* gadget_name, uint16_t pid, uint16_t vid);

// Gadget UUID 6088D2B3-983A-4EED-9F94-5AD1256816B7
#define GADGET_UUID_PART1 0x256816B7
#define GADGET_UUID_PART2 0x9F945AD1
#define GADGET_UUID_PART3 0x983A4EED
#define GADGET_UUID_PART4 0x6088D2B3

//EIR type defs
#define EIR_TYPE_FULL_NAME 0x09
#define EIR_TYPE_FULL_UUID 0x02
#define EIR_TYPE_GLOBAL_UUID 0x06
#define EIR_TYPE_MANU_DATA 0xff
#define EIR_NUM_PAGED_SRVC 1
#define EIR_PID_SIZE 2
#define EIR_VID_SIZE 2
#define EIR_UUID_SIZE 4

#define GADGET_UUID_32 0xFE151510
#define AMAZON_SIG_VID 0x0171
#endif
