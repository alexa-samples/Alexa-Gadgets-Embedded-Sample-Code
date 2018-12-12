# Alexa Gadgets Sample Code

This repository contains resources for Alexa Gadgets developers. For information on how to get started, developer documentation, and API references, please visit the [Alexa Gadgets Toolkit developer pages](https://developer.amazon.com/docs/alexa-gadgets-toolkit/understand-alexa-gadgets-toolkit.html).

## License

This library is licensed under the Amazon Software License.

## Folders
### /AlexaGadgetsProtobuf

This folder contains the protocol buffer (protobuf) message definition files and scripts to compile the corresponding serializer/deserializers.

#### /AlexaGadgetsProtobuf/Utils

This folder contains an example of a single protobuf deinfition and a shell script to compile all of the protobuf definitions for the Alexa Gadgets Toolkit.

### /ConnectionHelpers

This folder contains a collection of code snippets to help with establishing the Bluetooth pairing and connections between a gadget and a compatible Amazon Echo device.

#### /ConnectionHelpers/Checksum

This folder contains sample code for calculating the checksum of the packets exhanged. For more information, see [Exchanging Packets](https://developer.amazon.com/docs/alexa-gadgets-toolkit/exchange-packets.html).

#### /ConnectionHelpers/DeviceSecret

This folder contains sample code for generating the device secret using SHA256. These credentials are required in order to connect a gadget to a compatible Amazon Echo device. Fore more information on how to sue the device secret in the initial handshake, please see [Bluetooth pairing and connecting](https://developer.amazon.com/docs/alexa-gadgets-toolkit/pair-connect-bluetooth.html).

#### /ConnectionHelpers/EIR

This folder contains sample code for the Extended Inquiry Response (EIR). This is the first step in pairing a gadget with an Echo. For more information on the EIR, please see [Extended Inquiry Response](https://developer.amazon.com/docs/alexa-gadgets-toolkit/gadget-settings-bluetooth-communication.html#eir).

#### /ConnectionHelpers/SDP

This folder contains sample code for the Service Discover Protocol (SDP) database settings that must be added to the SDP database in order for your gadget to establish a bluetooth connection. For more information please see [SDP Settings](https://developer.amazon.com/docs/alexa-gadgets-toolkit/gadget-settings-bluetooth-communication.html#sdp-database).

#### /ConnectionHelpers/PacketHelper

This folder contains sample code for extracting packets from streaming buffers. Your Bluetooth connection is a stream based protocol, so there is no guarantee that you receive one packet at a time. For more information please see [Exchange Packets](https://developer.amazon.com/docs/alexa-gadgets-toolkit/exchange-packets.html).
