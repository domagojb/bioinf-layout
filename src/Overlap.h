//
// Created by Domagoj Boros on 06/11/2016.
//

#pragma once

class Overlap {
public:

    Overlap(int aId, int bId, bool afwd, int astart, int aend, int alen, bool bfwd, int bstart, int bend, int blen) :
    aId_(aId), bId_(bId),
    afwd_(afwd), astart_(astart), aend_(aend), alen_(alen),
    bfwd_(bfwd), bstart_(bstart), bend_(bend), blen_(blen) {}

    int getAId() { return aId_; }

    int getBId() { return bId_; }

    int getAStart() { return astart_; }

    int getAEnd() { return aend_; }

    int getBStart() {
        if (bfwd_) return bstart_;
        else return bend_;
    }

    int getBEnd() {
        if (bfwd_) return bend_;
        else return bstart_;
    }

    int getALength() { return alen_; }

    int getBLength() { return blen_; }

    /**
     *
     * @return true if the overlap is with the reverse comeplement of b
     */
    int isBrc() { return !bfwd_; }

private:

    int aId_;
    int bId_;
    int astart_;
    int aend_;
    int alen_;
    bool afwd_;
    int bstart_;
    int bend_;
    int blen_;
    bool bfwd_;
};
