// Copyright (c) 2010 Satoshi Nakamoto
// Copyright (c) 2009-2021 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <kernel/chainparams.h>

#include <chainparamsseeds.h>
#include <consensus/amount.h>
#include <consensus/merkle.h>
#include <consensus/params.h>
#include <hash.h>
#include <kernel/messagestartchars.h>
#include <logging.h>
#include <primitives/block.h>
#include <primitives/transaction.h>
#include <script/interpreter.h>
#include <script/script.h>
#include <uint256.h>
#include <util/chaintype.h>
#include <util/strencodings.h>

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cstring>
#include <type_traits>

// Workaround MSVC bug triggering C7595 when calling consteval constructors in
// initializer lists.
// A fix may be on the way:
// https://developercommunity.visualstudio.com/t/consteval-conversion-function-fails/1579014
#if defined(_MSC_VER)
auto consteval_ctor(auto&& input) { return input; }
#else
#define consteval_ctor(input) (input)
#endif

static CBlock CreateGenesisBlock(const char* pszTimestamp, const CScript& genesisOutputScript, uint32_t nTime, uint32_t nNonce, uint32_t nBits, int32_t nVersion, const CAmount& genesisReward, const char* extranonce)
{
    CMutableTransaction txNew;
    txNew.version = 1;
    txNew.vin.resize(1);
    txNew.vout.resize(1);
    txNew.vin[0].scriptSig = CScript() << OP_0 << ParseHex(extranonce) << std::vector<unsigned char>((const unsigned char*)pszTimestamp, (const unsigned char*)pszTimestamp + strlen(pszTimestamp));
    txNew.vout[0].nValue = genesisReward;
    txNew.vout[0].scriptPubKey = genesisOutputScript;

    CBlock genesis;
    genesis.nTime    = nTime;
    genesis.nBits    = nBits;
    genesis.nNonce   = nNonce;
    genesis.nVersion = nVersion;
    genesis.vtx.push_back(MakeTransactionRef(std::move(txNew)));
    genesis.hashPrevBlock.SetNull();
    genesis.hashMerkleRoot = BlockMerkleRoot(genesis);
    return genesis;
}

/**
 * Build the genesis block. Note that the output of its generation
 * transaction cannot be spent since it did not originally exist in the
 * database.
 *
 * CBlock(hash=000000000019d6, ver=1, hashPrevBlock=00000000000000, hashMerkleRoot=4a5e1e, nTime=1231006505, nBits=1d00ffff, nNonce=2083236893, vtx=1)
 *   CTransaction(hash=4a5e1e, ver=1, vin.size=1, vout.size=1, nLockTime=0)
 *     CTxIn(COutPoint(000000, -1), coinbase 04ffff001d0104455468652054696d65732030332f4a616e2f32303039204368616e63656c6c6f72206f6e206272696e6b206f66207365636f6e64206261696c6f757420666f722062616e6b73)
 *     CTxOut(nValue=50.00000000, scriptPubKey=0x5F1DF16B2B704C8A578D0B)
 *   vMerkleTree: 4a5e1e
 */
static CBlock CreateGenesisBlock(uint32_t nTime, uint32_t nNonce, uint32_t nBits, int32_t nVersion, const CAmount& genesisReward, const char* pk, const char* extranonce)
{
    const char* pszTimestamp = "barrons.com 05/Feb/2022 Bitcoin Is Back Over $41,000 as Cryptos Regain Strength";
    const CScript genesisOutputScript = CScript() << ParseHex(pk) << OP_CHECKSIG;
    return CreateGenesisBlock(pszTimestamp, genesisOutputScript, nTime, nNonce, nBits, nVersion, genesisReward, extranonce);
}


/**
 * Main network on which people trade goods and services.
 */
class CMainParams : public CChainParams {
public:
    CMainParams() {
        m_chain_type = ChainType::MAIN;
        consensus.signet_blocks = false;
        consensus.signet_challenge.clear();
        consensus.nSubsidyHalvingInterval = 2100000; // every 4 years
        consensus.BIP34Height = 1; // BIP34 is always active unless overridden
        consensus.BIP34Hash = uint256{};
        consensus.BIP65Height = 1; // BIP65 is always active unless overridden
        consensus.BIP66Height = 1; // BIP66 is always active unless overridden
        consensus.CSVHeight = 1; // CSV is always active unless overridden
        consensus.SegwitHeight = 1; // SEGWIT is always activated on unless overridden
        consensus.MinBIP9WarningHeight = 0;
        consensus.powLimit = uint256S("00ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.nPowTargetTimespan = 14 * 24 * 60 * 60; // two weeks
        consensus.nPowTargetSpacing = 60; // one minute
        consensus.fPowAllowMinDifficultyBlocks = false;
        consensus.enforce_BIP94 = false;
        consensus.fPowNoRetargeting = false;
        consensus.nRuleChangeActivationThreshold = 18144; // 90% of nMinerConfirmationWindow
        consensus.nMinerConfirmationWindow = 20160; // nPowTargetTimespan / nPowTargetSpacing
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = Consensus::BIP9Deployment::NEVER_ACTIVE;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].min_activation_height = 0; // No activation delay

        // Deployment of Taproot (BIPs 340-342)
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].bit = 2;
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].nStartTime = Consensus::BIP9Deployment::NEVER_ACTIVE;
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT;
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].min_activation_height = 0; // No activation delay

        consensus.nMinimumChainWork = uint256S("0x000000000000000000000000000000000000000000000008330a58c98ea88fae");
        consensus.defaultAssumeValid = uint256S("0x45f0050de6b342c004ef065e80375f0cb0a791cd17cdfb1a0a364753507d0fdf"); // 1302000

        consensus.nDevRewardHeight = 494000;
        consensus.nDevRewardHeight2 = 685800;
        consensus.n2023DiffAlgoHeight = 548200;
        consensus.n2023DiffAlgoWindow = 10;
        consensus.n2023DiffAlgoTimespan = 10 * 60;
        consensus.n2023DiffAlgoHeight2 = 590000;
        consensus.n2023DiffAlgoHeight3 = 685800;
        consensus.n2023DiffAlgoHeight4 = 831200;
        consensus.n2023DiffAlgoHeight5 = 841500;
        consensus.nFlexhashHeight = 1133000;
        consensus.nFlexhashBits = 0x2000ffff;

        /**
         * The message start string is designed to be unlikely to occur in normal data.
         * The characters are rarely used upper ASCII, not valid as UTF-8, and produce
         * a large 32-bit integer with any alignment.
         */
        pchMessageStart[0] = 0xd3;
        pchMessageStart[1] = 0x92;
        pchMessageStart[2] = 0xfb;
        pchMessageStart[3] = 0x0e;
        nDefaultPort = 5111;
        nPruneAfterHeight = 1000000;
        m_assumed_blockchain_size = 1;
        m_assumed_chain_state_size = 1;

        genesis = CreateGenesisBlock(1644088100, 600184878, 0x1d00ffff, 1, 0.005 * COIN, "047eb6f0deea46efeff2f3b0aea364f9e0718e821a5becce3c5c7d1a207694af4ff4aa4d3daaaf8dd5a9685a2c6797cbcba94983efd8919d5a1d1e45bacd057c44", "0000000000000000");
        consensus.hashGenesisBlock = genesis.GetHash();

        assert(consensus.hashGenesisBlock == uint256S("0x0000000091ddc5a0b9f0ed098a4db06ee9bc286a104f092f4fb476eb9364b7c9"));
        assert(genesis.hashMerkleRoot == uint256S("0xcf6ccfeaec206c220efb3b6bc67b2411a1291ed2209d2832c105493868d8f368"));

        // Note that of those which support the service bits prefix, most only support a subset of
        // possible options.
        // This is fine at runtime as we'll fall back to using them as an addrfetch if they don't support the
        // service bits we want, but we should get them updated to support all service bits wanted by any
        // release ASAP to avoid it where possible.
        vSeeds.emplace_back("seed.kylacoin.net.");

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,28); // C
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,26); // B
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,65); // A or 3
        base58Prefixes[EXT_PUBLIC_KEY] = {0x03, 0x8f, 0x33, 0x2e}; // kpub
        base58Prefixes[EXT_SECRET_KEY] = {0x03, 0x8f, 0x2e, 0xf4}; // kprv

        bech32_hrp = "kc";

        vFixedSeeds = std::vector<uint8_t>(std::begin(chainparams_seed_main), std::end(chainparams_seed_main));

        fDefaultConsistencyChecks = false;
        m_is_mockable_chain = false;

        checkpointData = {
            {
                {       0, uint256S("0x0000000091ddc5a0b9f0ed098a4db06ee9bc286a104f092f4fb476eb9364b7c9")},
                {     100, uint256S("0x00000000ecf70b5d120ab647be866f421a628f0a81cec8db84945a63adfaf3e4")},
                {    1000, uint256S("0x00000000cb39122095736b83fdb58fd2c33be77af086baa28713693019f407e9")},
                {   10000, uint256S("0x0000000018ac0acf006dc1d35b454bf9fff667ed3928e5306a83ac748e8c5476")},
                {   20000, uint256S("0x00000000f6b5cafe1c19127f107bd64fb03b7eda42026b52eecf58d3cd9a0f79")},
                {   40000, uint256S("0x000000003fb4850bc3ca1c1202523344bc2b8ae00e96a9e55bea706539b91976")},
                {   60000, uint256S("0x000000000cb831eda363fa3c4728f2f480fb2ff9a0bd38b5c4748befa63e04a6")},
                {   80000, uint256S("0x0000000003c9dee6a7d560fcd41c36821a5a9ebe6e21dcc810b0296ea22028d0")},
                {  100000, uint256S("0x00000000013b73246ad1952c90950a9dea7554dbd8385afa9d9ff39b84ffc625")},
                {  200000, uint256S("0x00000000016b703713d61f9a0bca725d6c2ec1b81fd0cc149688972936439d57")},
                {  300000, uint256S("0x000000000188ac3cd45ebdd06d7d827e3dfb306ab97e1608773920809525859e")},
                {  400000, uint256S("0x00000000002034811a860e97cecbe0b6a62a445cbedb34d80665dbedf8504860")},
                {  500000, uint256S("0x000000000093dd28986b0952ad198d772fee09f473280d5edaa3dbd42511326b")},
                {  700000, uint256S("0x0000000000008ebf6171903e307e36d6844aab5d49aeccd65624d21ec18779ec")},
                {  800000, uint256S("0x000000000000dec662226d59a8ee92cf0aa27c102cc267f592cf4995dc90777f")},
                {  900000, uint256S("0x00000000000182b10bb1e8279dd3f89ea200320c0907cf5c479413bb50cf57c2")},
                { 1000000, uint256S("0x000000000002186060cc90fe68881679e92fe2e35b0d9cdbbf750a2fd4e4cb13")},
                { 1100000, uint256S("0x0000000000000b3f0756379c814b3c0c3353b39ab158ab523fee247e32518cda")},
                { 1110000, uint256S("0x00000000000038e12d7171920fa51cdd7aa24aff69d473538fcd02756f12d25a")},
                { 1120000, uint256S("0x000000000000034cfcbd91390370ea1053f517e6ea1b4492bac251c9b326714d")},
                { 1130000, uint256S("0x0000000000002ccb0849431e435b67844b03e0bfacc0135b6e53aafa160be215")},
                { 1150000, uint256S("0x8fab1b100a06669b5fc7a2fc7274b9fc4e15e1facdd4b8863a5f3ea117af6f89")},
                { 1160000, uint256S("0xe3536cee589d75f24519c269ba587b6512f4831872dac6105e78c4a0930b40e3")},
                { 1200000, uint256S("0xed4d19d71e785da21b3bf2127175e8d7be68aab8143e6ae70637ca49f5cdc0dd")},
                { 1302000, uint256S("0x45f0050de6b342c004ef065e80375f0cb0a791cd17cdfb1a0a364753507d0fdf")},
            }
        };

        m_assumeutxo_data = {};

        chainTxData = ChainTxData{
            // Data from RPC: getchaintxstats 4096 45f0050de6b342c004ef065e80375f0cb0a791cd17cdfb1a0a364753507d0fdf
            /* nTime    */ 1728040698,
            /* nTxCount */ 9253,
            /* dTxRate  */ 0.0345453255727998,
        };
    }
};

/**
 * Testnet (v3): public test network which is reset from time to time.
 */
class CTestNetParams : public CChainParams {
public:
    CTestNetParams() {
        m_chain_type = ChainType::TESTNET;
        consensus.signet_blocks = false;
        consensus.signet_challenge.clear();
        consensus.nSubsidyHalvingInterval = 2100000; // every 4 years
        consensus.BIP34Height = 1; // BIP34 is always active unless overridden
        consensus.BIP34Hash = uint256{};
        consensus.BIP65Height = 1; // BIP65 is always active unless overridden
        consensus.BIP66Height = 1; // BIP66 is always active unless overridden
        consensus.CSVHeight = 1; // CSV is always active unless overridden
        consensus.SegwitHeight = 1; // SEGWIT is always activated on unless overridden
        consensus.MinBIP9WarningHeight = 0;
        consensus.powLimit = uint256S("00ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.nPowTargetTimespan = 14 * 24 * 60 * 60; // two weeks
        consensus.nPowTargetSpacing = 60; // one minute
        consensus.fPowAllowMinDifficultyBlocks = true;
        consensus.enforce_BIP94 = false;
        consensus.fPowNoRetargeting = false;
        consensus.nRuleChangeActivationThreshold = 15120; // 75% of nMinerConfirmationWindow for testchains
        consensus.nMinerConfirmationWindow = 20160; // nPowTargetTimespan / nPowTargetSpacing
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = Consensus::BIP9Deployment::NEVER_ACTIVE;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].min_activation_height = 0; // No activation delay

        // Deployment of Taproot (BIPs 340-342)
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].bit = 2;
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].nStartTime = Consensus::BIP9Deployment::NEVER_ACTIVE;
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT;
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].min_activation_height = 0; // No activation delay

        consensus.nMinimumChainWork = uint256S("0x00000000000000000000000000000000000000000000000000000002b2ef54b7");
        consensus.defaultAssumeValid = uint256S("0x17d1bf007ca6bbf17e90124ee9b2ac41001bb99c52abdc933571e9670b877854"); // 15476

        consensus.nDevRewardHeight = 0;
        consensus.nDevRewardHeight2 = 1;
        consensus.n2023DiffAlgoHeight = 0;
        consensus.n2023DiffAlgoWindow = 10;
        consensus.n2023DiffAlgoTimespan = 10 * 60;
        consensus.n2023DiffAlgoHeight2 = 0;
        consensus.n2023DiffAlgoHeight3 = 0;
        consensus.n2023DiffAlgoHeight4 = 0;
        consensus.n2023DiffAlgoHeight5 = 0;
        consensus.nFlexhashHeight = 1;
        consensus.nFlexhashBits = 0x2000ffff;

        pchMessageStart[0] = 0x92;
        pchMessageStart[1] = 0x1a;
        pchMessageStart[2] = 0xc0;
        pchMessageStart[3] = 0x18;
        nDefaultPort = 5222;
        nPruneAfterHeight = 10000;
        m_assumed_blockchain_size = 0;
        m_assumed_chain_state_size = 0;

        genesis = CreateGenesisBlock(1716115228, 1731684196, 0x1d00ffff, 1, 0.005 * COIN, "040fb26304128b9a94f1bc8bf44c587f51621853a501e7cd40a8cbc5d195165d8a27d8329dafabc4e3ca7af42144e431b59d13e629ea16429e13694dcda0497b59", "34d520f801000000");
        consensus.hashGenesisBlock = genesis.GetHash();

        assert(consensus.hashGenesisBlock == uint256S("0x00000000c6f0b6b3e3eff898fc01bf76a77e2b8c91dd32e2dc9e1abb636105af"));
        assert(genesis.hashMerkleRoot == uint256S("0xe3cd245aaacfd10d23a31f5dd8c9583d8b0d42e4aa1e252fe96a57f4626db79a"));

        vFixedSeeds.clear();
        vSeeds.clear();
        // nodes with support for servicebits filtering should be at the top
        vSeeds.emplace_back("testnet-seed.kylacoin.net");

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,38); // G
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,33); // E
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,168); // R or 6
        base58Prefixes[EXT_PUBLIC_KEY] = {0x04, 0x5f, 0x1c, 0xf6}; // vpub
        base58Prefixes[EXT_SECRET_KEY] = {0x04, 0x5f, 0x18, 0xbc}; // vprv

        bech32_hrp = "tkc";

        vFixedSeeds = std::vector<uint8_t>(std::begin(chainparams_seed_test), std::end(chainparams_seed_test));

        fDefaultConsistencyChecks = false;
        m_is_mockable_chain = false;

        checkpointData = {
            {
                {     0, uint256S("0x00000000c6f0b6b3e3eff898fc01bf76a77e2b8c91dd32e2dc9e1abb636105af")},
                {  5000, uint256S("0x7c11678532648beb00f89a0169f08047921f81d31da5da61fc885c931f860f4a")},
                { 10000, uint256S("0x35d833240dc6188d59db96acd236f9a1d291ab0430ead318d8328b62849df6b9")},
                { 12220, uint256S("0x3e4ee2b90b00e23963577fe6071e7086e0a02e4025a6adaaa2750d73ffa8114d")},
                { 15100, uint256S("0x6d2e569d6c1ff15d52bdd250a7e3ba4529d35ca4b3c64133e1fb7a2e22ba47c4")},
                { 15476, uint256S("0x17d1bf007ca6bbf17e90124ee9b2ac41001bb99c52abdc933571e9670b877854")},
            }
        };

        m_assumeutxo_data = {};

        chainTxData = ChainTxData{
            // Data from RPC: getchaintxstats 4096 17d1bf007ca6bbf17e90124ee9b2ac41001bb99c52abdc933571e9670b877854
            /* nTime    */ 1721738128,
            /* nTxCount */ 16253,
            /* dTxRate  */ 0.0009398130030477216,
        };
    }
};

/**
 * Testnet (v4): public test network which is reset from time to time.
 */
class CTestNet4Params : public CChainParams {
public:
    CTestNet4Params() {
        m_chain_type = ChainType::TESTNET4;
        consensus.signet_blocks = false;
        consensus.signet_challenge.clear();
        consensus.nSubsidyHalvingInterval = 2100000;
        consensus.BIP34Height = 1;
        consensus.BIP34Hash = uint256{};
        consensus.BIP65Height = 1;
        consensus.BIP66Height = 1;
        consensus.CSVHeight = 1;
        consensus.SegwitHeight = 1;
        consensus.MinBIP9WarningHeight = 0;
        consensus.powLimit = uint256S("00ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.nPowTargetTimespan = 14 * 24 * 60 * 60; // two weeks
        consensus.nPowTargetSpacing = 60;
        consensus.fPowAllowMinDifficultyBlocks = true;
        consensus.enforce_BIP94 = true;
        consensus.fPowNoRetargeting = false;
        consensus.nRuleChangeActivationThreshold = 15120; // 75% for testchains
        consensus.nMinerConfirmationWindow = 20160; // nPowTargetTimespan / nPowTargetSpacing
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = Consensus::BIP9Deployment::NEVER_ACTIVE;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].min_activation_height = 0; // No activation delay

        // Deployment of Taproot (BIPs 340-342)
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].bit = 2;
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].nStartTime = Consensus::BIP9Deployment::ALWAYS_ACTIVE;
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT;
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].min_activation_height = 0; // No activation delay

        consensus.nMinimumChainWork = uint256{"00000000000000000000000000000000000000000000005faa15d02e6202f3ba"};
        consensus.defaultAssumeValid = uint256{"000000005be348057db991fa5d89fe7c4695b667cfb311391a8db374b6f681fd"}; // 39550

        consensus.nDevRewardHeight = 0;
        consensus.nDevRewardHeight2 = 1;
        consensus.n2023DiffAlgoHeight = 0;
        consensus.n2023DiffAlgoWindow = 10;
        consensus.n2023DiffAlgoTimespan = 10 * 60;
        consensus.n2023DiffAlgoHeight2 = 0;
        consensus.n2023DiffAlgoHeight3 = 0;
        consensus.n2023DiffAlgoHeight4 = 0;
        consensus.n2023DiffAlgoHeight5 = 0;
        consensus.nFlexhashHeight = 1;
        consensus.nFlexhashBits = 0x2000ffff;

        pchMessageStart[0] = 0xa1;
        pchMessageStart[1] = 0x63;
        pchMessageStart[2] = 0x3c;
        pchMessageStart[3] = 0xe5;
        nDefaultPort = 5522;
        nPruneAfterHeight = 1000;
        m_assumed_blockchain_size = 0;
        m_assumed_chain_state_size = 0;

        genesis = CreateGenesisBlock(1727810655, 488917525, 0x1d00ffff, 1, 0.005 * COIN, "040fb26304128b9a94f1bc8bf44c587f51621853a501e7cd40a8cbc5d195165d8a27d8329dafabc4e3ca7af42144e431b59d13e629ea16429e13694dcda0497b59", "67779d5c01000000");
        consensus.hashGenesisBlock = genesis.GetHash();

        assert(consensus.hashGenesisBlock == uint256S("0x00000000bed0654c15c700849ca3156343862dcc5ccfbc98e0f0ed9284cf453c"));
        assert(genesis.hashMerkleRoot == uint256S("0x37709850c6bf9abd47907506696db9114f26b0e9181f4c0662304e0c335af050"));

        vFixedSeeds.clear();
        vSeeds.clear();
        // nodes with support for servicebits filtering should be at the top
        vSeeds.emplace_back("testnet4-seed.kylacoin.net.");

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,38); // G
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,33); // E
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,168); // R or 6
        base58Prefixes[EXT_PUBLIC_KEY] = {0x04, 0x5f, 0x1c, 0xf6}; // vpub
        base58Prefixes[EXT_SECRET_KEY] = {0x04, 0x5f, 0x18, 0xbc}; // vprv

        bech32_hrp = "tkc";

        vFixedSeeds = std::vector<uint8_t>(std::begin(chainparams_seed_testnet4), std::end(chainparams_seed_testnet4));

        fDefaultConsistencyChecks = false;
        m_is_mockable_chain = false;

        checkpointData = {
            {
                {},
            }
        };

        m_assumeutxo_data = {
            {}
        };

        chainTxData = ChainTxData{
            // Data from RPC: getchaintxstats
            .nTime    = 0,
            .tx_count = 0,
            .dTxRate  = 0.0,
        };
    }
};

/**
 * Signet: test network with an additional consensus parameter (see BIP325).
 */
class SigNetParams : public CChainParams {
public:
    explicit SigNetParams(const SigNetOptions& options)
    {
        std::vector<uint8_t> bin;
        vSeeds.clear();

        if (!options.challenge) {
            bin = ParseHex("512103ad5e0edad18cb1f0fc0d28a3d4f1f3e445640337489abb10404f2d1e086be430210359ef5021964fe22d6f8e05b2463c9540ce96883fe3b278760f048f5189f2e6c452ae");
            vSeeds.emplace_back("signet-seed.kylacoin.net");

            consensus.nMinimumChainWork = uint256{};
            consensus.defaultAssumeValid = uint256{};
            m_assumed_blockchain_size = 0;
            m_assumed_chain_state_size = 0;
            chainTxData = ChainTxData{
                // Data from RPC: getchaintxstats 4096 000000187d4440e5bff91488b700a140441e089a8aaea707414982460edbfe54
                /* nTime    */ 0,
                /* nTxCount */ 0,
                /* dTxRate  */ 0,
            };
        } else {
            bin = *options.challenge;

            consensus.nMinimumChainWork = uint256{};
            consensus.defaultAssumeValid = uint256{};
            m_assumed_blockchain_size = 0;
            m_assumed_chain_state_size = 0;
            chainTxData = ChainTxData{
                0,
                0,
                0,
            };
            LogPrintf("Signet with challenge %s\n", HexStr(bin));
        }

        if (options.seeds) {
            vSeeds = *options.seeds;
        }

        m_chain_type = ChainType::SIGNET;
        consensus.signet_blocks = true;
        consensus.signet_challenge.assign(bin.begin(), bin.end());
        consensus.nSubsidyHalvingInterval = 2100000; // every 4 years
        consensus.BIP34Height = 1; // BIP34 is always active unless overridden
        consensus.BIP34Hash = uint256{};
        consensus.BIP65Height = 1; // BIP65 is always active unless overridden
        consensus.BIP66Height = 1; // BIP66 is always active unless overridden
        consensus.CSVHeight = 1; // CSV is always active unless overridden
        consensus.SegwitHeight = 1; // SEGWIT is always activated on unless overridden
        consensus.nPowTargetTimespan = 14 * 24 * 60 * 60; // two weeks
        consensus.nPowTargetSpacing = 60; // one minute
        consensus.fPowAllowMinDifficultyBlocks = false;
        consensus.enforce_BIP94 = false;
        consensus.fPowNoRetargeting = false;
        consensus.nRuleChangeActivationThreshold = 18144; // 90% of nMinerConfirmationWindow
        consensus.nMinerConfirmationWindow = 20160; // nPowTargetTimespan / nPowTargetSpacing
        consensus.MinBIP9WarningHeight = 0;
        consensus.powLimit = uint256S("7fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = Consensus::BIP9Deployment::NEVER_ACTIVE;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].min_activation_height = 0; // No activation delay

        // Activation of Taproot (BIPs 340-342)
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].bit = 2;
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].nStartTime = Consensus::BIP9Deployment::ALWAYS_ACTIVE;
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT;
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].min_activation_height = 0; // No activation delay

        // message start is defined as the first 4 bytes of the sha256d of the block script
        HashWriter h{};
        h << consensus.signet_challenge;
        uint256 hash = h.GetHash();
        std::copy_n(hash.begin(), 4, pchMessageStart.begin());

        nDefaultPort = 5066;
        nPruneAfterHeight = 10000;

        genesis = CreateGenesisBlock(1716115366, 3701389690, 0x1e0377ae, 1, 0.005 * COIN, "040fb26304128b9a94f1bc8bf44c587f51621853a501e7cd40a8cbc5d195165d8a27d8329dafabc4e3ca7af42144e431b59d13e629ea16429e13694dcda0497b59", "cb43c9f200000000");
        consensus.hashGenesisBlock = genesis.GetHash();

        assert(consensus.hashGenesisBlock == uint256S("0x00000000641b013bfe8e47e0999de3218dc06c3ef77ba21bb60b98515468d0db"));
        assert(genesis.hashMerkleRoot == uint256S("0x02ffa5289b04fb071054b8a5cb68f99a73bea21258f84c01b435e8a9ceef8021"));

        vFixedSeeds.clear();

        consensus.nDevRewardHeight = 0;
        consensus.nDevRewardHeight2 = 1;
        consensus.n2023DiffAlgoHeight = 0;
        consensus.n2023DiffAlgoWindow = 10;
        consensus.n2023DiffAlgoTimespan = 10 * 60;
        consensus.n2023DiffAlgoHeight2 = 0;
        consensus.n2023DiffAlgoHeight3 = 0;
        consensus.n2023DiffAlgoHeight4 = 0;
        consensus.n2023DiffAlgoHeight5 = 0;
        consensus.nFlexhashHeight = 1;
        consensus.nFlexhashBits = 0x2000ffff;

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,38);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,33);
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,168);
        base58Prefixes[EXT_PUBLIC_KEY] = {0x04, 0x5f, 0x1c, 0xf6};
        base58Prefixes[EXT_SECRET_KEY] = {0x04, 0x5f, 0x18, 0xbc};

        bech32_hrp = "tkc";

        fDefaultConsistencyChecks = false;
        m_is_mockable_chain = false;
    }
};

/**
 * Regression test: intended for private networks only. Has minimal difficulty to ensure that
 * blocks can be found instantly.
 */
class CRegTestParams : public CChainParams
{
public:
    explicit CRegTestParams(const RegTestOptions& opts)
    {
        m_chain_type = ChainType::REGTEST;
        consensus.signet_blocks = false;
        consensus.signet_challenge.clear();
        consensus.nSubsidyHalvingInterval = 1500; // every 25 hours
        consensus.BIP34Height = 1; // BIP34 is always active unless overridden
        consensus.BIP34Hash = uint256{};
        consensus.BIP65Height = 1; // BIP65 is always active unless overridden
        consensus.BIP66Height = 1; // BIP66 is always active unless overridden
        consensus.CSVHeight = 1; // CSV is always active unless overridden
        consensus.SegwitHeight = 1; // SEGWIT is always activated on unless overridden
        consensus.MinBIP9WarningHeight = 0;
        consensus.powLimit = uint256S("7fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.nPowTargetTimespan = 14 * 24 * 60 * 60; // two weeks
        consensus.nPowTargetSpacing = 60; // one minute
        consensus.fPowAllowMinDifficultyBlocks = true;
        consensus.enforce_BIP94 = true;
        consensus.fPowNoRetargeting = true;
        consensus.nRuleChangeActivationThreshold = 15120; // 75% of nMinerConfirmationWindow for testchains
        consensus.nMinerConfirmationWindow = 1440; // Faster than normal for regtest (1440 instead of 20160)

        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].min_activation_height = 0; // No activation delay

        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].bit = 2;
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].nStartTime = Consensus::BIP9Deployment::ALWAYS_ACTIVE;
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT;
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].min_activation_height = 0; // No activation delay

        consensus.nMinimumChainWork = uint256{};
        consensus.defaultAssumeValid = uint256{};

        consensus.nDevRewardHeight = 0;
        consensus.nDevRewardHeight2 = 1;
        consensus.n2023DiffAlgoHeight = 0;
        consensus.n2023DiffAlgoWindow = 10;
        consensus.n2023DiffAlgoTimespan = 10 * 60;
        consensus.n2023DiffAlgoHeight2 = 0;
        consensus.n2023DiffAlgoHeight3 = 0;
        consensus.n2023DiffAlgoHeight3 = 0;
        consensus.n2023DiffAlgoHeight4 = 0;
        consensus.n2023DiffAlgoHeight5 = 0;
        consensus.nFlexhashHeight = 1;
        consensus.nFlexhashBits = 0x2000ffff;

        pchMessageStart[0] = 0xd2;
        pchMessageStart[1] = 0x13;
        pchMessageStart[2] = 0xf5;
        pchMessageStart[3] = 0xc9;
        nDefaultPort = 5355;
        m_assumed_blockchain_size = 0;
        m_assumed_chain_state_size = 0;

        for (const auto& [dep, height] : opts.activation_heights) {
            switch (dep) {
            case Consensus::BuriedDeployment::DEPLOYMENT_SEGWIT:
                consensus.SegwitHeight = int{height};
                break;
            case Consensus::BuriedDeployment::DEPLOYMENT_HEIGHTINCB:
                consensus.BIP34Height = int{height};
                break;
            case Consensus::BuriedDeployment::DEPLOYMENT_DERSIG:
                consensus.BIP66Height = int{height};
                break;
            case Consensus::BuriedDeployment::DEPLOYMENT_CLTV:
                consensus.BIP65Height = int{height};
                break;
            case Consensus::BuriedDeployment::DEPLOYMENT_CSV:
                consensus.CSVHeight = int{height};
                break;
            }
        }

        for (const auto& [deployment_pos, version_bits_params] : opts.version_bits_parameters) {
            consensus.vDeployments[deployment_pos].nStartTime = version_bits_params.start_time;
            consensus.vDeployments[deployment_pos].nTimeout = version_bits_params.timeout;
            consensus.vDeployments[deployment_pos].min_activation_height = version_bits_params.min_activation_height;
        }

        genesis = CreateGenesisBlock(1716115456, 3487834665, 0x207fffff, 1, 0.005 * COIN, "040fb26304128b9a94f1bc8bf44c587f51621853a501e7cd40a8cbc5d195165d8a27d8329dafabc4e3ca7af42144e431b59d13e629ea16429e13694dcda0497b59", "1d2be6aa00000000");
        consensus.hashGenesisBlock = genesis.GetHash();

        assert(consensus.hashGenesisBlock == uint256S("0x00000000978bc5d38b5ad775761df57631204e6606d9a586630d7e7c82427c20"));
        assert(genesis.hashMerkleRoot == uint256S("0x7c9796d9c4ecfe0938eaaa2838a177caf7fdcfa3137c7f4624a5d64b23c18a4e"));

        vFixedSeeds.clear(); //!< Regtest mode doesn't have any fixed seeds.
        vSeeds.clear();      //!< Regtest mode doesn't have any DNS seeds.

        fDefaultConsistencyChecks = true;
        m_is_mockable_chain = true;

        checkpointData = {
            {
                {0, uint256S("0x00000000978bc5d38b5ad775761df57631204e6606d9a586630d7e7c82427c20")},
            }
        };

        m_assumeutxo_data = {
        };

        chainTxData = ChainTxData{
            0,
            0,
            0
        };

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,38);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,33);
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,168);
        base58Prefixes[EXT_PUBLIC_KEY] = {0x04, 0x5f, 0x1c, 0xf6};
        base58Prefixes[EXT_SECRET_KEY] = {0x04, 0x5f, 0x18, 0xbc};

        bech32_hrp = "kcrt";
    }
};

std::unique_ptr<const CChainParams> CChainParams::SigNet(const SigNetOptions& options)
{
    return std::make_unique<const SigNetParams>(options);
}

std::unique_ptr<const CChainParams> CChainParams::RegTest(const RegTestOptions& options)
{
    return std::make_unique<const CRegTestParams>(options);
}

std::unique_ptr<const CChainParams> CChainParams::Main()
{
    return std::make_unique<const CMainParams>();
}

std::unique_ptr<const CChainParams> CChainParams::TestNet()
{
    return std::make_unique<const CTestNetParams>();
}

std::unique_ptr<const CChainParams> CChainParams::TestNet4()
{
    return std::make_unique<const CTestNet4Params>();
}

std::vector<int> CChainParams::GetAvailableSnapshotHeights() const
{
    std::vector<int> heights;
    heights.reserve(m_assumeutxo_data.size());

    for (const auto& data : m_assumeutxo_data) {
        heights.emplace_back(data.height);
    }
    return heights;
}

std::optional<ChainType> GetNetworkForMagic(const MessageStartChars& message)
{
    const auto mainnet_msg = CChainParams::Main()->MessageStart();
    const auto testnet_msg = CChainParams::TestNet()->MessageStart();
    const auto testnet4_msg = CChainParams::TestNet4()->MessageStart();
    const auto regtest_msg = CChainParams::RegTest({})->MessageStart();
    const auto signet_msg = CChainParams::SigNet({})->MessageStart();

    if (std::equal(message.begin(), message.end(), mainnet_msg.data())) {
        return ChainType::MAIN;
    } else if (std::equal(message.begin(), message.end(), testnet_msg.data())) {
        return ChainType::TESTNET;
    } else if (std::equal(message.begin(), message.end(), testnet4_msg.data())) {
        return ChainType::TESTNET4;
    } else if (std::equal(message.begin(), message.end(), regtest_msg.data())) {
        return ChainType::REGTEST;
    } else if (std::equal(message.begin(), message.end(), signet_msg.data())) {
        return ChainType::SIGNET;
    }
    return std::nullopt;
}
