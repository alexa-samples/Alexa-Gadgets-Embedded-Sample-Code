//
// Copyright 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
// These materials are licensed under the Amazon Software License in connection with the Alexa Gadgets Program.
// The Agreement is available at https://aws.amazon.com/asl/.
// See the Agreement for the specific terms and conditions of the Agreement.
// Capitalized terms not defined in this file have the meanings given to them in the Agreement.
//

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "common.h"
#include "helpers.h"


void printHexBuffer(uint8_t const *const buf, size_t const bufSize, int const indentSize) {
    bool newLine = false;
    for (size_t i = 0; i < bufSize; i++) {
        if (newLine) {
            for (size_t j = 0; j < indentSize; j++) printf(" ");
            newLine = false;
        }
        printf("%02x ", buf[i]);

        if ((i + 1) % 16 == 0) {
            printf("\n");
            newLine = true;
        } else if ((i + 1) % 8 == 0) {
            printf(" ");
        }
    }
    if (!newLine) {
        printf("\n");
    }
}

packet_list_t *PacketList_addToTail(packet_list_t *const list, packet_t const *const packet) {
    if (packet == NULL) return list;
    if (packet->data == NULL)
        return list;
    packet_list_t *node = malloc(sizeof(packet_list_t));
    if (!node) {
        return list;
    }
    node->packet = *packet;
    node->next = NULL;

    packet_list_t *tail = list;
    while (tail && tail->next) {
        tail = tail->next;
    }
    if (tail) {
        tail->next = node;
        return list;
    } else {
        return node;
    }
}

packet_list_t *PacketList_appendList(packet_list_t *dst, packet_list_t *src) {
    if (src == NULL) return dst;
    if (dst == NULL) return src;

    packet_list_t *tail = dst;
    while (tail->next != NULL) {
        tail = tail->next;
    }
    // Append the src list.
    tail->next = src;
    return dst;
}

void PacketList_freeList(packet_list_t *list) {
    while (list) {
        packet_list_t *temp = list;
        list = list->next;
        if (temp->packet.data)
            free(temp->packet.data);
        free(temp);
    }
}

size_t PacketList_getSize(packet_list_t const *const list) {
    size_t numPackets = 0;
    for (packet_list_t const *node = list; node != NULL; node = node->next) {
        numPackets++;
    }
    return numPackets;
}

void PacketList_PrintAll(packet_list_t const *const list) {
    size_t numPackets = PacketList_getSize(list), packetIndex = 0;
    if (numPackets == 0) {
        printf("Empty List\n");
        return;
    }
    for (packet_list_t const *node = list; node != NULL; node = node->next) {
        int numSpaces = printf("Packet [%zu/%zu] contains: ", ++packetIndex, numPackets);
        printHexBuffer(node->packet.data, node->packet.dataSize, numSpaces);
    }
}

void freePacket(packet_t *packet) {
    if (packet->data != NULL) {
        free(packet->data);
        packet->data = NULL;
        packet->dataSize = 0;
    }
}

void printPacket(packet_t const *const packet, int indentSize) {
    if (!packet)return;
    if (!packet->data)return;
    printHexBuffer(packet->data, packet->dataSize, indentSize);
}


size_t streamToIndex(stream_id_t streamId) {
    switch (streamId) {
        case CONTROL_STREAM:
            return 0;
        case ALEXA_STREAM:
            return 1;
        case OTA_STREAM:
            return 2;
        default:
            return -1;
    }
}

char *commandToString(Command command) {
    switch (command) {
        case Command_GET_DEVICE_INFORMATION:
            return "Command_GET_DEVICE_INFORMATION";
        case Command_GET_DEVICE_FEATURES:
            return "Command_GET_DEVICE_FEATURES";
        case Command_UPDATE_COMPONENT_SEGMENT:
            return "Command_UPDATE_COMPONENT_SEGMENT";
        case Command_APPLY_FIRMWARE:
            return "Command_APPLY_FIRMWARE";
        default:
            return "UNKNOWN_COMMAND";
    }
}