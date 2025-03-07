How to Upgrade
==============

If you are running an older version, shut it down. Wait until it has completely
shut down (which might take a few minutes in some cases), then run the
installer (on Windows) or just copy over `/Applications/Kylacoin-Qt` (on macOS)
or `kylacoind`/`kylacoin-qt` (on Linux).

Upgrading directly from a version of Kylacoin Core that has reached its EOL is
possible, but it might take some time if the data directory needs to be migrated. Old
wallet versions of Kylacoin Core are generally supported.

Running Kylacoin Core binaries on macOS requires self signing.
```
cd /path/to/kylacoin-4.1.0/bin
xattr -d com.apple.quarantine kylacoin-cli kylacoin-qt kylacoin-tx kylacoin-util kylacoin-wallet kylacoind test_kylacoin
codesign -s - kylacoin-cli kylacoin-qt kylacoin-tx kylacoin-util kylacoin-wallet kylacoind test_kylacoin
```
Changes
===============
Minor changes / Bugfix
