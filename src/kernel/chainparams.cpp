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
#include <chainparamsbase.h>
#include <logging.h>
#include <primitives/block.h>
#include <primitives/transaction.h>
#include <script/interpreter.h>
#include <script/script.h>
#include <uint256.h>
#include <util/strencodings.h>

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cstring>
#include <type_traits>

static CBlock CreateGenesisBlock(const char* pszTimestamp, const CScript& genesisOutputScript, uint32_t nTime, uint32_t nNonce, uint32_t nBits, int32_t nVersion, const CAmount& genesisReward, const char* extranonce)
{
    CMutableTransaction txNew;
    txNew.nVersion = 1;
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
        strNetworkID = CBaseChainParams::MAIN;
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
        consensus.powLimit = uint256S("00000000ffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.nPowTargetTimespan = 14 * 24 * 60 * 60; // two weeks
        consensus.nPowTargetSpacing = 60; // one minute
        consensus.fPowAllowMinDifficultyBlocks = false;
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

        consensus.nMinimumChainWork = uint256S("0x000000000000000000000000000000000000000000000000681a1f8ab633b776");
        consensus.defaultAssumeValid = uint256S("0x0000000000019f43d285ee592974be709d89780fc259101779fd913a03c0aaad");

        consensus.nDevRewardHeight = 494000;
        consensus.nDevRewardHeight2 = 685800;
        consensus.n2023DiffAlgoHeight = 548200;
        consensus.n2023DiffAlgoWindow = 10;
        consensus.n2023DiffAlgoTimespan = 10 * 60;
        consensus.n2023DiffAlgoHeight2 = 590000;
        consensus.n2023DiffAlgoHeight3 = 685800;

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

        genesis = CreateGenesisBlock(1644088100, 600184878, 0x1d00ffff, 1, 5000000000, "047eb6f0deea46efeff2f3b0aea364f9e0718e821a5becce3c5c7d1a207694af4ff4aa4d3daaaf8dd5a9685a2c6797cbcba94983efd8919d5a1d1e45bacd057c44", "0000000000000000");
        consensus.hashGenesisBlock = genesis.GetHash();

        assert(consensus.hashGenesisBlock == uint256S("0x0000000091ddc5a0b9f0ed098a4db06ee9bc286a104f092f4fb476eb9364b7c9"));
        assert(genesis.hashMerkleRoot == uint256S("0xcf6ccfeaec206c220efb3b6bc67b2411a1291ed2209d2832c105493868d8f368"));

        // Note that of those which support the service bits prefix, most only support a subset of
        // possible options.
        // This is fine at runtime as we'll fall back to using them as an addrfetch if they don't support the
        // service bits we want, but we should get them updated to support all service bits wanted by any
        // release ASAP to avoid it where possible.
        vSeeds.emplace_back("seed.kylacoin.com");

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,28); // C
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,26); // B
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,65); // A or 3
        base58Prefixes[EXT_PUBLIC_KEY] = {0x03, 0x8f, 0x33, 0x2e}; // kpub
        base58Prefixes[EXT_SECRET_KEY] = {0x03, 0x8f, 0x2e, 0xf4}; // kprv

        bech32_hrp = "kc";

        vFixedSeeds = std::vector<uint8_t>(std::begin(chainparams_seed_main), std::end(chainparams_seed_main));

        fDefaultConsistencyChecks = false;
        fRequireStandard = true;
        m_is_test_chain = false;
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
                {  450000, uint256S("0x000000000036cd3913f263a70df9aafb82ef46ef51eae7683ad68ab6c2e4eb63")},
                {  490000, uint256S("0x000000000029b023aaf9b749c5ba2fe918084c6628167c8bfbcee9a823c4e322")},
                {  500000, uint256S("0x000000000093dd28986b0952ad198d772fee09f473280d5edaa3dbd42511326b")},
                {  550000, uint256S("0x00000000000cf8079baecfa0bbda94836b829ceae0d0fede0c416a843553b8d7")},
                {  560000, uint256S("0x00000000000541f11cb3cbf14c2af6ca311d36ac457834206f1679a97b881d8f")},
                {  570000, uint256S("0x0000000000131f51be6f045335241d8ebfb95320b7caf6c4e0fd4a21bfce71a1")},
                {  670760, uint256S("0x0000000000019f43d285ee592974be709d89780fc259101779fd913a03c0aaad")},
            }
        };

        m_assumeutxo_data = MapAssumeutxo{
         // TODO to be specified in a future patch.
        };

        chainTxData = ChainTxData{
            // Data from RPC: getchaintxstats 4096 0000000000019f43d285ee592974be709d89780fc259101779fd913a03c0aaad
            /* nTime    */ 1688154144,
            /* nTxCount */ 876545,
            /* dTxRate  */ 0.02824495867209817,
        };
    }
};

/**
 * Testnet (v3): public test network which is reset from time to time.
 */
class CTestNetParams : public CChainParams {
public:
    CTestNetParams() {
        strNetworkID = CBaseChainParams::TESTNET;
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
        consensus.powLimit = uint256S("00000000ffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.nPowTargetTimespan = 14 * 24 * 60 * 60; // two weeks
        consensus.nPowTargetSpacing = 60; // one minute
        consensus.fPowAllowMinDifficultyBlocks = true;
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

        consensus.nMinimumChainWork = uint256S("0x0000000000000000000000000000000000000000000000000000000100010001");
        consensus.defaultAssumeValid = uint256{};

        consensus.nDevRewardHeight = 0;
        consensus.nDevRewardHeight2 = 0;
        consensus.n2023DiffAlgoHeight = 0;
        consensus.n2023DiffAlgoWindow = 10;
        consensus.n2023DiffAlgoTimespan = 10 * 60;
        consensus.n2023DiffAlgoHeight2 = 0;
        consensus.n2023DiffAlgoHeight3 = 0;

        pchMessageStart[0] = 0x92;
        pchMessageStart[1] = 0x1a;
        pchMessageStart[2] = 0xc0;
        pchMessageStart[3] = 0x18;
        nDefaultPort = 5222;
        nPruneAfterHeight = 10000;
        m_assumed_blockchain_size = 0;
        m_assumed_chain_state_size = 0;

        genesis = CreateGenesisBlock(1644088459, 4068332350, 0x1d00ffff, 1, 5000000000, "044dc56bc53f95586b2a4f6d993d0cc069daef5f6772da87fb6c9fc48eb4ff7711b080b37a3bf54b655b85adfb220fe5510cedf674f341c14b73ad889a0bc266d8", "0000000000000000");
        consensus.hashGenesisBlock = genesis.GetHash();

        assert(consensus.hashGenesisBlock == uint256S("0x00000000537d4ba50213510bd605bd4d0810b142e7b026f9acbe28b0c6f923b7"));
        assert(genesis.hashMerkleRoot == uint256S("0x19d97190c89b09ba2a44559bb33ac68b5fb8c20575c4115bdf9586059f5764aa"));

        vFixedSeeds.clear();
        vSeeds.clear();
        // nodes with support for servicebits filtering should be at the top
        vSeeds.emplace_back("testnet-seed.kylacoin.com");

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,38); // G
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,33); // E
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,168); // R or 6
        base58Prefixes[EXT_PUBLIC_KEY] = {0x04, 0x5f, 0x1c, 0xf6}; // vpub
        base58Prefixes[EXT_SECRET_KEY] = {0x04, 0x5f, 0x18, 0xbc}; // vprv

        bech32_hrp = "tkc";

        vFixedSeeds = std::vector<uint8_t>(std::begin(chainparams_seed_test), std::end(chainparams_seed_test));

        fDefaultConsistencyChecks = false;
        fRequireStandard = false;
        m_is_test_chain = true;
        m_is_mockable_chain = false;

        checkpointData = {
            {
                {0, uint256S("0x00000000537d4ba50213510bd605bd4d0810b142e7b026f9acbe28b0c6f923b7")},
            }
        };

        m_assumeutxo_data = MapAssumeutxo{
            // TODO to be specified in a future patch.
        };

        chainTxData = ChainTxData{
            // Data from RPC: getchaintxstats 4096 0000000000004ae2f3896ca8ecd41c460a35bf6184e145d91558cece1c688a76
            /* nTime    */ 0,
            /* nTxCount */ 0,
            /* dTxRate  */ 0,
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
            vSeeds.emplace_back("signet-seed.kylacoin.com");

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

        strNetworkID = CBaseChainParams::SIGNET;
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
        consensus.fPowNoRetargeting = false;
        consensus.nRuleChangeActivationThreshold = 18144; // 90% of nMinerConfirmationWindow
        consensus.nMinerConfirmationWindow = 20160; // nPowTargetTimespan / nPowTargetSpacing
        consensus.MinBIP9WarningHeight = 0;
        consensus.powLimit = uint256S("00000377ae000000000000000000000000000000000000000000000000000000");
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
        memcpy(pchMessageStart, hash.begin(), 4);

        nDefaultPort = 5066;
        nPruneAfterHeight = 10000;

        genesis = CreateGenesisBlock(1644088831, 2749120352, 0x1e0377ae, 1, 5000000000, "045e6d11056cb835c826d5f7d1cf9719c94044bfc53a7b3a112b596670cd993a0c6a9682accef4820ca872488547c162ad6accac5ecbdc0584be8eb256efdba997", "0000000001000000");
        consensus.hashGenesisBlock = genesis.GetHash();

        assert(consensus.hashGenesisBlock == uint256S("0x000000000e3a3e63516bc7b86e66760ae0f5fd8583c45eda6a17419b5ecbc5e6"));
        assert(genesis.hashMerkleRoot == uint256S("0x91cb1d08546d8d738764addff058360f2f2f94b4607482e46a82db16bb9d80fb"));

        vFixedSeeds.clear();

        consensus.nDevRewardHeight = 0;
        consensus.nDevRewardHeight2 = 0;
        consensus.n2023DiffAlgoHeight = 0;
        consensus.n2023DiffAlgoWindow = 10;
        consensus.n2023DiffAlgoTimespan = 10 * 60;
        consensus.n2023DiffAlgoHeight2 = 0;
        consensus.n2023DiffAlgoHeight3 = 0;

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,38);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,33);
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,168);
        base58Prefixes[EXT_PUBLIC_KEY] = {0x04, 0x5f, 0x1c, 0xf6};
        base58Prefixes[EXT_SECRET_KEY] = {0x04, 0x5f, 0x18, 0xbc};

        bech32_hrp = "tkc";

        fDefaultConsistencyChecks = false;
        fRequireStandard = true;
        m_is_test_chain = true;
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
        strNetworkID =  CBaseChainParams::REGTEST;
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
        consensus.nDevRewardHeight2 = 0;
        consensus.n2023DiffAlgoHeight = 0;
        consensus.n2023DiffAlgoWindow = 10;
        consensus.n2023DiffAlgoTimespan = 10 * 60;
        consensus.n2023DiffAlgoHeight2 = 0;
        consensus.n2023DiffAlgoHeight3 = 0;

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

        genesis = CreateGenesisBlock(1644088997, 2211185478, 0x207fffff, 1, 5000000000, "041c4d4fb4848aabac2fe8001e8eddabe6e68c0d63c4816f7e48e6c8f87f8eace3b5d202517ddd2a486193ad63dad3cbf5c4ab8c9ec4a165cae6d76b10d36aa9a3", "0000000000000000");
        consensus.hashGenesisBlock = genesis.GetHash();

        assert(consensus.hashGenesisBlock == uint256S("0x00000000a8438e3b9a503a6af26ce896dfb4cdd0a928d06486704998746db191"));
        assert(genesis.hashMerkleRoot == uint256S("0xeea9413df15691b76d85275a8171a6d279b58929d5ff7b6a71538daa126e464e"));

        vFixedSeeds.clear(); //!< Regtest mode doesn't have any fixed seeds.
        vSeeds.clear();      //!< Regtest mode doesn't have any DNS seeds.

        fDefaultConsistencyChecks = true;
        fRequireStandard = true;
        m_is_test_chain = true;
        m_is_mockable_chain = true;

        checkpointData = {
            {
                {0, uint256S("0x00000000a8438e3b9a503a6af26ce896dfb4cdd0a928d06486704998746db191")},
            }
        };

        m_assumeutxo_data = MapAssumeutxo{

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
