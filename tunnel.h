/*!
 * \file tunnel.h
 * \author Frank Chang
 * \brief Tunnel header file
 */

#ifndef TUNNEL_H
#define TUNNEL_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <strings.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h>
#include <net/if.h>
#include <linux/if_tun.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>

#define DEBUG

#define TUNNEL_NAME(x) (x->ifr.ifr_name)
#define TUNNEL_FD(x) (x->fd)
#define LOG_ERROR(fmt, ...) fprintf(stderr, "[ERROR][%s:%d:%s()] "fmt"\n", \
                __FILE__, __LINE__, __func__, ##__VA_ARGS__);

#ifdef DEBUG
#define LOG_DEBUG(fmt, ...) fprintf(stderr, "[DEBUG][%s:%d:%s()] "fmt"\n", \
                __FILE__, __LINE__, __func__, ##__VA_ARGS__);
#else
#define LOG_DEBUG(fmt, ...) {}
#endif

#define PERROR(msg) LOG_ERROR(msg": %s", strerror(errno));


/*! \brief Tunnel type
 *  
 *  TAP is L2 tunnel while TUN is L3 tunnel
 */
typedef enum tunnel_type {
    TAP,
    TUN
} tunnel_type_t;

/*! \brief Tunnel structure
 */
typedef struct tunnel {
    int fd;                 /**< Tunnel file descriptor. */
    tunnel_type_t type;     /**< Tunnel type. */
    struct ifreq ifr;       /**< Parameter for ioctl() system call. */
} tunnel_t;

int tunnel_create(tunnel_t *tun, tunnel_type_t type, const char *name, bool persist);
int tunnel_set_up(tunnel_t *tun);
int tunnel_set_down(tunnel_t *tun);
int tunnel_set_persistence(tunnel_t *tun, bool persist);
int tunnel_delete(tunnel_t *tun);
void tunnel_close(tunnel_t *tun);

#endif
