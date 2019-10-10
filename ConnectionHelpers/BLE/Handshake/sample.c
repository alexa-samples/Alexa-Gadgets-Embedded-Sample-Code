//
// Copyright 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
// These materials are licensed under the Amazon Software License in connection with the Alexa Gadgets Program.
// The Agreement is available at https://aws.amazon.com/asl/.
// See the Agreement for the specific terms and conditions of the Agreement.
// Capitalized terms not defined in this file have the meanings given to them in the Agreement.
//

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "helpers.h"
#include "tx.h"
#include "rx.h"


void runSampleCreateAdvertisingPacket() {
    printf("=================================================================================\n");
    printf("runSample of AdvertisingPacket\n");
    printf("=================================================================================\n");
    // Build advertising parameters.
    packet_t advPacket = createAdvertisingPacket();
    if (!advPacket.data) {
        fprintf(stderr, "Could not create Advertising Data packet. Exiting");
        exit(1);
    }
    int indentSize = printf("Advertising packet payload is: ");
    printPacket(&advPacket, indentSize);
    freePacket(&advPacket);
}

void runSampleProtocolVersionPacket() {
    printf("=================================================================================\n");
    printf("runSample for handshake: ProtocolVersionPacket\n");
    printf("=================================================================================\n");
    // Build protocol version packet.
    packet_t pvPacket = createProtocolVersionPacket();
    if (!pvPacket.data) {
        fprintf(stderr, "Could not create ProtocolVersion packet. Exiting");
        exit(1);
    }
    int indentSize = printf("Protocol Version packet content is: ");
    printPacket(&pvPacket, indentSize);
    freePacket(&pvPacket);
}

void runSample(packet_list_t *txPackets, char *sampleName) {
    printf("=================================================================================\n");
    printf("runSample for handshake: %s\n", sampleName);
    printf("=================================================================================\n");
    printf("<<<<< Sending a message from Echo -> Gadget:\n");
    if (!txPackets) {
        fprintf(stderr, "%s: Could not create  message. Exiting", __FUNCTION__);
        exit(1);
    }
    PacketList_PrintAll(txPackets);
    printf("----- Gadget receives the message -----\n");
    packet_list_t *responseList = receivePackets(ROLE_GADGET, txPackets);
    PacketList_freeList(txPackets);

    printf(">>>>> Response from Gadget -> Echo:\n");
    PacketList_PrintAll(responseList);

    printf("----- Echo receives the response -----\n");
    txPackets = responseList;
    responseList = receivePackets(ROLE_ECHO, txPackets);
    // No further responses sent from Echo device.
    assert(responseList == NULL);

    PacketList_freeList(txPackets);
    PacketList_freeList(responseList);
}

packet_list_t *testMyPacketCapturesFromEchoDevice() {
    // Replace these payloads with your own BLE packet captures.
    uint8_t packet1[] = {0x00, 0x00, 0x00, 0x00, 0x02, 0x02, 0x08, 0x14};
    uint8_t packet2[] = {0x02, 0x00, 0x00, 0x00, 0x02, 0x02, 0x08, 0x1c};

    uint8_t *capturedBlePackets[] = {packet1, packet2};
    size_t capturedBlePacketSizes[] = {ARRAY_SIZE(packet1), ARRAY_SIZE(packet2)};

    packet_list_t *packetList = NULL;
    for (size_t i = 0; i < ARRAY_SIZE(capturedBlePackets); i++) {
        packet_t packet = {};
        packet.data = malloc(capturedBlePacketSizes[i]);
        if (!packet.data) {
            fprintf(stderr, "%s: malloc failed\n", __FUNCTION__);
            return packetList;
        }
        memcpy(packet.data, capturedBlePackets[i], capturedBlePacketSizes[i]);
        packet.dataSize = capturedBlePacketSizes[i];
        packetList = PacketList_addToTail(packetList, &packet);
    }
    return packetList;
}

int main(int argc, char *argv[]) {

    runSampleCreateAdvertisingPacket();

    runSampleProtocolVersionPacket();

    runSample(createCommandGetDeviceInformation(), "GetDeviceInformation");

    runSample(createCommandGetDeviceFeatures(), "GetDeviceFeatures");

    runSample(createCommandUpdateComponentSegment(), "UpdateComponentSegment");

    runSample(createCommandApplyFirmware(), "ApplyFirmware");

    runSample(createAlexaDiscoveryDiscoverDirective(), "AlexaDiscovery");

    // Test your packet captures here...
    runSample(testMyPacketCapturesFromEchoDevice(), "TestMyPacketCaptures");

}
