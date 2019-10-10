## `compile_nanos` Shell and Batch Scripts
The `compile_nanos` script will compile all the `.proto` files in this repository and deposit the C and header source files 
into this folder. To run the script, open a shell to the `examples` folder and edit the `compile_nano.sh` (macOS/Linux) or 
`compile_nanos.bat` (Windows). Within the script, update the `PROTO_COMPILE_PATH` variable to point to the install location 
of Nanopb (Nanopb is a requirement for these scripts and can be found here https://github.com/nanopb/nanopb). 
Run the script from the `examples` folder. All generated source code C and Header files will be deposited in the 
`examples` folder.

## proto_sample.c

This sample code file proto_sample.c demonstrates how to encode / decode each interface.
For example, the sample code has the following proto message to encode:
notificationsSetIndicatorDirective.proto, represented by the following JSON:
```json
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
```

It will call `encode_set_indicator_directive()` to create the binary and print it out. 
You can also see the following functions for other interfaces:
```
encode_sample_discover_response_event()
encode_sample_discover_directive()
encode_sample_state_update_directive()
encode_sample_speechmarks_directive()
```

You can also use `decode_directive()` or `decode_event()` to test out your own protobuf encoded binary. 
For example, the sample code has `notification_binary[]` hardcoded, and pass it to `decode_directive()`.

The `encode_sample*` functions will decode themselves after encoding.


### Building proto_sample.c

#### 1. Copy supporting source files
In order to prepare the sample, copy the following files from your Nanopb installation folder into the same folder where
`proto_sample.c` resides:
```
pb.h
pb_common.c
pb_common.h
pb_decode.c
pb_decode.h
pb_encode.c
pb_encode.h
```

#### 2. Execute the shell script `compile_nanos.sh` or `compile_nanos.bat`

This step will generate the .c/.h files of all the proto/option files shipped.

#### 3. Compile the proto_sample.c, use the following gcc command:

```gcc -I. -DPB_FIELD_16BIT *.c  -o sample```

#### 4. Run the executable file:

On Linux/macOS, run ```./sample``` or on Windows run ```sample.exe```

Modify `proto_sample.c` to test out your own decoding / encoding and re-run the steps above as needed. 

### Modifying statically allocated data size (optional)

In the proto definition, we provided option files for each payload. This option file specifies the max_size for each 
field. As a result, the proto compiler will statically allocate the field for you. For example, in the option file of 
Discover Response event:
```
alexaDiscovery.DiscoverResponseEventPayloadProto.Endpoints.capabilities    max_count:32
```

This defines the size of the capabilities array. In the generated code, 32 capabilities will be statically allocated. 
You can try to change it and regenerate the code. This indicates what is the `max size` supported on Echo devices. 
If you increase the max value, the decoding may fail. To optimize your memory usage, you can decrease the `max_size` or 
`max_count`, so less memory will be statically allocated to the proto buf message you are trying to create.
