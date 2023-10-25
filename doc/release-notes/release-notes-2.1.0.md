### Network Rules
- Difficulty adjustment algorithm changed from 10 blocks to 1 block by 0.5% (activation after block height 831200, in 14 days)

### P2P

- [#27626](https://github.com/bitcoin/bitcoin/pull/27626) Fixed parallel compact block downloads
- [#27743](https://github.com/bitcoin/bitcoin/pull/27743) p2p: Unconditionally return when compact block status == READ_STATUS_FAILED

### Fees

- [#27622](https://github.com/bitcoin/bitcoin/pull/27622) Fee estimation: avoid serving stale fee estimate

### RPC

- [#27727](https://github.com/bitcoin/bitcoin/pull/27727) rpc: Fix invalid bech32 address handling

### Rest

- [#27853](https://github.com/bitcoin/bitcoin/pull/27853) rest: fix crash error when calling /deploymentinfo
- [#28551](https://github.com/bitcoin/bitcoin/pull/28551) http: bugfix: allow server shutdown in case of remote client disconnection

### Wallet

- [#28038](https://github.com/bitcoin/bitcoin/pull/28038) wallet: address book migration bug fixes
- [#28067](https://github.com/bitcoin/bitcoin/pull/28067) descriptors: do not return top-level only funcs as sub descriptors
- [#28125](https://github.com/bitcoin/bitcoin/pull/28125) wallet: bugfix, disallow migration of invalid scripts
- [#28542](https://github.com/bitcoin/bitcoin/pull/28542) wallet: Check for uninitialized last processed and conflicting heights in MarkConflicted
