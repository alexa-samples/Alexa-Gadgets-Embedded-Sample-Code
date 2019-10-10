//
// Copyright 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
// These materials are licensed under the Amazon Software License in connection with the Alexa Gadgets Program.
// The Agreement is available at https://aws.amazon.com/asl/.
// See the Agreement for the specific terms and conditions of the Agreement.
// Capitalized terms not defined in this file have the meanings given to them in the Agreement.
//


#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "accessories.pb.h"
#include "common.h"
#include "helpers.h"
#include "pb.h"
#include "pb_decode.h"
#include "rx.h"
#include "tx.h"


typedef struct rx_buffer_s {
    transaction_id_t transactionId;
    stream_id_t streamId;
    uint8_t seqNum;
    size_t bufferSize;
    size_t dataSize;
    uint8_t data[];
} rx_buffer_t;

static void freeRxBufferPtr(rx_buffer_t **ppRxBuffer) {
    if (!ppRxBuffer) return;
    if (*ppRxBuffer != NULL) {
        free(*ppRxBuffer);
        *ppRxBuffer = NULL;
    }
}

static void handleDeviceInformationReceived(DeviceInformation const *const deviceInformation) {
    printf("Inside %s\n", __FUNCTION__);
    printf("Device Information is:\n");
    printf("        Serial number: %s\n", deviceInformation->serial_number);
    printf("                 Name: %s\n", deviceInformation->name);
    printf("          Device type: %s\n", deviceInformation->device_type);
    printf("       Num transports: %d\n", deviceInformation->supported_transports_count);
    for (int i = 0; i < deviceInformation->supported_transports_count; i++) {
        printf("  transport[%d]: %d\n", i, deviceInformation->supported_transports[i]);
    }
}

static void handleDeviceFeaturesReceived(DeviceFeatures const *const devicefeatures) {
    printf("Inside %s\n", __FUNCTION__);
    printf("Device Features are:\n");
    printf("features           : %llu\n", devicefeatures->features);
    printf("attributes         : %llu\n", devicefeatures->device_attributes);
}

static packet_list_t *handleReceivedResponse(packet_list_t *rspPacketList, ControlEnvelope *controlEnvelope) {
    printf("Inside %s\n", __FUNCTION__);
    printf("Received response for command: %s\n", commandToString(controlEnvelope->command));
    switch (controlEnvelope->payload.response.which_payload) {
        case Response_device_information_tag:
            handleDeviceInformationReceived(&controlEnvelope->payload.response.payload.device_information);
            break;
        case Response_device_features_tag:
            handleDeviceFeaturesReceived(&controlEnvelope->payload.response.payload.device_features);
            break;
        default:
            break;
    }
    return rspPacketList;
}

packet_list_t *handleCommandUpdateComponentSegment(packet_list_t *rspPacketList, UpdateComponentSegment *message) {
    printf("Inside %s\n", __FUNCTION__);
    printf("Segment size = %u\n", message->segment_size);
    printf("Component name = %s\n", message->component_name);
    printf("Component offset = %u\n", message->component_offset);
    int indentSize = printf("signature = ");
    printHexBuffer((uint8_t *) &message->segment_signature[0], sizeof(message->segment_signature), indentSize);

    rspPacketList = PacketList_appendList(rspPacketList, createResponseUpdateComponentSegment());
    return rspPacketList;
}

packet_list_t *handleCommandApplyFirmware(packet_list_t *rspPacketList, ApplyFirmware *applyFirmware) {
    printf("Inside %s\n", __FUNCTION__);
    printf("restart_required = %s\n", applyFirmware->restart_required ? "true" : "false");
    printf("Firmware information is:\n");
    printf("                  name : %s\n", applyFirmware->firmware_information.name);
    printf("          version name : %s\n", applyFirmware->firmware_information.version_name);
    printf("                locale : %s\n", applyFirmware->firmware_information.locale);
    printf("               version : %u\n", applyFirmware->firmware_information.version);
    printf("       component count : %u\n", applyFirmware->firmware_information.components_count);
    if (applyFirmware->firmware_information.components_count > 0) {
        printf("     component[0] name : %s\n", applyFirmware->firmware_information.components[0].name);
        printf("  component[0] version : %u\n", applyFirmware->firmware_information.components[0].version);
        printf("     component[0] size : %u\n", applyFirmware->firmware_information.components[0].size);
        int indentSize = printf("component[0] signature : ");
        printHexBuffer((uint8_t *) &applyFirmware->firmware_information.components[0].signature[0],
                       sizeof(applyFirmware->firmware_information.components[0].signature), indentSize);
    }
    rspPacketList = PacketList_appendList(rspPacketList, createResponseApplyFirmware());
    return rspPacketList;
}

packet_list_t *handleReceivedCommand(packet_list_t *rspPacketList, ControlEnvelope *controlEnvelope) {
    switch (controlEnvelope->command) {
        case Command_GET_DEVICE_INFORMATION:
            rspPacketList = PacketList_appendList(rspPacketList, createResponseGetDeviceInformation());
            break;
        case Command_GET_DEVICE_FEATURES:
            rspPacketList = PacketList_appendList(rspPacketList, createResponseGetDeviceFeatures());
            break;
        case Command_UPDATE_COMPONENT_SEGMENT:
            rspPacketList = handleCommandUpdateComponentSegment(rspPacketList,
                                                                &controlEnvelope->payload.update_component_segment);
            break;
        case Command_APPLY_FIRMWARE:
            rspPacketList = handleCommandApplyFirmware(rspPacketList, &controlEnvelope->payload.apply_firmware);
            break;
        default:
            rspPacketList = PacketList_appendList(rspPacketList,
                                                  createResponseError(controlEnvelope->command,
                                                                      ErrorCode_UNSUPPORTED,
                                                                      0));
            break;
    }
    return rspPacketList;
}

packet_list_t *handleControlMessage(packet_list_t *rspPacketList, uint8_t *buffer, size_t bufferSize) {
    ControlEnvelope controlEnvelope = ControlEnvelope_init_default;
    pb_istream_t stream = pb_istream_from_buffer(buffer, bufferSize);
    if (!pb_decode(&stream, ControlEnvelope_fields, &controlEnvelope)) {
        fprintf(stderr, "%s: pb_decode Failed :: %s\n", __FUNCTION__, PB_GET_ERROR(&stream));
        return rspPacketList;
    }
    if (controlEnvelope.which_payload == ControlEnvelope_response_tag) {
        rspPacketList = handleReceivedResponse(rspPacketList, &controlEnvelope);
    } else {
        rspPacketList = handleReceivedCommand(rspPacketList, &controlEnvelope);
    }
    return rspPacketList;
}

packet_list_t *handleAlexaDirective(packet_list_t *rspPacketList, uint8_t *buffer, size_t buffersize) {
    printf("Inside %s\n", __FUNCTION__);

    // For parsing this message, check out the sample code in AlexaGadgetsProtobuf/examples folder.
    int indentSize = printf("Received Alexa directive: ");
    printHexBuffer(buffer, buffersize, indentSize);

    rspPacketList = PacketList_appendList(rspPacketList, createSampleDiscoveryResponseMessage());
    return rspPacketList;
}

static packet_list_t *handleAlexaEvent(packet_list_t *rspPacketList, uint8_t *buffer, size_t buffersize) {
    printf("Inside %s\n", __FUNCTION__);

    // For parsing this message, check out the sample code in AlexaGadgetsProtobuf/examples folder.
    int indentSize = printf("Received Alexa event: ");

    printHexBuffer(buffer, buffersize, indentSize);
    return rspPacketList;
}

static packet_list_t *handleDataReceived(role_t role, packet_list_t *rspPacketList, stream_id_t streamId,
                                         transaction_id_t transactionId, uint8_t *buffer, size_t bufferSize, bool ack) {
    switch (streamId) {
        case CONTROL_STREAM: {
            if (role == ROLE_GADGET) {
                packet_t controlAck = createControlAckPacket(streamId, transactionId, ack,
                                                             CONTROL_PACKET_RESULT_SUCCESS);
                rspPacketList = PacketList_addToTail(rspPacketList, &controlAck);
            }
            rspPacketList = handleControlMessage(rspPacketList, buffer, bufferSize);
        }
            break;
        case OTA_STREAM: {
            // Handle OTA stream packets.
        }
            break;
        case ALEXA_STREAM: {
            if (role == ROLE_GADGET) {
                packet_t controlAck = createControlAckPacket(streamId, transactionId, ack,
                                                             CONTROL_PACKET_RESULT_SUCCESS);
                rspPacketList = PacketList_addToTail(rspPacketList, &controlAck);
                rspPacketList = handleAlexaDirective(rspPacketList, buffer, bufferSize);
            } else { // ROLE_ECHO
                rspPacketList = handleAlexaEvent(rspPacketList, buffer, bufferSize);
            }
            break;
        }
        default:
            fprintf(stderr, "Unhandled stream [%u]", streamId);
    }
    return rspPacketList;
}

packet_list_t *decodePacket(role_t role, packet_list_t *rspPacketList, packet_t const *const packet) {
    if (!packet) return rspPacketList;

    uint8_t const *const buffer = packet->data;
    size_t const bufferSize = packet->dataSize;

    static rx_buffer_t *rxBuffers[3] = {NULL, NULL, NULL};
    size_t offset = 0;

    while (bufferSize > offset) {
        if (bufferSize - offset < 2) {
            fprintf(stderr, "Insufficient Length :: [%lu/%d]", bufferSize - offset, 2);
            return rspPacketList;
        }
        stream_id_t streamId = (buffer[offset] >> STREAM_ID_SHIFT) & STREAM_ID_MASK;
        transaction_id_t transactionId = (buffer[offset] >> TRANSACTION_ID_SHIFT) & TRANSACTION_ID_MASK;
        offset++;
        uint8_t seqNum = (buffer[offset] >> SEQ_NUM_ID_SHIFT) & SEQ_NUM_ID_MASK;
        transaction_type_t transactionType = (buffer[offset] >> TRANSACTION_TYPE_SHIFT) & TRANSACTION_TYPE_MASK;
        bool ack = (buffer[offset] & (1U << ACK_BIT_SHIFT)) != 0;
        bool extendLength = (buffer[offset] & (1U << EXTENDED_LENGTH_BIT_SHIFT)) != 0;
        offset++;

        if (transactionType == TRANSACTION_TYPE_CONTROL) {
            if (bufferSize - offset < (CONTROL_PACKET_LENGTH - 2)) {
                fprintf(stderr, "Insufficient Length :: Control Packet [%lu/%d]", bufferSize - offset,
                        CONTROL_PACKET_LENGTH - 2);
                return rspPacketList;
            }
            // Reserved: 1 byte.
            offset++;
            // Length: 1 byte.
            size_t length = buffer[offset++];
            assert(length == 2);
            // Reserved: 1 byte.
            offset++;
            control_ack_result_t result = buffer[offset++];
            printf("RX ControlPacketAck: result=%s\n",
                   (result == CONTROL_PACKET_RESULT_SUCCESS) ? "SUCCESS" : "UNSUPPORTED");
            continue;
        }

        int rxBufferIndex = -1;
        if (transactionType == TRANSACTION_TYPE_INITIAL) {
            if (bufferSize - offset < 5) {
                fprintf(stderr, "Insufficient Length :: Initial Packet [%lu/%d]", bufferSize - offset, 5);
            }
            // Reserved: 1 byte.
            offset++;

            // Total transaction Length
            uint16_t transactionLength;
            transactionLength = buffer[offset++] << 8U;
            transactionLength |= buffer[offset++] << 0U;
            printf("New Rx Transaction [%d] :: Stream [%d]\n", transactionId, streamId);
            rxBufferIndex = streamToIndex(streamId);
            if (rxBufferIndex < 0) {
                fprintf(stderr, "Invalid streamId [%d]. Could not create an RX Buffer.", streamId);
                return rspPacketList;
            }
            // Ensure that this packet does not exist and then create it.
            assert(rxBuffers[rxBufferIndex] == NULL);
            rxBuffers[rxBufferIndex] = malloc(sizeof(rx_buffer_t) + transactionLength);
            if (!rxBuffers[rxBufferIndex]) {
                fprintf(stderr, "Failed to alloc a new RX packet for Transaction [%d] :: stream [%d]",
                        transactionId, streamId);
                return rspPacketList;
            }

            // Initialize the new packet.
            rxBuffers[rxBufferIndex]->streamId = streamId;
            rxBuffers[rxBufferIndex]->transactionId = transactionId;
            rxBuffers[rxBufferIndex]->bufferSize = transactionLength;
            rxBuffers[rxBufferIndex]->seqNum = 0;
            rxBuffers[rxBufferIndex]->dataSize = 0;
        } else {
            // Find an existing packet
            rxBufferIndex = streamToIndex(streamId);
            if (rxBufferIndex < 0) {
                fprintf(stderr, "Invalid streamId [%d]. Could not find an RX Buffer.", streamId);
                return rspPacketList;
            }
            if (rxBuffers[rxBufferIndex] == NULL) { // Ensure that this packet exists.
                fprintf(stderr, "Unable to find Rx packet :: Transaction [%d] :: Stream [%d]", transactionId, streamId);
                return rspPacketList;
            }
            assert(rxBuffers[rxBufferIndex]->transactionId == transactionId);
            assert(rxBuffers[rxBufferIndex]->streamId == streamId);
        }
        size_t currentPayloadLength = 0;
        if (extendLength) {
            if (bufferSize - offset < 2) {
                fprintf(stderr, "Insufficient Length :: Extended Payload Header [%lu/%d]", bufferSize - offset, 2);
                packet_t controlAck = createControlAckPacket(streamId, transactionId, ack,
                                                             CONTROL_PACKET_RESULT_FAILURE);
                rspPacketList = PacketList_addToTail(rspPacketList, &controlAck);
                freeRxBufferPtr(&rxBuffers[rxBufferIndex]);
                return rspPacketList;
            }
            currentPayloadLength |= buffer[offset++] << 8U; // MSB of payload length.
        } else {
            if (bufferSize - offset < 1) {
                fprintf(stderr, "Insufficient Length :: Extended Payload Header [%lu/%d]", bufferSize - offset, 1);
                packet_t controlAck = createControlAckPacket(streamId, transactionId, ack,
                                                             CONTROL_PACKET_RESULT_FAILURE);
                rspPacketList = PacketList_addToTail(rspPacketList, &controlAck);
                freeRxBufferPtr(&rxBuffers[rxBufferIndex]);
                return rspPacketList;
            }
        }
        currentPayloadLength |= buffer[offset++] << 0U; // LSB of payload length.

        if (bufferSize - offset < currentPayloadLength) {
            fprintf(stderr, "Insufficient Length :: payload [%lu/%zu]", bufferSize - offset, currentPayloadLength);
            packet_t controlAck = createControlAckPacket(streamId, transactionId, ack, CONTROL_PACKET_RESULT_FAILURE);
            rspPacketList = PacketList_addToTail(rspPacketList, &controlAck);
            freeRxBufferPtr(&rxBuffers[rxBufferIndex]);
            return rspPacketList;
        }

        if (rxBuffers[rxBufferIndex]->seqNum != seqNum) {
            fprintf(stderr, "Sequence Failed [%d] :: Expected [%d]", seqNum, rxBuffers[rxBufferIndex]->seqNum);
            packet_t controlAck = createControlAckPacket(streamId, transactionId, ack, CONTROL_PACKET_RESULT_FAILURE);
            rspPacketList = PacketList_addToTail(rspPacketList, &controlAck);
            freeRxBufferPtr(&rxBuffers[rxBufferIndex]);
            return rspPacketList;
        }

        // Check if destination packet has sufficient length.
        if (rxBuffers[rxBufferIndex]->bufferSize - rxBuffers[rxBufferIndex]->dataSize < currentPayloadLength) {
            fprintf(stderr, "Buffer Overflow :: Transaction [%d] :: Received [%zu/%zu] :: Packet %zu\n", transactionId,
                    rxBuffers[rxBufferIndex]->dataSize, rxBuffers[rxBufferIndex]->bufferSize, currentPayloadLength);
            packet_t controlAck = createControlAckPacket(streamId, transactionId, ack, CONTROL_PACKET_RESULT_FAILURE);
            rspPacketList = PacketList_addToTail(rspPacketList, &controlAck);
            freeRxBufferPtr(&rxBuffers[rxBufferIndex]);
            return rspPacketList;
        }
        memcpy(rxBuffers[rxBufferIndex]->data + rxBuffers[rxBufferIndex]->dataSize,
               &buffer[offset],
               currentPayloadLength);
        rxBuffers[rxBufferIndex]->dataSize += currentPayloadLength;
        offset += currentPayloadLength;
        rxBuffers[rxBufferIndex]->seqNum = (rxBuffers[rxBufferIndex]->seqNum + 1) & 0x0FU;
        printf("Rx Progress [%zu/%zu] :: Stream [%d] :: Transaction [%d]\n",
               rxBuffers[rxBufferIndex]->dataSize, rxBuffers[rxBufferIndex]->bufferSize, streamId, transactionId);
        if (rxBuffers[rxBufferIndex]->dataSize == rxBuffers[rxBufferIndex]->bufferSize) {
            rspPacketList = handleDataReceived(role, rspPacketList, streamId, transactionId,
                                               rxBuffers[rxBufferIndex]->data, rxBuffers[rxBufferIndex]->dataSize, ack);
            freeRxBufferPtr(&rxBuffers[rxBufferIndex]);
        }
    }
    return rspPacketList;
}


packet_list_t *receivePackets(role_t role, packet_list_t const *const list) {
    packet_list_t *response = NULL;
    for (packet_list_t const *node = list; node != NULL; node = node->next) {
        response = decodePacket(role, response, &node->packet);
    }
    return response;
}
