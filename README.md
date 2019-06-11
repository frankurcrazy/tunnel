Tunnel
======

C tunnel helper.

## Example
```code: c
#include "tunnel.h"

int main() {
    tunnel_t tun;

    tunnel_create(&tun, TUN, "test_tun", 0);
    // DO SOMETHING

    tunnel_close(&tun);
}
```

## Docs
Doxygenerated document can be found [here](docs/html/index.html).
