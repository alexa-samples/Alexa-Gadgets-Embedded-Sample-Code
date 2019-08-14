//
// Copyright 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
// These materials are licensed under the Amazon Software License in connection with the Alexa Gadgets Program.
// The Agreement is available at https://aws.amazon.com/asl/.
// See the Agreement for the specific terms and conditions of the Agreement.
// Capitalized terms not defined in this file have the meanings given to them in the Agreement.
//
#include<stdio.h>
#include "pb.h"
#include "pb_decode.h"
#include "pb_encode.h"
#include "directiveParser.pb.h"
#include "eventParser.pb.h"
#include "notificationsSetIndicatorDirective.pb.h"
#include "alexaDiscoveryDiscoverResponseEvent.pb.h"
#include "alexaDiscoveryDiscoverDirective.pb.h"
#include "alexaGadgetStateListenerStateUpdateDirective.pb.h"
#include "alexaGadgetSpeechDataSpeechmarksDirective.pb.h"

typedef unsigned char uint8_t;
typedef unsigned char BOOL;

// Decode proto buf encoded directive binary and print out the content.
void decode_directive(uint8_t* buffer, int len)
{
    printf("\nParsing Directive:\n");
    pb_istream_t stream = pb_istream_from_buffer(buffer, len);
    directive_DirectiveParserProto envelope = directive_DirectiveParserProto_init_default;

    pb_decode(&stream, directive_DirectiveParserProto_fields, &envelope);

    printf("name = %s, namespace=%s\n", envelope.directive.header.name, envelope.directive.header.namespace);

    if (0 == strcmp(envelope.directive.header.name, "SetIndicator")
            && (0 == strcmp(envelope.directive.header.namespace, "Notifications"))) {
        // Parse the notification payload now
        pb_istream_t stream_notification = pb_istream_from_buffer(buffer, len);
        notifications_SetIndicatorDirectiveProto notifications_envelope = notifications_SetIndicatorDirectiveProto_init_default;
        pb_decode(&stream_notification, notifications_SetIndicatorDirectiveProto_fields, &notifications_envelope);

        printf("visualIndicator:%d, audioIndicator=%d, assetId=%s, url=%s\n",
        notifications_envelope.directive.payload.persistVisualIndicator,
        notifications_envelope.directive.payload.playAudioIndicator,
        notifications_envelope.directive.payload.asset.assetId,
        notifications_envelope.directive.payload.asset.url);
    } else if (0 == strcmp(envelope.directive.header.name, "Discover")
            && (0 == strcmp(envelope.directive.header.namespace, "Alexa.Discovery"))) {
        pb_istream_t stream = pb_istream_from_buffer(buffer, len);
        alexaDiscovery_DiscoverDirectiveProto envelope = alexaDiscovery_DiscoverDirectiveProto_init_default;
        pb_decode(&stream, alexaDiscovery_DiscoverDirectiveProto_fields, &envelope);
        printf("scope type: %s\n", envelope.directive.payload.scope.type);
        printf("scope token: %s\n", envelope.directive.payload.scope.token);
    } else if (0 == strcmp(envelope.directive.header.name, "StateUpdate")
            && (0 == strcmp(envelope.directive.header.namespace, "Alexa.Gadget.StateListener"))) {
        pb_istream_t stream = pb_istream_from_buffer(buffer, len);
        alexaGadgetStateListener_StateUpdateDirectiveProto envelope = alexaGadgetStateListener_StateUpdateDirectiveProto_init_default;
        pb_decode(&stream, alexaGadgetStateListener_StateUpdateDirectiveProto_fields, &envelope);
        int states_count = envelope.directive.payload.states_count;
        for (int i = 0; i < states_count; ++i) {
            printf("state name: %s\n", envelope.directive.payload.states[i].name);
            printf("state value: %s\n", envelope.directive.payload.states[i].value);
        }
    } else if (0 == strcmp(envelope.directive.header.name, "Speechmarks")
            && (0 == strcmp(envelope.directive.header.namespace, "Alexa.Gadget.SpeechData"))) {
        pb_istream_t stream = pb_istream_from_buffer(buffer, len);
        alexaGadgetSpeechData_SpeechmarksDirectiveProto envelope = alexaGadgetSpeechData_SpeechmarksDirectiveProto_init_default;
        pb_decode(&stream, alexaGadgetSpeechData_SpeechmarksDirectiveProto_fields, &envelope);
        int speechmarks_count = envelope.directive.payload.speechmarksData_count;
        printf("player offset: %d\n", envelope.directive.payload.playerOffsetInMilliSeconds);
        for (int i = 0; i < speechmarks_count; ++i) {
            printf("speechmark type: %s\n", envelope.directive.payload.speechmarksData[i].type);
            printf("speechmark value: %s\n", envelope.directive.payload.speechmarksData[i].value);
            printf("speechmark start offset: %d\n", envelope.directive.payload.speechmarksData[i].startOffsetInMilliSeconds);
        }
    } else {
        printf("Error: do not have parsing code for this directive, check directive name\n");
    }
}

// Decode proto buf encoded event binary and print out the content.
void decode_event(uint8_t* buffer, int len)
{
    printf("\nParsing Event:\n");
    pb_istream_t stream = pb_istream_from_buffer(buffer, len);
    event_EventParserProto envelope = event_EventParserProto_init_default;

    pb_decode(&stream, event_EventParserProto_fields, &envelope);

    printf("name = %s, namespace=%s\n", envelope.event.header.name, envelope.event.header.namespace);

    if (0 == strcmp(envelope.event.header.name, "Discover.Response")
            && 0 == strcmp(envelope.event.header.namespace, "Alexa.Discovery")) {
        // Parse the discover response event binary
        pb_istream_t stream_discover_response = pb_istream_from_buffer(buffer, len);
        alexaDiscovery_DiscoverResponseEventProto discover_response_envelope =
                alexaDiscovery_DiscoverResponseEventProto_init_default;
        pb_decode(&stream_discover_response, alexaDiscovery_DiscoverResponseEventProto_fields, &discover_response_envelope);
        int endpoints_count = discover_response_envelope.event.payload.endpoints_count;
        printf("endpoint count : %d\n", endpoints_count);
        int endpoint_index;
        for (endpoint_index = 0; endpoint_index < endpoints_count; ++endpoint_index) {
            printf("endpoint id: %s\n", discover_response_envelope.event.payload.endpoints[endpoint_index].endpointId);
            printf("friendly name: %s\n", discover_response_envelope.event.payload.endpoints[endpoint_index].friendlyName);
            printf("description: %s\n", discover_response_envelope.event.payload.endpoints[endpoint_index].description);
            printf("manufacturer name: %s\n", discover_response_envelope.event.payload.endpoints[endpoint_index].manufacturerName);
            int capabilities_count = discover_response_envelope.event.payload.endpoints[endpoint_index].capabilities_count;
            int capability_index;
            for (capability_index = 0; capability_index < capabilities_count; ++capability_index) {
                printf("capability type: %s\n", discover_response_envelope.event.payload.endpoints[endpoint_index]
                        .capabilities[capability_index].type);
                printf("capability interface: %s\n", discover_response_envelope.event.payload.endpoints[endpoint_index]
                        .capabilities[capability_index].interface);
                printf("capability version: %s\n", discover_response_envelope.event.payload.endpoints[endpoint_index]
                        .capabilities[capability_index].version);
            }
            printf("additional id - firmware version: %s\n", discover_response_envelope.event.payload.endpoints[endpoint_index]
                    .additionalIdentification.firmwareVersion);
            printf("additional id - device token: %s\n", discover_response_envelope.event.payload.endpoints[endpoint_index]
                    .additionalIdentification.deviceToken);
            printf("additional id - device token encryption type: %s\n", discover_response_envelope.event.payload.endpoints[endpoint_index]
                    .additionalIdentification.deviceTokenEncryptionType);
            printf("additional id - amazon device type: %s\n", discover_response_envelope.event.payload.endpoints[endpoint_index]
                    .additionalIdentification.amazonDeviceType);
            printf("additional id - model name: %s\n", discover_response_envelope.event.payload.endpoints[endpoint_index]
                    .additionalIdentification.modelName);
            printf("additional id - radioAddress: %s\n", discover_response_envelope.event.payload.endpoints[endpoint_index]
                    .additionalIdentification.radioAddress);
        }
    }
}


// Encode set indicator directive and print out the encoded binary in hex.
void encode_set_indicator_directive(BOOL audio, BOOL visual, char* assetId, char* url)
{
/*
{
    "directive": {
        "header": {
            "namespace": "Notifications",
            "name": "SetIndicator",
            "messageId": "messageID1"
        },
        "payload": {
            "persistVisualIndicator": true,
            "playAudioIndicator": true,
            "asset": {
                "assetId": "assetID1",
                "url": "url1"
            }
        }
    }
}
*/

    printf("\nCreating setindicator directive:\n");
    uint8_t buffer[256];
    pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer));
    notifications_SetIndicatorDirectiveProto notifications_envelope = notifications_SetIndicatorDirectiveProto_init_default;

    strcpy(notifications_envelope.directive.header.namespace, "Notifications");
    strcpy(notifications_envelope.directive.header.name, "SetIndicator");

    notifications_envelope.directive.payload.persistVisualIndicator = visual;
    notifications_envelope.directive.payload.playAudioIndicator = audio;
    strcpy(notifications_envelope.directive.payload.asset.assetId, assetId);
    strcpy(notifications_envelope.directive.payload.asset.url, url);

    BOOL status = pb_encode(&stream, notifications_SetIndicatorDirectiveProto_fields, &notifications_envelope);
    if (!status)
    {
      printf("%s: Error encoding message\n", __FUNCTION__);
      return;
    }

    printf("bytes written:%zu\n", stream.bytes_written);
    uint8_t index;
    for(index = 0; index < stream.bytes_written; index++)
    {
        printf("0x%02x ", buffer[index]);
    }
    printf("\n");
}

void encode_sample_discover_response_event()
{
    /*
    {
        "event": {
            "header": {
                "namespace": "Alexa.Discovery",
                "name": "Discover.Response",
                "messageId": ""
            },
            "payload": {
                "endpoints": [{
                    "endpointId": "test id",
                    "friendlyName": "friendly name",
                    "capabilities": [
                    {
                        "type": "test type 1",
                        "interface": "Test interface 1",
                        "version": "1.0"
                    },
                    {
                        "type": "test type 2",
                        "interface": "Test interface 2",
                        "version": "1.0"
                    },
                    {
                        "type": "test type 3",
                        "interface": "Test interface 3",
                        "version": "1.1"
                    }],
                    "additionalIdentification": {
                        "firmwareVersion": "19",
                        "deviceToken": "xxxxxxxxx",
                        "deviceTokenEncryptionType": "yyy",
                        "amazonDeviceType": "aabbccd",
                        "modelName": "mock model name",
                        "radioAddress": "1234567890"
                    }
                }]
            }
        }
    }
    */
    printf("\nCreating discover response event:\n");
    uint8_t buffer[256];
    pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer));
    alexaDiscovery_DiscoverResponseEventProto discover_response_envelope =
           alexaDiscovery_DiscoverResponseEventProto_init_default;

    strcpy(discover_response_envelope.event.header.namespace, "Alexa.Discovery");
    strcpy(discover_response_envelope.event.header.name, "Discover.Response");

    discover_response_envelope.event.payload.endpoints_count = 1;
    strcpy(discover_response_envelope.event.payload.endpoints[0].endpointId, "test id");
    strcpy(discover_response_envelope.event.payload.endpoints[0].friendlyName, "friendly name");
    discover_response_envelope.event.payload.endpoints[0].capabilities_count = 3;
    strcpy(discover_response_envelope.event.payload.endpoints[0].capabilities[0].type, "test type 1");
    strcpy(discover_response_envelope.event.payload.endpoints[0].capabilities[0].interface, "Test interface 1");
    strcpy(discover_response_envelope.event.payload.endpoints[0].capabilities[0].version, "1.0");
    strcpy(discover_response_envelope.event.payload.endpoints[0].capabilities[1].type, "test type 2");
    strcpy(discover_response_envelope.event.payload.endpoints[0].capabilities[1].interface, "Test interface 2");
    strcpy(discover_response_envelope.event.payload.endpoints[0].capabilities[1].version, "1.0");
    strcpy(discover_response_envelope.event.payload.endpoints[0].capabilities[2].type, "test type 3");
    strcpy(discover_response_envelope.event.payload.endpoints[0].capabilities[2].interface, "Test interface 3");
    strcpy(discover_response_envelope.event.payload.endpoints[0].capabilities[2].version, "1.1");
    strcpy(discover_response_envelope.event.payload.endpoints[0].additionalIdentification.firmwareVersion, "19");
    strcpy(discover_response_envelope.event.payload.endpoints[0].additionalIdentification.deviceToken, "xxxxxxxxx");
    strcpy(discover_response_envelope.event.payload.endpoints[0].additionalIdentification.deviceTokenEncryptionType, "yyy");
    strcpy(discover_response_envelope.event.payload.endpoints[0].additionalIdentification.amazonDeviceType, "aabbccd");
    strcpy(discover_response_envelope.event.payload.endpoints[0].additionalIdentification.modelName, "mock model name");
    strcpy(discover_response_envelope.event.payload.endpoints[0].additionalIdentification.radioAddress, "1234567890");

    BOOL status = pb_encode(&stream, alexaDiscovery_DiscoverResponseEventProto_fields, &discover_response_envelope);
    if (!status)
    {
      printf("%s: Error encoding message\n", __FUNCTION__);
      return;
    }

    printf("bytes written:%zu\n", stream.bytes_written);
    uint8_t index;
    for(index = 0; index < stream.bytes_written; index++)
    {
        printf("0x%02x ", buffer[index]);
    }
    printf("\n");
    decode_event(buffer, stream.bytes_written);
}

void encode_sample_discover_directive()
{
    /*
    {
        "directive": {
            "header": {
                "namespace": "Alexa.Discovery",
                "name": "Discover",
                "messageId": ""
            },
            "payload": {
                "scope": {
                    "type": "",
                    "token": ""
                }
            }
        }
    }
    */

    printf("\nCreating discover directive:\n");
    uint8_t buffer[256];
    pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer));
    alexaDiscovery_DiscoverDirectiveProto envelope = alexaDiscovery_DiscoverDirectiveProto_init_default;

    strcpy(envelope.directive.header.namespace, "Alexa.Discovery");
    strcpy(envelope.directive.header.name, "Discover");

    // discover directive has empty payload right now
    BOOL status = pb_encode(&stream, alexaDiscovery_DiscoverDirectiveProto_fields, &envelope);
    if (!status)
    {
      printf("%s: Error encoding message\n", __FUNCTION__);
      return;
    }

    printf("bytes written:%zu\n", stream.bytes_written);
    uint8_t index;
    for(index = 0; index < stream.bytes_written; index++)
    {
        printf("0x%02x ", buffer[index]);
    }
    printf("\n");
    decode_directive(buffer, stream.bytes_written);
}

void encode_sample_state_update_directive()
{
    /*
    {
        "directive": {
            "header": {
                "namespace": "Alexa.Gadget.StateListener",
                "name": "StateUpdate",
                "messageId": ""
            },
            "payload": {
                "states": [{
                    "name": "timers",
                    "value": "active"
                }]
            }
        }
    }
    */

    printf("\nCreating state update directive:\n");
    uint8_t buffer[256];
    pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer));
    alexaGadgetStateListener_StateUpdateDirectiveProto envelope = alexaGadgetStateListener_StateUpdateDirectiveProto_init_default;

    strcpy(envelope.directive.header.namespace, "Alexa.Gadget.StateListener");
    strcpy(envelope.directive.header.name, "StateUpdate");
    envelope.directive.payload.states_count = 1;
    strcpy(envelope.directive.payload.states[0].name, "timers");
    strcpy(envelope.directive.payload.states[0].value, "active");

    // discover directive has empty payload right now
    BOOL status = pb_encode(&stream, alexaGadgetStateListener_StateUpdateDirectiveProto_fields, &envelope);
    if (!status)
    {
      printf("%s: Error encoding message\n", __FUNCTION__);
      return;
    }

    printf("bytes written:%zu\n", stream.bytes_written);
    uint8_t index;
    for(index = 0; index < stream.bytes_written; index++)
    {
        printf("0x%02x ", buffer[index]);
    }
    printf("\n");
    decode_directive(buffer, stream.bytes_written);
}

void encode_sample_speechmarks_directive()
{
    /*
    {
        "directive": {
            "header": {
                "namespace": "Alexa.Gadget.SpeechData",
                "name": "Speechmarks",
                "messageId": ""
            },
            "payload": {
                "playerOffsetInMilliSeconds": 0
                "speechmarksData": [{
                    "type": "viseme",
                    "value": "s",
                    "startOffsetInMilliSeconds": 130
                }]
            }
        }
    }
    */

    printf("\nCreating speechmarks directive:\n");
    uint8_t buffer[256];
    pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer));
    alexaGadgetSpeechData_SpeechmarksDirectiveProto envelope = alexaGadgetSpeechData_SpeechmarksDirectiveProto_init_default;

    strcpy(envelope.directive.header.namespace, "Alexa.Gadget.SpeechData");
    strcpy(envelope.directive.header.name, "Speechmarks");
    envelope.directive.payload.playerOffsetInMilliSeconds = 0;
    envelope.directive.payload.speechmarksData_count = 1;
    strcpy(envelope.directive.payload.speechmarksData[0].type, "viseme");
    strcpy(envelope.directive.payload.speechmarksData[0].value, "s");
    envelope.directive.payload.speechmarksData[0].startOffsetInMilliSeconds = 130;

    // discover directive has empty payload right now
    BOOL status = pb_encode(&stream, alexaGadgetSpeechData_SpeechmarksDirectiveProto_fields, &envelope);
    if (!status)
    {
      printf("%s: Error encoding message\n", __FUNCTION__);
      return;
    }

    printf("bytes written:%zu\n", stream.bytes_written);
    uint8_t index;
    for(index = 0; index < stream.bytes_written; index++)
    {
        printf("0x%02x ", buffer[index]);
    }
    printf("\n");
    decode_directive(buffer, stream.bytes_written);
}


// Sample byte array that represents a serialized protobuf message
int main(int argc, char** argv)
{
    printf("\nNotifications - SetIndicator Example\n");
    // decoding and encoding of notifcation - set indicator directive
    uint8_t notification_binary[] = {
    		 0x0a, 0x37, 0x0a, 0x1d, 0x0a, 0x0d, 0x4e, 0x6f, 0x74, 0x69,
		 0x66, 0x69, 0x63, 0x61, 0x74, 0x69, 0x6f, 0x6e, 0x73, 0x12,
		 0x0c, 0x53, 0x65, 0x74, 0x49, 0x6e, 0x64, 0x69, 0x63, 0x61,
		 0x74, 0x6f, 0x72, 0x12, 0x16, 0x08, 0x01, 0x10, 0x01, 0x1a,
		 0x10, 0x0a, 0x08, 0x61, 0x73, 0x73, 0x65, 0x74, 0x49, 0x44,
		 0x31, 0x12, 0x04, 0x75, 0x72, 0x6c, 0x31};
    decode_directive(notification_binary, sizeof(notification_binary));

    // creating sample set indicator directive
    encode_set_indicator_directive(1, 1, "assetID1", "url1");

    printf("\nAlexa.Discovery - Discover.Response Example\n");
    encode_sample_discover_response_event();

    printf("\nAlexa.Discovery - Discover Example\n");
    encode_sample_discover_directive();

    printf("\nAlexa.Gadget.StateListener - StateUpdate Example\n");
    encode_sample_state_update_directive();

    printf("\nAlexa.Gadget.SpeechData - Speechmarks Example\n");
    encode_sample_speechmarks_directive();

    return 0;
}
