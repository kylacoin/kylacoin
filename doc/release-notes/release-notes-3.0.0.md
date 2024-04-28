How to Upgrade
==============

If you are running an older version, shut it down. Wait until it has completely
shut down (which might take a few minutes in some cases), then run the
installer (on Windows) or just copy over `/Applications/Kylacoin-Qt` (on macOS)
or `kylacoind`/`kylacoin-qt` (on Linux).

Notable changes
===============
Algorithm Change
-------------------
The new proof-of-work algorithm, which tends to be only minable with CPUs,
has already been activated on the testnet.
Since the testnet was never used before, the testnet genesis block is regenerated.
The new algorithm will be activated on the mainnet after blockheight of 1128985.

libbitcoinconsensus
-------------------

- libbitcoinconsensus is deprecated and will be removed for upcoming releases.
  This library has existed for nearly 10 years with very little known uptake
  or impact. It has become a maintenance burden.

  The underlying functionality does not change between versions, so any users of
  the library can continue to use the final release indefinitely, with the
  understanding that Taproot is its final consensus update.

  In the future, libbitcoinkernel will provide a much more useful API that is
  aware of the UTXO set, and therefore be able to fully validate transactions and
  blocks. ([#29189](https://github.com/bitcoin/bitcoin/pull/29189))

mempool.dat compatibility
-------------------------

- The `mempool.dat` file created by -persistmempool or the savemempool RPC will
  be written in a new format. This new format includes the XOR'ing of transaction
  contents to mitigate issues where external programs (such as anti-virus) attempt
  to interpret and potentially modify the file.

  This new format can not be read by previous software releases. To allow for a
  downgrade, a temporary setting `-persistmempoolv1` has been added to fall back
  to the legacy format. ([#28207](https://github.com/bitcoin/bitcoin/pull/28207))

P2P and network changes
-----------------------

- BIP324 v2 transport is now enabled by default. It remains possible to disable v2
  by running with `-v2transport=0`. ([#29347](https://github.com/bitcoin/bitcoin/pull/29347))
- Manual connection options (`-connect`, `-addnode` and `-seednode`) will
  now follow `-v2transport` to connect with v2 by default. They will retry with
  v1 on failure. ([#29058](https://github.com/bitcoin/bitcoin/pull/29058))

- Network-adjusted time has been removed from consensus code. It is replaced
  with (unadjusted) system time. The warning for a large median time offset
  (70 minutes or more) is kept. This removes the implicit security assumption of
  requiring an honest majority of outbound peers, and increases the importance
  of the node operator ensuring their system time is (and stays) correct to not
  fall out of consensus with the network. ([#28956](https://github.com/bitcoin/bitcoin/pull/28956))

Mempool Policy Changes
----------------------

- Opt-in Topologically Restricted Until Confirmation (TRUC) Transactions policy
  (aka v3 transaction policy) is available for use on test networks when
  `-acceptnonstdtxn=1` is set. By setting the transaction version number to 3, TRUC transactions
  request the application of limits on spending of their unconfirmed outputs. These
  restrictions simplify the assessment of incentive compatibility of accepting or
  replacing TRUC transactions, thus ensuring any replacements are more profitable for
  the node and making fee-bumping more reliable. TRUC transactions are currently
  nonstandard and can only be used on test networks where the standardness rules are
  relaxed or disabled (e.g. with `-acceptnonstdtxn=1`). ([#28948](https://github.com/bitcoin/bitcoin/pull/28948))

External Signing
----------------

- Support for external signing on Windows has been disabled. It will be re-enabled
  once the underlying dependency (Boost Process), has been replaced with a different
  library. ([#28967](https://github.com/bitcoin/bitcoin/pull/28967))

Updated RPCs
------------

- The addnode RPC now follows the `-v2transport` option (now on by default, see above) for making connections.
  It remains possible to specify the transport type manually with the v2transport argument of addnode. ([#29239](https://github.com/bitcoin/bitcoin/pull/29239))

Build System
------------

- A C++20 capable compiler is now required to build Kylacoin Core. ([#28349](https://github.com/bitcoin/bitcoin/pull/28349))
- MacOS releases are configured to use the hardened runtime libraries ([#29127](https://github.com/bitcoin/bitcoin/pull/29127))

Wallet
------
- Wallet loading has changed in this release. Wallets with some corrupted records that could be
  previously loaded (with warnings) may no longer load. For example, wallets with corrupted
  address book entries may no longer load. If this happens, it is recommended
  load the wallet in a previous version of Kylacoin Core and import the data into a new wallet.

- The `createwallet` RPC will no longer create legacy (BDB) wallets when
  setting `descriptors=false` without also providing the
  `-deprecatedrpc=create_bdb` option. This is because the legacy wallet is
  being deprecated in a future release. ([#28597](https://github.com/bitcoin/bitcoin/pull/28597))

- The `gettransaction`, `listtransactions`, `listsinceblock` RPCs now return
  the `abandoned` field for all transactions. Previously, the "abandoned" field
  was only returned for sent transactions. ([#25158](https://github.com/bitcoin/bitcoin/pull/25158))

- The `listdescriptors`, `decodepsbt` and similar RPC methods now show `h` rather than apostrophe (`'`) to indicate
  hardened derivation. This does not apply when using the `private` parameter, which
  matches the marker used when descriptor was generated or imported. Newly created
  wallets use `h`. This change makes it easier to handle descriptor strings manually.
  E.g. the `importdescriptors` RPC call is easiest to use `h` as the marker: `'["desc": ".../0h/..."]'`.
  With this change `listdescriptors` will use `h`, so you can copy-paste the result,
  without having to add escape characters or switch `'` to 'h' manually.
  Note that this changes the descriptor checksum.
  For legacy wallets the `hdkeypath` field in `getaddressinfo` is unchanged,
  nor is the serialization format of wallet dumps. ([#26076](https://github.com/bitcoin/bitcoin/pull/26076))

- The `getbalances` RPC now returns a `lastprocessedblock` JSON object which contains the wallet's last processed block
  hash and height at the time the balances were calculated. This result shouldn't be cached because importing new keys could invalidate it. ([#26094](https://github.com/bitcoin/bitcoin/pull/26094))

- The `gettransaction` RPC now returns a `lastprocessedblock` JSON object which contains the wallet's last processed block
  hash and height at the time the transaction information was generated. ([#26094](https://github.com/bitcoin/bitcoin/pull/26094))

- The `getwalletinfo` RPC now returns a `lastprocessedblock` JSON object which contains the wallet's last processed block
  hash and height at the time the wallet information was generated. ([#26094](https://github.com/bitcoin/bitcoin/pull/26094))

- Coin selection and transaction building now accounts for unconfirmed low-feerate ancestor transactions. When it is necessary to spend unconfirmed outputs, the wallet will add fees to ensure that the new transaction with its ancestors will achieve a mining score equal to the feerate requested by the user. ([#26152](https://github.com/bitcoin/bitcoin/pull/26152))

- For RPC methods which accept `options` parameters ((`importmulti`, `listunspent`,
  `fundrawtransaction`, `bumpfee`, `send`, `sendall`, `walletcreatefundedpsbt`,
  `simulaterawtransaction`), it is now possible to pass the options as named
  parameters without the need for a nested object. ([#26485](https://github.com/bitcoin/bitcoin/pull/26485))

This means it is possible make calls like:

```sh
src/kylacoin-cli -named bumpfee txid fee_rate=100
```

instead of

```sh
src/kylacoin-cli -named bumpfee txid options='{"fee_rate": 100}'
```

- The `deprecatedrpc=walletwarningfield` configuration option has been removed.
  The `createwallet`, `loadwallet`, `restorewallet` and `unloadwallet` RPCs no
  longer return the "warning" string field. The same information is provided
  through the "warnings" field added in previous release, which returns a JSON array of
  strings. The "warning" string field was deprecated also in previous release. ([#27757](https://github.com/bitcoin/bitcoin/pull/27757))

- The `signrawtransactionwithkey`, `signrawtransactionwithwallet`,
  `walletprocesspsbt` and `descriptorprocesspsbt` calls now return the more
  specific RPC_INVALID_PARAMETER error instead of RPC_MISC_ERROR if their
  sighashtype argument is malformed. ([#28113](https://github.com/bitcoin/bitcoin/pull/28113))

- RPC `walletprocesspsbt`, and `descriptorprocesspsbt` return
  object now includes field `hex` (if the transaction
  is complete) containing the serialized transaction
  suitable for RPC `sendrawtransaction`. ([#28414](https://github.com/bitcoin/bitcoin/pull/28414))

- It's now possible to use [Miniscript](https://bitcoin.sipa.be/miniscript/) inside Taproot leaves for descriptor wallets. ([#27255](https://github.com/bitcoin/bitcoin/pull/27255))

- The CoinGrinder coin selection algorithm has been introduced to mitigate unnecessary
  large input sets and lower transaction costs at high feerates. CoinGrinder
  searches for the input set with minimal weight. Solutions found by
  CoinGrinder will produce a change output. CoinGrinder is only active at
  elevated feerates (default: 30+ sat/vB, based on `-consolidatefeerate`×3). ([#27877](https://github.com/bitcoin/bitcoin/pull/27877))
- The Branch And Bound coin selection algorithm will be disabled when the subtract fee
  from outputs feature is used. ([#28994](https://github.com/bitcoin/bitcoin/pull/28994))
- If the birth time of a descriptor is detected to be later than the first transaction
  involving that descriptor, the birth time will be reset to the earlier time. ([#28920](https://github.com/bitcoin/bitcoin/pull/28920))

GUI changes
-----------

- The transaction list in the GUI no longer provides a special category for "payment to yourself". Now transactions that have both inputs and outputs that affect the wallet are displayed on separate lines for spending and receiving. ([gui#119](https://github.com/bitcoin-core/gui/pull/119))

- A new menu option allows migrating a legacy wallet based on keys and implied output script types stored in BerkeleyDB (BDB) to a modern wallet that uses descriptors stored in SQLite. ([gui#738](https://github.com/bitcoin-core/gui/pull/738))

- The PSBT operations dialog marks outputs paying your own wallet with "own address". ([gui#740](https://github.com/bitcoin-core/gui/pull/740))

- The ability to create legacy wallets is being removed. ([gui#764](https://github.com/bitcoin-core/gui/pull/764))

Low-level changes
=================

Pruning
-------

- When pruning during initial block download, more blocks will be pruned at each
  flush in order to speed up the syncing of such nodes. ([#20827](https://github.com/bitcoin/bitcoin/pull/20827))

Init
----

- Various fixes to prevent issues where subsequent instances of Kylacoin Core would
  result in deletion of files in use by an existing instance. ([#28784](https://github.com/bitcoin/bitcoin/pull/28784), [#28946](https://github.com/bitcoin/bitcoin/pull/28946))
- Improved handling of empty `settings.json` files. ([#29144](https://github.com/bitcoin/bitcoin/pull/29144))