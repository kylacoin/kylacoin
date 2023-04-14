// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2018 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <pow.h>

#include <arith_uint256.h>
#include <chain.h>
#include <primitives/block.h>
#include <uint256.h>

unsigned int GetNextWorkRequired(const CBlockIndex* pindexLast, const CBlockHeader *pblock, const Consensus::Params& params)
{
    assert(pindexLast != nullptr);
    unsigned int nProofOfWorkLimit = UintToArith256(params.powLimit).GetCompact();

    if((pindexLast->nHeight+1) < params.n2023DiffAlgoHeight) {
        // Only change once per difficulty adjustment interval
        if ((pindexLast->nHeight+1) % params.DifficultyAdjustmentInterval() != 0)
        {
            if (params.fPowAllowMinDifficultyBlocks)
            {
                // Special difficulty rule for testnet:
                // If the new block's timestamp is more than 2* 10 minutes
                // then allow mining of a min-difficulty block.
                if (pblock->GetBlockTime() > pindexLast->GetBlockTime() + params.nPowTargetSpacing*2)
                    return nProofOfWorkLimit;
                else
                {
                    // Return the last non-special-min-difficulty-rules-block
                    const CBlockIndex* pindex = pindexLast;
                    while (pindex->pprev && pindex->nHeight % params.DifficultyAdjustmentInterval() != 0 && pindex->nBits == nProofOfWorkLimit)
                        pindex = pindex->pprev;
                    return pindex->nBits;
                }
            }
            return pindexLast->nBits;
        }
    } else {
        // Only change once per difficulty adjustment interval
        if ((pindexLast->nHeight+1) % params.n2023DiffAlgoWindow != 0)
        {
            if (params.fPowAllowMinDifficultyBlocks)
            {
                // Special difficulty rule for testnet:
                // If the new block's timestamp is more than 2* 10 minutes
                // then allow mining of a min-difficulty block.
                if (pblock->GetBlockTime() > pindexLast->GetBlockTime() + params.nPowTargetSpacing*2)
                    return nProofOfWorkLimit;
                else
                {
                    // Return the last non-special-min-difficulty-rules-block
                    const CBlockIndex* pindex = pindexLast;
                    while (pindex->pprev && pindex->nHeight % params.n2023DiffAlgoWindow != 0 && pindex->nBits == nProofOfWorkLimit)
                        pindex = pindex->pprev;
                    return pindex->nBits;
                }
            }
            return pindexLast->nBits;
        }
    }
    int nHeightFirst;
    if((pindexLast->nHeight+1) < params.n2023DiffAlgoHeight) {
        // Go back by what we want to be 14 days worth of blocks
        nHeightFirst = pindexLast->nHeight - (params.DifficultyAdjustmentInterval()-1);
    } else {
        nHeightFirst = pindexLast->nHeight - (params.n2023DiffAlgoWindow-1);
    }

    assert(nHeightFirst >= 0);
    const CBlockIndex* pindexFirst = pindexLast->GetAncestor(nHeightFirst);
    assert(pindexFirst);

    return CalculateNextWorkRequired(pindexLast, pindexFirst->GetBlockTime(), params);
}

unsigned int CalculateNextWorkRequired(const CBlockIndex* pindexLast, int64_t nFirstBlockTime, const Consensus::Params& params)
{
    if (params.fPowNoRetargeting)
        return pindexLast->nBits;

    // Limit adjustment step
    int64_t nActualTimespan = pindexLast->GetBlockTime() - nFirstBlockTime;
    
    if((pindexLast->nHeight+1) < params.n2023DiffAlgoHeight) {
        if (nActualTimespan < params.nPowTargetTimespan/4)
            nActualTimespan = params.nPowTargetTimespan/4;
        if (nActualTimespan > params.nPowTargetTimespan*4)
            nActualTimespan = params.nPowTargetTimespan*4;
    } else if((pindexLast->nHeight+1) < params.n2023DiffAlgoHeight2) {
        if (nActualTimespan < params.n2023DiffAlgoTimespan/1.007)
            nActualTimespan = params.n2023DiffAlgoTimespan/1.007;
        if (nActualTimespan > params.n2023DiffAlgoTimespan*1.5)
            nActualTimespan = params.n2023DiffAlgoTimespan*1.5;
    } else {
        if (nActualTimespan < params.n2023DiffAlgoTimespan/1.014)
            nActualTimespan = params.n2023DiffAlgoTimespan/1.014;
        if (nActualTimespan > params.n2023DiffAlgoTimespan*1.014)
            nActualTimespan = params.n2023DiffAlgoTimespan*1.014;
    }

    // Retarget
    const arith_uint256 bnPowLimit = UintToArith256(params.powLimit);
    arith_uint256 bnNew;
    bnNew.SetCompact(pindexLast->nBits);
    bnNew *= nActualTimespan;
    if((pindexLast->nHeight+1) < params.n2023DiffAlgoHeight) {
        bnNew /= params.nPowTargetTimespan;
    } else {
        bnNew /= params.n2023DiffAlgoTimespan;
    }

    if (bnNew > bnPowLimit)
        bnNew = bnPowLimit;

    return bnNew.GetCompact();
}

bool CheckProofOfWork(uint256 hash, unsigned int nBits, const Consensus::Params& params)
{
    bool fNegative;
    bool fOverflow;
    arith_uint256 bnTarget;

    bnTarget.SetCompact(nBits, &fNegative, &fOverflow);

    // Check range
    if (fNegative || bnTarget == 0 || fOverflow || bnTarget > UintToArith256(params.powLimit))
        return false;

    // Check proof of work matches claimed amount
    if (UintToArith256(hash) > bnTarget)
        return false;

    return true;
}
