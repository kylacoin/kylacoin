// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2019 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <primitives/block.h>

#include <hash.h>
#include <tinyformat.h>

uint256 CBlockHeader::GetHash() const
{
    return (Hash3Writer{} << *this).GetHash();
}

uint256 CBlockHeader::GetHash2() const
{
    return (Hash4Writer{} << *this).GetHash();
}

uint256 CBlockHeader::GetPoWHash() const
{
    uint256 hash;
    uint256 hash2;
    std::string result;
    hash = GetHash();
    if(nVersion & 0x8000) {
        SQLiteCache cache("cache.db");
        result = cache.get(hash.ToString());
        if(result == "") {
            hash2 = GetHash2();
            cache.set(hash.ToString(), hash2.ToString());
        } else {
            hash2 = uint256S(result);
        }
        return hash2;
    }
    return hash;
}


std::string CBlock::ToString() const
{
    std::stringstream s;
    s << strprintf("CBlock(hash=%s, ver=0x%08x, hashPrevBlock=%s, hashMerkleRoot=%s, nTime=%u, nBits=%08x, nNonce=%u, vtx=%u)\n",
        GetHash().ToString(),
        nVersion,
        hashPrevBlock.ToString(),
        hashMerkleRoot.ToString(),
        nTime, nBits, nNonce,
        vtx.size());
    for (const auto& tx : vtx) {
        s << "  " << tx->ToString() << "\n";
    }
    return s.str();
}
