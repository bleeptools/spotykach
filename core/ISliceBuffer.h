//
//  ISliceBuffer.h
//  spotykach_core
//
//  Created by Vladyslav Lytvynenko on 17.05.22.
//

#ifndef ISliceBuffer_h
#define ISliceBuffer_h

class ISliceBuffer {
public:
    virtual void initialize() = 0;
    
    virtual float read(int channel, uint32_t frame) = 0;
    
    virtual void write(float in0, float in1) = 0;
    virtual uint32_t writeHead() = 0;
    virtual void rewind() = 0;
    virtual bool isFull() = 0;
    
    virtual void reset() = 0;
    
    virtual ~ISliceBuffer() {};
};

#endif /* ISliceBuffer_h */
