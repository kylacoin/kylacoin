Yilacoin Development Repository
=====================================

https://yilacoin.eu.org

These are some comparisons for Yilacoin:

|                      | Bitcoin                                         | Yilacoin                                 |
| :----                |     :---:                                       |       :----:                             |
| Ticker               | BTC                                             | YCN                                      |
| Symbol               | ₿                                               | Ɏ                                        |
| Block Time           | 10 Minutes                                      | 1 Minute                                 |
| Decimals             | 8 Digits                                        | 12 Digits                                |
| Minimum Value        | 0.00000001                                      | 0.000000000001                           |
| Block Size           | 4 MB                                            | 400 KB                                   |
| Halving              | 210,000 Blocks<br>(~ every 4 years)             | 2,100,000 Blocks<br>(~ every 4 years)    |
| Starting Reward      | 50 BTC                                          | 0.005 YCN                                |
| Block Maturity       | 100 Confirmations<br>(~ 16 hours and 40 minutes)| 60 Confirmations<br>(~ 1 hour)           |
| Difficulty Retarget  | 2,016 Blocks<br>(~ every two weeks)             | 20,160 Blocks<br>(~ every two weeks)     |
| Max Supply           | 21,000,000 BTC                                  | 9,021,000 YCN                            |
| Start Year           | 2009                                            | 2022                                     |

Algorithm
-------
Yilacoin uses double SHA3-256 instead of double SHA-256, which is more secure. SHA3 is also known as keccak but it's diffrent than keccak algorithm, and became standard from keccak in 2015.

Release Notes
=======

v1.0.1
-------
* RPC commands `sendtoaddress` and `getbalance` fixed.
* Example addresses fixed.
* Chain params updated.

v1.0.0
-------
* Initial Release

License
=======

Yilacoin is released under the terms of the MIT license. See [COPYING](COPYING) for more
information or see https://opensource.org/licenses/MIT.
