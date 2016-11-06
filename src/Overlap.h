//
// Created by Domagoj Boros on 06/11/2016.
//

#pragma once

class Overlap {
public:

    Overlap(int aId, int bId, bool afwd, int astart, int aend, int alen, bool bfwd, int bstart, int bend, int blen) :
    aId_(aId), bId_(bId),
    afwd_(afwd), astart_(astart), aend_(aend), alen_(alen),
    bfwd_(bfwd), bstart_(bstart), bend_(bend), blen_(blen) {};

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
