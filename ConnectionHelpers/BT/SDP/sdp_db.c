//
// Copyright 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
// These materials are licensed under the Amazon Software License in connection with the Alexa Gadgets Program.
// The Agreement is available at https://aws.amazon.com/asl/.
// See the Agreement for the specific terms and conditions of the Agreement.
// Capitalized terms not defined in this file have the meanings given to them in the Agreement.
//

#include "wiced_bt_sdp.h"

uint8_t sdp_database[] =
{
    
    SDP_ATTR_SEQUENCE_2(278),  // length is the sum of all records
    
    //first SDP record Serial Port
    SDP_ATTR_SEQUENCE_1(69),                                            // 2 bytes
    SDP_ATTR_RECORD_HANDLE(0x10002),                                // 8 bytes
    SDP_ATTR_CLASS_ID(UUID_SERVCLASS_SERIAL_PORT),                  // 8
    SDP_ATTR_RFCOMM_PROTOCOL_DESC_LIST(OTA_RFCOMM_SCN),                        // 17 bytes
    SDP_ATTR_BROWSE_LIST,                                           // 8
    SDP_ATTR_PROFILE_DESC_LIST(UUID_SERVCLASS_SERIAL_PORT, 0x0102), // 13 byte
    SDP_ATTR_TEXT_1(ATTR_ID_SERVICE_NAME, 10),                      // 15
    'S', 'P', 'P', ' ', 'S', 'E', 'R', 'V', 'E', 'R',
    
    // second  SDP record generic RFCOMM networking for directives
    SDP_ATTR_SEQUENCE_1(69),                                            // 2 bytes
    SDP_ATTR_RECORD_HANDLE(0x10003),                                // 8 bytes
    SDP_ATTR_CLASS_ID(UUID_SERVCLASS_GENERIC_NETWORKING),                  // 8
    SDP_ATTR_RFCOMM_PROTOCOL_DESC_LIST(DIRECTIVE_RFCOMM_SCN),                        // 17 bytes
    SDP_ATTR_BROWSE_LIST,                                           // 8
    SDP_ATTR_PROFILE_DESC_LIST(UUID_SERVCLASS_GENERIC_NETWORKING, 0x0102), // 13 byte
    SDP_ATTR_TEXT_1(ATTR_ID_SERVICE_NAME, 10),                      // 15
    'R', 'F', 'C', ' ', 'S', 'E', 'R', 'V', 'E', 'R',
    
    // third SDP record gadget 128 uuid
    SDP_ATTR_SEQUENCE_1(59),                         // 2 bytes, length of the record
    SDP_ATTR_RECORD_HANDLE(0x10010),             // 8 bytes
    SDP_ATTR_CLASS_ID128,                        // 6 bytes
    SDP_GADGET_UUID128,                          // 16 bytes
    SDP_ATTR_PROTOCOL_DESC_LIST(1),              // 18
    SDP_ATTR_TEXT_1(ATTR_ID_SERVICE_NAME, 6),    // 11
    'g', 'a', 'd', 'g', 'e', 't',
    
    /************************************************************************/
    // forth SDP record Device ID (total = 69 + 2 = 71)
    SDP_ATTR_SEQUENCE_1(69),                                            // 2 bytes, length of the record
    SDP_ATTR_RECORD_HANDLE(0x10004),                                    // 8 byte
    SDP_ATTR_CLASS_ID(UUID_SERVCLASS_PNP_INFORMATION),                  // 8
    SDP_ATTR_PROTOCOL_DESC_LIST(1),                                     // 18
    SDP_ATTR_UINT2(ATTR_ID_SPECIFICATION_ID, 0x103),                  // 6
    SDP_ATTR_UINT2(ATTR_ID_VENDOR_ID, 0x0000),                         // 6
    SDP_ATTR_UINT2(ATTR_ID_PRODUCT_ID, 0x0000),                         // 6
    SDP_ATTR_UINT2(ATTR_ID_PRODUCT_VERSION, 0x0001),                    // 6
    SDP_ATTR_BOOLEAN(ATTR_ID_PRIMARY_RECORD, 0x01),                     // 5
    SDP_ATTR_UINT2(ATTR_ID_VENDOR_ID_SOURCE, DI_VENDOR_ID_SOURCE_BTSIG) // 6
    
};
const uint16_t sdp_database_len = (sizeof(sdp_database));


void set_sdp_db(uint16_t pid, uint16_t vid)
{
    WICED_BT_TRACE("%s, pid=%d, vid=%d\n",
                   __FUNCTION__, pid, vid);
    //set PID
    sdp_database[PRODUCT_ID_LOCATION_MSB] = (pid >> 8) & 0xff;
    sdp_database[PRODUCT_ID_LOCATION_LSB] = pid & 0xff;
    
    //set VID
    sdp_database[VENDOR_ID_LOCATION_MSB] = (vid >> 8) & 0xff;
    sdp_database[VENDOR_ID_LOCATION_LSB] = vid & 0xff;
    
    //Use the below commented function to set the sdp
    //wiced_bt_sdp_db_init((uint8_t*) sdp_database, sdp_database_len)
}
