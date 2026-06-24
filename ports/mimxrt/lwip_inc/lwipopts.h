#ifndef MICROPY_INCLUDED_MIMXRT_LWIP_LWIPOPTS_H
#define MICROPY_INCLUDED_MIMXRT_LWIP_LWIPOPTS_H

#define LWIP_NETIF_EXT_STATUS_CALLBACK  1

#define LWIP_IPV6                       0

#define LWIP_RAND() trng_random_u32()

// Larger lwIP memory configuration for video streaming (MJPEG over TCP +
// RTP/JPEG over UDP). The 8KB common default holds only ~5 in-flight 1.4KB
// UDP pbufs, so fragmenting a ~40KB JPEG frame into ~30 RTP packets fails
// with ENOMEM mid-frame. RT1062 has ample RAM; use the high-throughput
// profile from lwipopts_common.h with a heap sized for one full frame burst.
// MEM_SIZE must hold a full JPEG frame queued on the MJPEG TCP socket
// (TCP_SND_BUF) plus a full ~30-packet RTP/UDP frame burst concurrently.
#define MEM_SIZE (128 * 1024)
#define TCP_MSS (1460)
#define TCP_WND (8 * TCP_MSS)
// One whole ~41KB MJPEG frame must fit in the send buffer: the asyncio event
// loop is blocked ~86ms per JPEG encode, so every extra TCP drain cycle costs
// a full encode period and divides the delivered MJPEG frame rate.
#define TCP_SND_BUF (32 * TCP_MSS)
// lwIP derives TCP_SND_QUEUELEN as 4 * TCP_SND_BUF / TCP_MSS by default, so
// keep the segment pool large enough for the 32*MSS send buffer.
#define MEMP_NUM_TCP_SEG (128)

// Relocate the lwIP heap and memp pools out of DTCM (which has no headroom
// for the larger MEM_SIZE above) into the dedicated .lwip section placed in
// DRAM by the board linker config (OMV_LWIP_MEMORY).
#define LWIP_DECLARE_MEMORY_ALIGNED(variable_name, size) \
    __attribute__((section(".lwip"), aligned(MEM_ALIGNMENT))) u8_t variable_name[size]

// Include common lwIP configuration.
#include "extmod/lwip-include/lwipopts_common.h"

extern uint32_t trng_random_u32(void);

#endif // MICROPY_INCLUDED_MIMXRT_LWIP_LWIPOPTS_H
