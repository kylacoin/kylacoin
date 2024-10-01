How to Upgrade
==============

If you are running an older version, shut it down. Wait until it has completely
shut down (which might take a few minutes in some cases), then run the
installer (on Windows) or just copy over `/Applications/Kylacoin-Qt` (on macOS)
or `kylacoind`/`kylacoin-qt` (on Linux).

Compatibility
==============

Kylacoin Core is supported and extensively tested on operating systems
using the Linux Kernel 3.17+, macOS 11.0+, and Windows 7 and newer. Kylacoin
Core should also work on most other Unix-like systems but is not as
frequently tested on them. It is not recommended to use Kylacoin Core on
unsupported systems.

Notable changes
===============

Synchronization
---------------
Fixed sync problems with the Flex algorithm on slower CPUs and added
an SQLite3 cache to make data access faster.

Windows Data Directory
----------------------

The default data directory on Windows has been moved from `C:\Users\Username\AppData\Roaming\Kylacoin`
to `C:\Users\Username\AppData\Local\Kylacoin`. Kylacoin Core will check the existence
of the old directory first and continue to use that directory for backwards
compatibility if it is present. ([#27064](https://github.com/bitcoin/bitcoin/pull/27064))

Updated RPCs
------------

- The JSON-RPC server now recognizes JSON-RPC 2.0 requests and responds with
strict adherence to the [specification](https://www.jsonrpc.org/specification).
See [JSON-RPC-interface.md](https://github.com/bitcoin/bitcoin/blob/master/doc/JSON-RPC-interface.md#json-rpc-11-vs-20) for details.

- The `dumptxoutset` RPC now returns the UTXO set dump in a new and
  improved format. At the same time the `loadtxoutset` RPC now
  expects this new format in dumps it tries to load. Dumps with the
  old format are no longer supported and need to be recreated using
  the new format in order to be usable.

- The `warnings` field in `getblockchaininfo`, `getmininginfo` and
  `getnetworkinfo` now returns all the active node warnings as an array
  of strings, instead of just a single warning. The current behaviour
  can temporarily be restored by running kylacoind with configuration
  option `-deprecatedrpc=warnings`.

- Previously when using the `sendrawtransaction` rpc and specifying outputs
  that are already in the UXTO set an RPC error code `-27` with RPC error
  text "Transaction already in block chain" was returned in response.
  The help text has been updated to "Transaction outputs already in utxo set"
  to more accurately describe the source of the issue.

- The default mode for the `estimatesmartfee` RPC has been updated from `conservative` to `economical`.
  which is expected to reduce overestimation for many users, particularly if Replace-by-Fee is an option.
  For users that require high confidence in their fee estimates at the cost of potentially overestimating,
  the `conservative` mode remains available.

- An item of `unspents`, of `scantxoutset`, has two new fields: `blockhash`
  and `confirmations`. `blockhash` is the hash of the block where the UTXO was
  created. `confirmations` is the number of confirmations of the UTXO. ([#30515](https://github.com/bitcoin/bitcoin/pull/30515))

Wallet
------

- The wallet now detects when wallet transactions conflict with the mempool. Mempool
conflicting transactions can be seen in the `"mempoolconflicts"` field of
`gettransaction`. The inputs of mempool conflicted transactions can now be respent
without manually abandoning the transactions when the parent transaction is dropped
from the mempool, which can cause wallet balances to appear higher.

- A new RPC `createwalletdescriptor` is added which allows users to add new automatically
generated descriptors to their wallet. This can be used to upgrade wallets created prior to
the introduction of a new standard descriptor, such as taproot.

- A new RPC `gethdkeys` is added which will list all of the BIP 32 HD keys in use by all
of the descriptors in the wallet. These keys can be used in conjunction with `createwalletdescriptor`
to create and add single key descriptors to the wallet for a particular key that the wallet
already knows.

- The `sendall` RPC can spend unconfirmed change and will include additional fees as necessary
for the resulting transaction to bump the unconfirmed transactions' feerates to the specified feerate.

GUI changes
-----------

- The "Migrate Wallet" menu allows users to migrate any legacy wallet in their wallet
directory, regardless of the wallets loaded. ([#824](https://github.com/bitcoin-core/gui/pull/824))

- The "Information" window now displays the maximum mempool size along with the
mempool usage. ([#825](https://github.com/bitcoin-core/gui/pull/825))
