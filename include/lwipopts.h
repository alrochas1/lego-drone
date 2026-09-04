#pragma once

// lwipopts.h - Configuration file for lwIP stack
// This file contains configuration options for the lwIP stack used in the project.
// Needed to be included before including any lwIP headers.

#define NO_SYS 0                    // Enable operating system support (FreeRTOS)
#define SYS_LIGHTWEIGHT_PROT 1      // Enable lightweight protection for critical sections

#define LWIP_SOCKET 0               // Enable socket API
#define LWIP_NETCONN 1              // Enable netconn API

#define LWIP_DHCP 1                 // Enable DHCP support
#define LWIP_DNS 1                  // Enable DNS support

#define LWIP_IPV4 1                 // Enable IPv4 support
#define LWIP_IPV6 0                 // Disable IPv6 support

#define LWIP_UDP 1                  // Enable UDP support
#define LWIP_TCP 0                  // Disable TCP support 

#define MEM_SIZE 16000              // Total heap memory size for lwIP (in bytes)

#define MEMP_NUM_UDP_PCB 4          // Number of UDP protocol control blocks
// #define MEMP_NUM_TCP_PCB 1          // Number of TCP protocol control blocks (unused since TCP is disabled)
// #define MEMP_NUM_TCP_SEG 32         // Number of TCP segments (unused since TCP is disabled)

#define PBUF_POOL_SIZE 16           // Number of buffers in the pbuf pool
#define PBUF_POOL_BUFSIZE 1536      // Size of each buffer in the pbuf pool (in bytes)

#define LWIP_NETIF_HOSTNAME 1       // Enable hostname support for network interfaces

#define LWIP_TIMEVAL_PRIVATE 0      // Don't use private timeval structure (conflict with sockets)

#define LWIP_STATS 0                // Disable statistics collection
#define LWIP_DEBUG 0                // Disable debug output
