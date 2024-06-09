How to Upgrade
==============

If you are running an older version, shut it down. Wait until it has completely
shut down (which might take a few minutes in some cases), then run the
installer (on Windows) or just copy over `/Applications/Kylacoin-Qt` (on macOS)
or `kylacoind`/`kylacoin-qt` (on Linux).

Notable changes
===============

### RPC
- #30094 rpc: move UniValue in blockToJSON

### Gui
- #gui812 Fix create unsigned transaction fee bump
- #gui813 Don't permit port in proxy IP option

### P2P
- #30085 p2p: detect addnode cjdns peers in GetAddedNodeInfo()

### Build
- #30097 crypto: disable asan for sha256_sse4 with clang and -O0
- #30151 depends: Fetch miniupnpc sources from an alternative website

### Misc
- #30149 contrib: Renew Windows code signing certificate
- Performance improved when loading the blockchain index