/*!
 * \file tunnel.c
 * \author Frank Chang
 * \brief Tunnel implementation
 */

#include "tunnel.h"

/*! \brief Create a tunnel interface
 *  \param tun Pointer to tunnel_t instance.
 *  \param type Tunnel type, TAP or TUN.
 *  \param name Tunnel name, set to NULL to use default name.
 *  \param persist Persistency of tunnel interface
 *  \return 0 on success, -1 on error.
 */
int tunnel_create(
     tunnel_t *tun,
     tunnel_type_t type,
     const char *name,
     bool persist
) { 
    bzero(tun, sizeof(tunnel_t));

    TUNNEL_FD(tun) = open("/dev/net/tun", O_RDWR);
    if (0 > TUNNEL_FD(tun)) {
        PERROR("open()");
        return -1;
    }

    if (name) strncpy(TUNNEL_NAME(tun), name, strlen(name)+1);
    tun->ifr.ifr_flags = (type == TUN? IFF_TUN : IFF_TAP);
    tun->ifr.ifr_flags |= IFF_NO_PI;

    if (0 > ioctl(TUNNEL_FD(tun), TUNSETIFF, &tun->ifr)) {
        PERROR("ioctl()");
        goto clean_exit;
    }
    LOG_DEBUG("Tunnel interface name: %s", TUNNEL_NAME(tun));

    tunnel_set_persistence(tun, persist);

    return 0;

clean_exit:
    tunnel_close(tun);
    return -1;
}

/*! \brief Configure the tunnel persistence.
 *  \param tun Pointer to tunnel instance.
 *  \param persist Persistency of the tunnel interface.
 *  \return 0 on success, -1 on error.
 *
 * If persist is set to true, then tunnel will remain even
 * if the process is killed. Otherwise, the life of the tunnel
 * ends with the process. 
 */
int tunnel_set_persistence(tunnel_t *tun, bool persist) {
    LOG_DEBUG("Set tunnel %s persistency to %d", \
                    TUNNEL_NAME(tun), persist);

    if (0 < ioctl(TUNNEL_FD(tun), TUNSETPERSIST, persist)) {
        PERROR("ioctl()");
        return -1;
    }

    return 0;
}

/*! \brief Delete the tunnel interface.
 *  \param tun Pointer to tunnel instance.
 */
int tunnel_delete(tunnel_t *tun) {
    LOG_DEBUG("Deleting tunnel %s.", TUNNEL_NAME(tun));
    tunnel_set_persistence(tun, 0);
}

/*! \brief Close the tunnel interface.
 *  \param tun Pointer to tunnel interface.
 */
void tunnel_close(tunnel_t *tun) {
    close(TUNNEL_FD(tun));
}

/*! \brief Set the tunnel state to UP
 *  \param tun Pointer to tunnel interface.
 *  \return 0 on success, -1 on error.
 */
int tunnel_set_up(tunnel_t *tun) {
    int sock_fd;

    LOG_DEBUG("Setting tunnel %s to UP.", TUNNEL_NAME(tun));

    sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (0 > sock_fd) {
        PERROR("socket()");
        return -1;
    }

    tun->ifr.ifr_flags |= IFF_UP | IFF_RUNNING;
    if (0 > ioctl(sock_fd, SIOCGIFFLAGS, &tun->ifr)) {
        PERROR("ioctl()");
        goto clean_exit;
    }

    close(sock_fd);
    return 0;

clean_exit:
    close(sock_fd);
    return -1;
}

/*! \brief Set the tunnel state to DOWN
 *  \param tun Pointer to tunnel interface.
 *  \return 0 on success, -1 on error.
 */
int tunnel_set_down(tunnel_t *tun) {
    int sock_fd;

    LOG_DEBUG("Setting tunnel %s to UP.", TUNNEL_NAME(tun));

    sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (0 > sock_fd) {
        PERROR("socket()");
        return -1;
    }

    tun->ifr.ifr_flags &= ~(IFF_UP | IFF_RUNNING);
    if (0 > ioctl(sock_fd, SIOCGIFFLAGS, &tun->ifr)) {
        PERROR("ioctl()");
        goto clean_exit;
    }

    close(sock_fd);
    return 0;

clean_exit:
    close(sock_fd);
    return -1;
}
