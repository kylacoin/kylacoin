How to Upgrade
==============

If you are running an older version, shut it down. Wait until it has completely
shut down (which might take a few minutes in some cases), then run the
installer (on Windows) or just copy over `/Applications/Kylacoin-Qt` (on macOS)
or `kylacoind`/`kylacoin-qt` (on Linux).

Notable changes
===============
### Bugfix
There was a problem that caused the blockchain to accept different types of proof-of-work (PoW) algorithms. This issue has now been resolved.

### Miniscript

- [#29853](https://github.com/bitcoin/bitcoin/pull/29853) sign: don't assume we are parsing a sane TapMiniscript

### RPC

- [#29869](https://github.com/bitcoin/bitcoin/pull/29869) rpc, bugfix: Enforce maximum value for setmocktime
- [#29870](https://github.com/bitcoin/bitcoin/pull/29870) rpc: Reword SighashFromStr error message

### Index

- [#29776](https://github.com/bitcoin/bitcoin/pull/29776) Fix [#29767](https://github.com/bitcoin/bitcoin/pull/29767), set m_synced = true after Commit()

### Test

- [#29892](https://github.com/bitcoin/bitcoin/pull/29892) test: Fix failing univalue float test

### Build

- [#29747](https://github.com/bitcoin/bitcoin/pull/29747) depends: fix mingw-w64 Qt DEBUG=1 build
- [#29859](https://github.com/bitcoin/bitcoin/pull/29859) build: Fix false positive CHECK_ATOMIC test
- [#29985](https://github.com/bitcoin/bitcoin/pull/29985) depends: Fix build of Qt for 32-bit platforms with recent glibc
