//
// Copyright 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
// These materials are licensed under the Amazon Software License in connection with the Alexa Gadgets Program.
// The Agreement is available at https://aws.amazon.com/asl/.
// See the Agreement for the specific terms and conditions of the Agreement.
// Capitalized terms not defined in this file have the meanings given to them in the Agreement.
//

#ifndef ALEXA_GADGETS_SAMPLE_CODE_TX_H
#define ALEXA_GADGETS_SAMPLE_CODE_TX_H

#include "helpers.h"
#include "common.h"
#include "accessories.pb.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Create sample AlexaDiscovery.Discover directive as sent from Echo device.
 * https://developer.amazon.com/docs/alexa-gadgets-toolkit/proto-buffer-format.html#directive-proto-files
 */
packet_list_t *createAlexaDiscoveryDiscoverDirective();

/**
 * Create sample ApplyFirmware response as sent from Gadget.
 * https://developer.amazon.com/docs/alexa-gadgets-toolkit/packet-ble.html#apply-firmware-response
 */
packet_list_t *createResponseApplyFirmware();

/**
 * Create sample error response as sent from Gadget.
 * https://developer.amazon.com/docs/alexa-gadgets-toolkit/packet-ble.html#response
 */
packet_list_t *createResponseError(Command cmd, ErrorCode errorCode, uint16_t tag);

/**
 * Create sample GetDeviceFeatures response as sent from Gadget.
 * https://developer.amazon.com/docs/alexa-gadgets-toolkit/packet-ble.html#device-features-response
 */
packet_list_t *createResponseGetDeviceFeatures();

/**
 * Create sample GetDeviceInformation response as sent from Gadget.
 * https://developer.amazon.com/docs/alexa-gadgets-toolkit/packet-ble.html#device-information-response
 */
packet_list_t *createResponseGetDeviceInformation();

/**
 * Create sample UpdateComponentSegment response as sent from Gadget.
 * https://developer.amazon.com/docs/alexa-gadgets-toolkit/packet-ble.html#update-component-segment-response
 */
packet_list_t *createResponseUpdateComponentSegment();

/**
 * Create sample Alexa.Discovery DiscoveryResponse as sent from Gadget.
 * https://developer.amazon.com/docs/alexa-gadgets-toolkit/proto-buffer-format.html#event-proto-files
 */
packet_list_t *createSampleDiscoveryResponseMessage();

/**
 * Create sample ApplyFirmware command as sent from Echo device.
 */
packet_list_t *createCommandApplyFirmware();

/**
 * Create sample GetDeviceFeatures command as sent from Echo device.
 */
packet_list_t *createCommandGetDeviceFeatures();

/**
 * Create sample GetDeviceInformation command as sent from Echo device.
 */
packet_list_t *createCommandGetDeviceInformation();

/**
 * Create sample UpdateComponentSegment command as sent from Echo device.
 */
packet_list_t *createCommandUpdateComponentSegment();

/**
 * Create sample advertising packet payload as sent from Gadget.
 * https://developer.amazon.com/docs/alexa-gadgets-toolkit/bluetooth-le-settings.html#adv
 */
packet_t createAdvertisingPacket();

/**
 * Create sample control ACK packet as sent from Gadget.
 * https://developer.amazon.com/docs/alexa-gadgets-toolkit/packet-ble.html#ack-packet
 */
packet_t
createControlAckPacket(stream_id_t streamId, transaction_id_t transactionId, bool ack, control_ack_result_t result);

/**
 * Create sample Protocol Version packet as sent from Gadget.
 * https://developer.amazon.com/docs/alexa-gadgets-toolkit/bluetooth-le-settings.html#pvp
 */
packet_t createProtocolVersionPacket();

#ifdef __cplusplus
}
#endif

#endif //ALEXA_GADGETS_SAMPLE_CODE_TX_H
