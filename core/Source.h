//
//  Source.h
//  Spotykach
//
//  Created by Vladyslav Lytvynenko on 17/08/14.
//  Copyright (c) 2014 Vladyslav Lytvynenko. All rights reserved.
//

#ifndef __Spotykach__Source__
#define __Spotykach__Source__

#include <vector>
#include "ISource.h"

class Source: public ISource {
public:
    Source();
    void set_frozen(bool) override;
    bool is_frozen() override { return _frozen; }

    void set_antifreeze(bool) override;
    
    void set_cycle_start(uint32_t) override;

    void initialize() override;
    
    uint32_t length() override { return _bufferLength; };

    void write(float, float) override;
    uint32_t read_head() override;
    
    void read(float&, float&, uint32_t) override;
    
    bool is_filled() override { return _filled; }
    
    void reset() override;
    
private:
    float* _buffer[2];
    uint32_t _bufferLength;
    bool _frozen;
    bool _antifreeze;
    bool _filled;
    uint32_t _writeHead;
    uint32_t _readHead;
    uint32_t _sycleStart;
};

#endif
