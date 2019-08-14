//
// Copyright 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
// These materials are licensed under the Amazon Software License in connection with the Alexa Gadgets Program.
// The Agreement is available at https://aws.amazon.com/asl/.
// See the Agreement for the specific terms and conditions of the Agreement.
// Capitalized terms not defined in this file have the meanings given to them in the Agreement.
//

#include "stdio.h"
#include "string.h"
#include "bluetooth_eir.h"

static void add_gadget_name_to_eir(uint8_t* p_buffer, uint8_t* p_len, const char* name)
{
    printf("%s\n", __FUNCTION__);

    uint8_t* base_ptr = p_buffer + *p_len;
    p_buffer = base_ptr;

    *p_buffer++ = (1 + strlen(name));

    *p_buffer++ = EIR_TYPE_FULL_NAME;

    memcpy(p_buffer, name, strlen(name));
    p_buffer = p_buffer + strlen(name);

    *p_len = *p_len + p_buffer - base_ptr;
}

static void add_gadget_uuid128_to_eir(uint8_t* p_buffer, uint8_t* p_len)
{
    printf("%s\n", __FUNCTION__);

    uint8_t* base_ptr = p_buffer + *p_len;
    p_buffer = base_ptr;

    *p_buffer++ = (EIR_NUM_PAGED_SRVC*16) + 1;

    /** PART1 **/
    *p_buffer++ = EIR_TYPE_GLOBAL_UUID;

    *p_buffer++ = (GADGET_UUID_PART1 & 0xff);

    *p_buffer++ =  (GADGET_UUID_PART1 >> 8) & 0xff;

    *p_buffer++ =  (GADGET_UUID_PART1 >> 16) & 0xff;

    *p_buffer++ =  (GADGET_UUID_PART1 >> 24) & 0xff;

     /** PART2 **/
    *p_buffer++ = GADGET_UUID_PART2 & 0xff;

    *p_buffer++ =  (GADGET_UUID_PART2 >> 8) & 0xff;

    *p_buffer++ =  (GADGET_UUID_PART2 >> 16) & 0xff;

    *p_buffer++ =  (GADGET_UUID_PART2 >> 24) & 0xff;
   
     /** PART3 **/
    *p_buffer++ = GADGET_UUID_PART3 & 0xff;

    *p_buffer++ =  (GADGET_UUID_PART3 >> 8) & 0xff;

    *p_buffer++ =  (GADGET_UUID_PART3 >> 16) & 0xff;

    *p_buffer++ =  (GADGET_UUID_PART3 >> 24) & 0xff;

     /** PART4 **/
    *p_buffer++ = GADGET_UUID_PART4 & 0xff;

    *p_buffer++ =  (GADGET_UUID_PART4 >> 8) & 0xff;

    *p_buffer++ =  (GADGET_UUID_PART4 >> 16) & 0xff;

    *p_buffer++ =  (GADGET_UUID_PART4 >> 24) & 0xff;

    *p_len = *p_len + p_buffer - base_ptr;
}

static void add_gadget_pid_vid_to_eir(uint8_t* p_buffer, uint8_t* p_len, uint16_t pid, uint16_t vid)
{
    printf("%s\n", __FUNCTION__);
    uint8_t* base_ptr = p_buffer + *p_len;
    p_buffer = base_ptr;

    *p_buffer++ = 1 + EIR_PID_SIZE + EIR_VID_SIZE + EIR_VID_SIZE + EIR_UUID_SIZE;

    *p_buffer++ = ((uint8_t)EIR_TYPE_MANU_DATA & (uint8_t)0xff);

    /** vid **/
    *p_buffer++ = (vid & 0xff);

    *p_buffer++ = (vid >> 8) & 0xff;

    /** pid **/
    *p_buffer++ =  pid & 0xff;

    *p_buffer++ =  (pid >> 8) & 0xff;

    //AMAZON VID
    *p_buffer++ = AMAZON_SIG_VID & 0xff;

    *p_buffer++ =  (AMAZON_SIG_VID >> 8) & 0xff;

    // Copy Gadget UUID
    *p_buffer++ =  GADGET_UUID_32 & 0xff;
    *p_buffer++ =  (GADGET_UUID_32 >> 8) & 0xff;
    *p_buffer++ =  (GADGET_UUID_32 >> 16) & 0xff;
    *p_buffer++ =  (GADGET_UUID_32 >> 24) & 0xff;

    *p_buffer++ = 0;

    *p_len = *p_len + p_buffer - base_ptr;
}

static void print_buffer(uint8_t* buffer, uint8_t len)
{
    printf("%s\n", __FUNCTION__);

    uint8_t index = 0;
    for (index = 0; index < len; index++)
    {
        printf("%02x", buffer[index]);
    }
    printf("\n");
}

void construct_gadget_eir(uint8_t* buffer, uint8_t* p_len, const char* gadget_name, uint16_t pid, uint16_t vid)
{
     //Assuming that the buffer is sufficient for filling in EIR data
     //Initialize the p_len
     *p_len = 0;

     printf("%s: constructing eir data with gadget name: %s\n", __FUNCTION__, gadget_name);

     add_gadget_name_to_eir(buffer, p_len, gadget_name);

     add_gadget_uuid128_to_eir(buffer, p_len);

     add_gadget_pid_vid_to_eir(buffer, p_len, pid, vid);

}

int main()
{
    uint8_t buffer[256];
    uint8_t len = 0;
    construct_gadget_eir(buffer, &len, "GadgetX", 0x1234, 0x4567);
    print_buffer(buffer, len);
}
