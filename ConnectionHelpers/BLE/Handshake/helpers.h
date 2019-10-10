//
// Copyright 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
// These materials are licensed under the Amazon Software License in connection with the Alexa Gadgets Program.
// The Agreement is available at https://aws.amazon.com/asl/.
// See the Agreement for the specific terms and conditions of the Agreement.
// Capitalized terms not defined in this file have the meanings given to them in the Agreement.
//

#ifndef ALEXA_GADGETS_SAMPLE_CODE_HELPERS_H
#define ALEXA_GADGETS_SAMPLE_CODE_HELPERS_H

#include <stdlib.h>

#include "common.h"
#include "accessories.pb.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#define ARRAY_SIZE(a) (sizeof((a))/sizeof((a)[0]))

/**
 * Represents a single packet that is exchanged between Echo device and the gadget.
 * The data member need to be malloced on the heap and can be freed using freePacket().
 * @sa freePacket().
 */
typedef struct {
    size_t dataSize;
    uint8_t *data;
} packet_t;

/**
 * A linked list of packets.
 * @sa PacketList_addToTail.
 * @sa PacketList_appendList.
 * @sa PacketList_PrintAll.
 * @sa PacketList_freeList.
 */
typedef struct packet_list_s {
    packet_t packet;
    struct packet_list_s *next;
} packet_list_t;

/**
 * Creates (or appends a packet to) a linked list of packet.
 * @param list set to NULL to create a new list, otherwise it appends to the tail of the linked list
 * pointed to by \p list.
 * @param packet a pointer to a packet structure. This can be allocated on the stack.
 * A copy of the content of the packet structure is duplicated and saved in the list.
 * The data pointer of the packet structure must be malloced on the heap and it is not duplicated.
 * @return  returns a pointer to the new list head if a new list has been created, or the same value as
 * \p list value if the a new \p packet was appended. If \p packet is NULL, the value of \p list is returned as is.
 */
packet_list_t *PacketList_addToTail(packet_list_t *list, packet_t const *packet);

/**
 * Appends the \p src packet list to the tail of the \p dst packet list.
 * @param dst the dst packet list.
 * @param src the source list.
 * @return if src is NULL it returns dst. If dst is a NULL list, it returns the src list,
 * otherwise, it appends the src list to the dst list.
 */
packet_list_t *PacketList_appendList(packet_list_t *dst, packet_list_t *src);

/**
 * Prints hexdump of the contents of all packets in the list.
 * @param list the list to print.
 */
void PacketList_PrintAll(packet_list_t const *list);

/**
 * Frees all packet in the list along with their underlying data buffers.
 * @param list  the list to free.
 */
void PacketList_freeList(packet_list_t *list);

/**
 * Return the number of packets in the list.
 * @param list the list to get its size.
 */
size_t PacketList_getSize(packet_list_t const *list);

/**
 * Free a single packet along with its data buffer.
 * @param packet the packet to free.
 */
void freePacket(packet_t *packet);

/**
 * Prints hexdump of the packet content.
 * @param packet the packet which content to print.
 * @param indentSize the whitespace to leave on each line before printing multiline hex dump for pretty logging.
 */
void printPacket(packet_t const *packet, int indentSize);

/**
 * Maps a Gadgets stream id to a continuous array index.
 * @param streamId value as enumerated in stream_id_t.
 * @return the array index value or -1 if the \p streamId value is not valid.
 */
size_t streamToIndex(stream_id_t streamId);

/**
 * Returns a readable string of the command name.
 * @param command value as enumerated in Command.
 */
char *commandToString(Command command);

/**
 * Prints hexdump of a C buffer.
 * @param buf a pointer to the buffer.
 * @param bufSize buffer size in bytes.
 * @param indentSize the whitespace to leave on each line before printing multiline hex dump for pretty logging.
 */
void printHexBuffer(uint8_t const *buf, size_t bufSize, int indentSize);


#ifdef __cplusplus
}
#endif

#endif // ALEXA_GADGETS_SAMPLE_CODE_HELPERS_H
