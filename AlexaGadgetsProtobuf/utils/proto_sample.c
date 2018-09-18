#include<stdio.h>
#include "pb.h"
#include "pb_decode.h"
#include "pb_encode.h"
#include "directiveParser.pb.h"
#include "notificationsSetIndicatorDirective.pb.h"

typedef unsigned char uint8_t;
typedef unsigned char BOOL;

void parse_directive_proto(uint8_t* buffer, int len)
{
    pb_istream_t stream = pb_istream_from_buffer(buffer, len);
    directive_DirectiveParserProto envelope = directive_DirectiveParserProto_init_default;

    pb_decode(&stream, directive_DirectiveParserProto_fields, &envelope);

    printf("name = %s, namespace=%s\n", envelope.directive.header.name, envelope.directive.header.namespace);      


    //Parse the notification payload now 
    pb_istream_t stream_notification = pb_istream_from_buffer(buffer, len);
    notifications_SetIndicatorDirectiveProto notifications_envelope = notifications_SetIndicatorDirectiveProto_init_default;
    pb_decode(&stream_notification, notifications_SetIndicatorDirectiveProto_fields, &notifications_envelope);

    printf("visualIndicator:%d, audioIndicator=%d, assetId=%s, url=%s\n",
    notifications_envelope.directive.payload.persistVisualIndicator,
    notifications_envelope.directive.payload.playAudioIndicator,
    notifications_envelope.directive.payload.asset.assetId,
    notifications_envelope.directive.payload.asset.url);
}


void serialize_to_proto(BOOL audio, BOOL visual, char* assetId, char* url)
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

    printf("\nbytes written:%zu\n", stream.bytes_written);
    uint8_t index;
    for(index = 0; index < stream.bytes_written; index++)
    {
        printf("0x%02x ", buffer[index]);
    }
    printf("\n");
}

// Sample byte array that represents a serialized protobuf message
int main(int argc, char** argv)
{

    printf("%s\n", __FUNCTION__);
    uint8_t buffer[] = {
    		 0x0a, 0x37, 0x0a, 0x1d, 0x0a, 0x0d, 0x4e, 0x6f, 0x74, 0x69,
		 0x66, 0x69, 0x63, 0x61, 0x74, 0x69, 0x6f, 0x6e, 0x73, 0x12,
		 0x0c, 0x53, 0x65, 0x74, 0x49, 0x6e, 0x64, 0x69, 0x63, 0x61,
		 0x74, 0x6f, 0x72, 0x12, 0x16, 0x08, 0x01, 0x10, 0x01, 0x1a,
		 0x10, 0x0a, 0x08, 0x61, 0x73, 0x73, 0x65, 0x74, 0x49, 0x44,
		 0x31, 0x12, 0x04, 0x75, 0x72, 0x6c, 0x31
                 };
    parse_directive_proto(buffer, sizeof(buffer));
    serialize_to_proto(1, 1, "assetID1", "url1");
    return 0;
}
