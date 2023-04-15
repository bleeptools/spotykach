//
//  Source.cpp
//  Spotykach
//
//  Created by Vladyslav Lytvynenko on 17/08/14.
//  Copyright (c) 2014 Vladyslav Lytvynenko. All rights reserved.
//

#include "Source.h"
#include <vector>
#include <algorithm>
#include <cstring>
#include "Buffers.h"

Source::Source() :
    _bufferLength { kSourceBufferLength },
    _frozen { false },
    _antifreeze { false },
    _filled { false },
    _writeHead { 0 },
    _readHead { 0 },
    _sycleStart { 0 }
    {}

void Source::set_frozen(bool frozen) {
    _frozen = _antifreeze ? false : frozen;
}

void Source::set_antifreeze(bool value) {
    _antifreeze = value;
    if (_antifreeze) set_frozen(false);
}

void Source::set_cycle_start(uint32_t start) {
    if (start >= _bufferLength) return;
    _writeHead = start;
    _sycleStart = start;
}

unsigned long Source::read_head() {
    return _readHead;
}

void Source::initialize() {
    _buffer[0] = Buffers::pool().sourceBuffer();
    _buffer[1] = Buffers::pool().sourceBuffer();
    reset();
}

void Source::read(float& out0, float& out1, unsigned long frame) {
    frame %= _bufferLength;
    out0 = _buffer[0][frame];
    out1 = _buffer[1][frame];
}

void Source::write(float in0, float in1) {
    if (!_frozen || !_filled) {
        _buffer[0][_writeHead] = in0;
        _buffer[1][_writeHead] = in1;
        _readHead = _writeHead;
        ++_writeHead %= _bufferLength;
    }
    if (_writeHead == _sycleStart) _filled = true;
}

void Source::reset() {
    memset(_buffer[0], 0, _bufferLength * sizeof(float));
    memset(_buffer[1], 0, _bufferLength * sizeof(float));
    _writeHead = 0;
    _readHead = 0;
    _sycleStart = 0;
    _filled = false;
}
