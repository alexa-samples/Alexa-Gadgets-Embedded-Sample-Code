//
// Copyright 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
// These materials are licensed under the Amazon Software License in connection with the Alexa Gadgets Program.
// The Agreement is available at https://aws.amazon.com/asl/.
// See the Agreement for the specific terms and conditions of the Agreement.
// Capitalized terms not defined in this file have the meanings given to them in the Agreement.
//

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "accessories.pb.h"
#include "common.h"
#include "helpers.h"
#include "pb.h"
#include "pb_encode.h"

static transaction_id_t getNextTransactionId(stream_id_t streamId) {
    static uint8_t lastTransactionId[3] = {0xff, 0xff, 0xff};

    int index = streamToIndex(streamId);
    if (index < 0) {
        return 0;
    }

    lastTransactionId[index] = (lastTransactionId[index] + 1) & 0x0f; // TransactionId is 4-bits only.
    return lastTransactionId[index];
}

packet_t createProtocolVersionPacket() {
    printf("Inside %s\n", __FUNCTION__);
    uint8_t *buffer = malloc(PROTOCOL_VERSION_PACKET_SIZE);
    if (buffer) {
        memset(buffer, 0, PROTOCOL_VERSION_PACKET_SIZE);
        buffer[0] = (uint8_t) (PROTOCOL_IDENTIFIER >> 8U);
        buffer[1] = (uint8_t) (PROTOCOL_IDENTIFIER >> 0U);
        buffer[2] = PROTOCOL_VERSION_MAJOR;
        buffer[3] = PROTOCOL_VERSION_MINOR;
        buffer[4] = (uint8_t) (SAMPLE_NEGOTIATED_MTU >> 8U);
        buffer[5] = (uint8_t) (SAMPLE_NEGOTIATED_MTU >> 0U);
        buffer[6] = (uint8_t) (SAMPLE_MAX_TRANSACTION_SIZE >> 8U);
        buffer[7] = (uint8_t) (SAMPLE_MAX_TRANSACTION_SIZE >> 0U);
    }
    packet_t packet = {};
    if (buffer) {
        packet.data = buffer;
        packet.dataSize = PROTOCOL_VERSION_PACKET_SIZE;
    }
    return packet;
}

packet_t createAdvertisingPacket() {
    printf("Inside %s\n", __FUNCTION__);
    // Advertising data (AD) is organized in LTV (Length/Tag/Value) triplets.
    uint8_t *buffer = malloc(ADV_DATA_LEN);
    if (buffer) {
        // Flags
        buffer[0] = 2;    // AD Type Length.
        buffer[1] = 0x01; // AD Type Identifier: flags.
        buffer[2] = BLE_AD_TYPE;
        // Service UUID
        buffer[3] = 3;    // AD Type Length.
        buffer[4] = 0x03; // AD Type Identifier: 16-bit service class UUID.
        buffer[5] = (uint8_t) (SERVICE_UUID >> 0);
        buffer[6] = (uint8_t) (SERVICE_UUID >> 8);
        // Service data
        buffer[7] = 23;   // AD Type Length.
        buffer[8] = 0x16; // AD Type identifier: service data.
        buffer[9] = (uint8_t) (SERVICE_UUID >> 0);
        buffer[10] = (uint8_t) (SERVICE_UUID >> 8);
        buffer[11] = (uint8_t) (VENDOR_ID >> 0);
        buffer[12] = (uint8_t) (VENDOR_ID >> 8);
        buffer[13] = 0x00;          // reserved.
        buffer[14] = 0xFF;          // Product category.
        memset(&buffer[15], 0, 16); // reserved for future use.
    }

    packet_t packet = {};
    if (buffer) {
        packet.data = buffer;
        packet.dataSize = ADV_DATA_LEN;
    }
    return packet;
}

packet_t
createControlAckPacket(stream_id_t streamId, transaction_id_t transactionId, bool ack, control_ack_result_t result) {
    packet_t packet = {};
    if (ack == false) return packet;

    printf("Inside %s\n", __FUNCTION__);
    uint8_t *buffer = malloc(CONTROL_PACKET_LENGTH);
    if (buffer) {

        buffer[0] = (streamId & STREAM_ID_MASK) << STREAM_ID_SHIFT;
        buffer[0] |= (transactionId & TRANSACTION_ID_MASK) << TRANSACTION_ID_SHIFT;
        buffer[1] = (TRANSACTION_TYPE_CONTROL & TRANSACTION_TYPE_MASK) << TRANSACTION_TYPE_SHIFT;
        buffer[1] |= (result == CONTROL_PACKET_RESULT_SUCCESS) ? (1U << ACK_BIT_SHIFT) : 0;
        buffer[2] = 0; // Reserved.
        buffer[3] = 2; // Length 2 bytes.
        buffer[4] = 1; // Reserved
        buffer[5] = result;

        packet.data = buffer;
        packet.dataSize = CONTROL_PACKET_LENGTH;
    }
    return packet;
}

static packet_list_t *buildStreamPacket(stream_id_t streamId, bool ack, uint8_t *payload, size_t payloadSize) {
    // https://developer.amazon.com/docs/alexa-gadgets-toolkit/packet-ble.html#packet-format
    if (streamId != CONTROL_STREAM && streamId != OTA_STREAM && streamId != ALEXA_STREAM) {
        fprintf(stderr, "Invalid argument streamId");
        return NULL;
    }
    if (payloadSize > 0xffff) {
        fprintf(stderr, "Invalid argument payloadSize");
        return NULL;
    }

    size_t remainingSize = payloadSize;
    packet_list_t *packetListHead = NULL;
    uint8_t seqNum = 0;
    transaction_id_t transactionId = 0;
    size_t srcIndex = 0;

    while (remainingSize > 0) {
        // Calculate the current size.
        size_t currentPacketHeaderSize = 3;
        transaction_type_t transactionType = TRANSACTION_TYPE_CONTINUE;
        if (srcIndex == 0) { // First packet header
            transactionType = TRANSACTION_TYPE_INITIAL;
            transactionId = getNextTransactionId(streamId);
            printf("New Tx Transaction [%d] :: Stream [%d]\n", transactionId, streamId);
            currentPacketHeaderSize += 3;
        }
        size_t currentPacketPayloadSize = MIN(SAMPLE_NEGOTIATED_MTU - currentPacketHeaderSize, remainingSize);
        bool extendedLength = false;
        if (currentPacketPayloadSize > 0xff) {
            extendedLength = true;
            currentPacketHeaderSize++;
            if (currentPacketPayloadSize > SAMPLE_NEGOTIATED_MTU - currentPacketHeaderSize) {
                currentPacketPayloadSize--;
            }
        }

        // Make the last packet as final.
        if (transactionType == TRANSACTION_TYPE_CONTINUE && currentPacketPayloadSize == remainingSize) {
            transactionType = TRANSACTION_TYPE_FINAL;
        }

        size_t currentPacketSize = currentPacketHeaderSize + currentPacketPayloadSize;
        uint8_t *const buffer = malloc(currentPacketSize);
        if (buffer) {
            size_t dstIndex = 0;
            // StreamId: 4 bits
            buffer[dstIndex] = (streamId & STREAM_ID_MASK) << STREAM_ID_SHIFT;
            // TransactionId: 4 bits
            buffer[dstIndex] |= (transactionId & TRANSACTION_ID_MASK) << TRANSACTION_ID_SHIFT;
            dstIndex++;

            // Sequence number: 4 bits
            buffer[dstIndex] = (seqNum & SEQ_NUM_ID_MASK) << SEQ_NUM_ID_SHIFT;
            seqNum = (seqNum + 1) & SEQ_NUM_ID_MASK;
            // Transaction type: 4 bits
            buffer[dstIndex] |= (transactionType & TRANSACTION_TYPE_MASK) << TRANSACTION_TYPE_SHIFT;
            // ACK: 1 bit
            buffer[dstIndex] |= (ack) ? (1U << ACK_BIT_SHIFT) : 0;
            // LengthExtender: 1 bit
            buffer[dstIndex] |= (extendedLength) ? (1U << EXTENDED_LENGTH_BIT_SHIFT) : 0;
            dstIndex++;

            if (transactionType == TRANSACTION_TYPE_INITIAL) {
                // Reserved: 8 bits
                buffer[dstIndex++] = 0x00;
                // Total transaction length.
                buffer[dstIndex++] = payloadSize >> 8U;
                buffer[dstIndex++] = payloadSize >> 0U;
            }

            if (extendedLength) {
                // ExtendedLength: extra 8 bits in payload length.
                buffer[dstIndex++] = currentPacketPayloadSize >> 8U;
            }
            // Length: 8 bits.
            buffer[dstIndex++] = currentPacketPayloadSize >> 0U;

            // Payload
            memcpy(&buffer[dstIndex], &payload[srcIndex], currentPacketPayloadSize);
            dstIndex += currentPacketPayloadSize;
            srcIndex += currentPacketPayloadSize;
            printf("Tx Progress [%zu/%zu] :: Stream [%d] :: Transaction [%d]\n", srcIndex, payloadSize, streamId,
                   transactionId);

            // Append this packet to the list of buffers ready for TX.
            packet_t packet = {};
            packet.data = buffer;
            packet.dataSize = currentPacketSize;
            packetListHead = PacketList_addToTail(packetListHead, &packet);
        } else {
            fprintf(stderr, "Failed to allocate memory for TX packet. Exiting");
            exit(1);
        }
        // Update the remaining size.
        remainingSize -= currentPacketPayloadSize;
    }
    return packetListHead;
}

static packet_list_t *createControlPacket(ControlEnvelope const *const controlEnvelope, bool ackRequired) {
    size_t encoded_size;
    if (!pb_get_encoded_size(&encoded_size, ControlEnvelope_fields, controlEnvelope)) {
        fprintf(stderr, "Failed To Calculate Control Envelope Encoded Size");
        return NULL;
    }
    uint8_t buffer[encoded_size];
    pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer));
    bool status = pb_encode(&stream, ControlEnvelope_fields, controlEnvelope);
    if (!status) {
        fprintf(stderr, "%s: pb_encode failed :: %s\n", __FUNCTION__, PB_GET_ERROR(&stream));
        return NULL;
    }
    return buildStreamPacket(CONTROL_STREAM, ackRequired, buffer, stream.bytes_written);
}

packet_list_t *createCommandGetDeviceInformation() {
    ControlEnvelope controlEnvelope = ControlEnvelope_init_default;
    controlEnvelope.command = Command_GET_DEVICE_INFORMATION;

    printf("Creating command: %s\n", commandToString(controlEnvelope.command));
    return createControlPacket(&controlEnvelope, false);
}

packet_list_t *createCommandGetDeviceFeatures() {
    ControlEnvelope controlEnvelope = ControlEnvelope_init_default;
    controlEnvelope.command = Command_GET_DEVICE_FEATURES;

    printf("Creating command: %s\n", commandToString(controlEnvelope.command));
    return createControlPacket(&controlEnvelope, false);
}

packet_list_t *createCommandUpdateComponentSegment() {
    ControlEnvelope controlEnvelope = ControlEnvelope_init_default;
    controlEnvelope.command = Command_UPDATE_COMPONENT_SEGMENT;
    controlEnvelope.which_payload = ControlEnvelope_update_component_segment_tag;

    UpdateComponentSegment *updateComponentSegment = &controlEnvelope.payload.update_component_segment;
    strcpy(updateComponentSegment->component_name, "ComponentName");
    updateComponentSegment->component_offset = 0;
    // note: cannot overwrite the last byte of the signature because nanopb needs null termination.
    memset(updateComponentSegment->segment_signature, 0xAA,
           sizeof(updateComponentSegment->segment_signature) - 1);
    updateComponentSegment->segment_size = 1024;

    printf("Creating command: %s\n", commandToString(controlEnvelope.command));
    return createControlPacket(&controlEnvelope, true);
}

packet_list_t *createCommandApplyFirmware() {
    ControlEnvelope controlEnvelope = ControlEnvelope_init_default;
    controlEnvelope.command = Command_APPLY_FIRMWARE;
    controlEnvelope.which_payload = ControlEnvelope_apply_firmware_tag;

    ApplyFirmware *applyFirmware = &controlEnvelope.payload.apply_firmware;
    applyFirmware->restart_required = true;
    applyFirmware->firmware_information.version = 12;
    strcpy(applyFirmware->firmware_information.name, "FirmwareName");
    strcpy(applyFirmware->firmware_information.locale, "enUS");
    strcpy(applyFirmware->firmware_information.version_name, "Version 12");

    applyFirmware->firmware_information.components_count = 1;
    FirmwareComponent * firmwareComponent = &applyFirmware->firmware_information.components[0];
    strcpy(firmwareComponent->name, "ComponentName");
    // note: cannot overwrite the last byte of the signature because nanopb needs null termination.
    memset(firmwareComponent->signature, 0xBB, sizeof(firmwareComponent->signature) - 1);


    printf("Creating command: %s\n", commandToString(controlEnvelope.command));
    return createControlPacket(&controlEnvelope, true);
}

packet_list_t *createResponseError(Command cmd, ErrorCode errorCode, uint16_t tag) {
    ControlEnvelope controlEnvelope = ControlEnvelope_init_default;
    controlEnvelope.command = cmd;
    controlEnvelope.which_payload = ControlEnvelope_response_tag;

    Response *response = &controlEnvelope.payload.response;
    response->error_code = errorCode;
    response->which_payload = tag;

    printf("Creating response error for command: %s\n", commandToString(controlEnvelope.command));
    return createControlPacket(&controlEnvelope, false);
}

packet_list_t *createResponseGetDeviceInformation() {
    ControlEnvelope controlEnvelope = ControlEnvelope_init_default;
    controlEnvelope.command = Command_GET_DEVICE_INFORMATION;
    controlEnvelope.which_payload = ControlEnvelope_response_tag;
    controlEnvelope.payload.response.error_code = ErrorCode_SUCCESS;
    controlEnvelope.payload.response.which_payload = Response_device_information_tag;
    DeviceInformation *deviceInformation = &controlEnvelope.payload.response.payload.device_information;

    strcpy(deviceInformation->serial_number, "aabbccd");
    strcpy(deviceInformation->name, "GadgetName");
    deviceInformation->supported_transports_count = 1;
    deviceInformation->supported_transports[0] = Transport_BLUETOOTH_LOW_ENERGY;
    strcpy(deviceInformation->device_type, "wxyz");

    printf("Creating response: %s\n", commandToString(controlEnvelope.command));
    return createControlPacket(&controlEnvelope, false);
}

packet_list_t *createResponseGetDeviceFeatures() {
    ControlEnvelope controlEnvelope = ControlEnvelope_init_default;
    controlEnvelope.command = Command_GET_DEVICE_FEATURES;
    controlEnvelope.which_payload = ControlEnvelope_response_tag;
    controlEnvelope.payload.response.error_code = ErrorCode_SUCCESS;
    controlEnvelope.payload.response.which_payload = Response_device_features_tag;

    DeviceFeatures *deviceFeatures = &controlEnvelope.payload.response.payload.device_features;
    deviceFeatures->features = 0x13; // Support Alexa Gadgets Toolkit and OTA.

    printf("Creating response: %s\n", commandToString(controlEnvelope.command));
    return createControlPacket(&controlEnvelope, false);
}

packet_list_t *createResponseUpdateComponentSegment() {
    ControlEnvelope controlEnvelope = ControlEnvelope_init_default;
    controlEnvelope.command = Command_UPDATE_COMPONENT_SEGMENT;
    controlEnvelope.which_payload = ControlEnvelope_response_tag;
    controlEnvelope.payload.response.error_code = ErrorCode_SUCCESS;

    printf("Creating response: %s\n", commandToString(controlEnvelope.command));
    return createControlPacket(&controlEnvelope, false);
}

packet_list_t *createResponseApplyFirmware() {
    ControlEnvelope controlEnvelope = ControlEnvelope_init_default;
    controlEnvelope.command = Command_APPLY_FIRMWARE;
    controlEnvelope.which_payload = ControlEnvelope_response_tag;
    controlEnvelope.payload.response.error_code = ErrorCode_SUCCESS;

    printf("Creating response: %s\n", commandToString(controlEnvelope.command));
    return createControlPacket(&controlEnvelope, false);
}

packet_list_t *createAlexaDiscoveryDiscoverDirective() {
    printf("Creating Alexa.Discovery::Discover directive\n");

    // For creating this message, check out the sample code in AlexaGadgetsProtobuf/examples folder.
    uint8_t buffer[] = {0x0a, 0x1d, 0x0a, 0x1b, 0x0a, 0x0f, 0x41, 0x6c, 0x65, 0x78, 0x61, 0x2e, 0x44, 0x69, 0x73, 0x63,
                        0x6f, 0x76, 0x65, 0x72, 0x79, 0x12, 0x08, 0x44, 0x69, 0x73, 0x63, 0x6f, 0x76, 0x65, 0x72};
    packet_list_t *packetList = buildStreamPacket(ALEXA_STREAM, false, buffer, sizeof(buffer));
    return packetList;
}

packet_list_t *createSampleDiscoveryResponseMessage() {
    printf("Creating Alexa.Discovery::Discover.Response event\n");

    // For creating this message, check out the sample code in AlexaGadgetsProtobuf/examples folder.
    uint8_t buffer[] = {0x0a, 0xf2, 0x01, 0x0a, 0x24, 0x0a, 0x0f, 0x41, 0x6c, 0x65, 0x78, 0x61, 0x2e, 0x44, 0x69, 0x73,
                        0x63, 0x6f, 0x76, 0x65, 0x72, 0x79, 0x12, 0x11, 0x44, 0x69, 0x73, 0x63, 0x6f, 0x76, 0x65, 0x72,
                        0x2e, 0x52, 0x65, 0x73, 0x70, 0x6f, 0x6e, 0x73, 0x65, 0x12, 0xc9, 0x01, 0x0a, 0xc6, 0x01, 0x0a,
                        0x07, 0x74, 0x65, 0x73, 0x74, 0x20, 0x69, 0x64, 0x12, 0x0d, 0x66, 0x72, 0x69, 0x65, 0x6e, 0x64,
                        0x6c, 0x79, 0x20, 0x6e, 0x61, 0x6d, 0x65, 0x5a, 0x24, 0x0a, 0x0b, 0x74, 0x65, 0x73, 0x74, 0x20,
                        0x74, 0x79, 0x70, 0x65, 0x20, 0x31, 0x12, 0x10, 0x54, 0x65, 0x73, 0x74, 0x20, 0x69, 0x6e, 0x74,
                        0x65, 0x72, 0x66, 0x61, 0x63, 0x65, 0x20, 0x31, 0x1a, 0x03, 0x31, 0x2e, 0x30, 0x5a, 0x24, 0x0a,
                        0x0b, 0x74, 0x65, 0x73, 0x74, 0x20, 0x74, 0x79, 0x70, 0x65, 0x20, 0x32, 0x12, 0x10, 0x54, 0x65,
                        0x73, 0x74, 0x20, 0x69, 0x6e, 0x74, 0x65, 0x72, 0x66, 0x61, 0x63, 0x65, 0x20, 0x32, 0x1a, 0x03,
                        0x31, 0x2e, 0x30, 0x5a, 0x24, 0x0a, 0x0b, 0x74, 0x65, 0x73, 0x74, 0x20, 0x74, 0x79, 0x70, 0x65,
                        0x20, 0x33, 0x12, 0x10, 0x54, 0x65, 0x73, 0x74, 0x20, 0x69, 0x6e, 0x74, 0x65, 0x72, 0x66, 0x61,
                        0x63, 0x65, 0x20, 0x33, 0x1a, 0x03, 0x31, 0x2e, 0x31, 0x62, 0x3a, 0x0a, 0x02, 0x31, 0x39, 0x12,
                        0x09, 0x78, 0x78, 0x78, 0x78, 0x78, 0x78, 0x78, 0x78, 0x78, 0x1a, 0x03, 0x79, 0x79, 0x79, 0x22,
                        0x07, 0x61, 0x61, 0x62, 0x62, 0x63, 0x63, 0x64, 0x2a, 0x0f, 0x6d, 0x6f, 0x63, 0x6b, 0x20, 0x6d,
                        0x6f, 0x64, 0x65, 0x6c, 0x20, 0x6e, 0x61, 0x6d, 0x65, 0x32, 0x0a, 0x31, 0x32, 0x33, 0x34, 0x35,
                        0x36, 0x37, 0x38, 0x39, 0x30};
    packet_list_t *packetList = buildStreamPacket(ALEXA_STREAM, false, buffer, sizeof(buffer));
    return packetList;
}
