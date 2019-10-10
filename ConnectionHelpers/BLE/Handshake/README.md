## `compile_nanos` Shell and Batch Scripts
The `compile_nanos` script will compile all the `.proto` files in this repository 
and deposit the C and header source files into this folder. To run the script, 
open a shell to the `Handshake` folder and edit the `compile_nano.sh` (macOS/Linux) 
or `compile_nanos.bat` (Windows). Within the script, update the `PROTO_COMPILE_PATH`
variable to point to the install location of Nanopb (Nanopb is a requirement for 
these scripts and can be found here https://github.com/nanopb/nanopb). Run the 
script from the `Handshake` folder. All generated source code C and Header files 
will be deposited in the `Handshake` folder.

## sample.c, tx.c and rx.c

The files `sample.c`, `tx.c` and `rx.c` demonstrate the encoding and decoding of the packets
during the handshake between your BLE gadget and the Echo device.

In `sample.c`, you will find sample code that runs simulated 
handshake scenario for the following messages/packets:

1. Sample BLE advertising packet.
2. Protocol Version `PV` packet.
3. `GetDeviceInformation` command and sample response.
4. `GetDeviceFeatures` command and sample response.
5. `UpdateComponentSegment` command and sample response.
6. `ApplyFirmware` command and sample response.
7. `AlexaDiscovery` `Discover` directive sample response.
8. Decode your BLE packet captures as received from Echo device.

When each sample runs, it prints the BLE packet payload exchanged 
between the gadget and the Echo device during the handshake.

### Building the sample code

#### 1. Copy supporting source files. 

In order to prepare the sample, copy the following files from your Nanopb 
installation folder into the same Handshake folder:
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

#### 3. Compile the all source files
Run the following gcc command in Handshake folder:

```gcc -I. -DPB_FIELD_16BIT  *.c -o sample```

#### 4. Run the executable file:

On Linux/macOS, run ```./sample``` or on Windows run ```sample.exe```

### (Optional) Configuring the parameters

The sample program uses configurations declared in `config.h` where it sets 
the Maximum Transfer Unit (MTU) size and the maximum transaction size. 
```
#define SAMPLE_MAX_TRANSACTION_SIZE (5000U)
#define SAMPLE_NEGOTIATED_MTU       (128U)
```
You can modify these configurations and rebuild the sample as needed. 
