#include "daisy_seed.h"

struct StoredParams {
    int even_pattern_a;
    int cword_pattern_a;
    int even_pattern_b;
    int cword_pattern_b; 
    int tag;
};

static StoredParams DSY_QSPI_BSS params_qspi;

class Persistence {
public:
    Persistence() = default;
    ~Persistence() = default;

    void initialize(daisy::DaisySeed& hw) {
        _params = params_qspi;
        _hw = &hw;
    }

    bool is_updated() {
        return _params.tag == 3;
    }

    int cword_pattern_a() { return _params.cword_pattern_a; }
    void set_cword_pattern_a(int value) { 
        _params.cword_pattern_a = value;
        store();
    }

    int cword_pattern_b() { return _params.cword_pattern_b; }
    void set_cword_pattern_b(int value) { 
        _params.cword_pattern_b = value;
        store();
    }

    int even_pattern_a() { return _params.even_pattern_a; }
    void set_even_pattern_a(int value) { 
        _params.even_pattern_a = value;
        store();
    }

    int even_pattern_b() { return _params.even_pattern_b; }
    void set_even_pattern_b(int value) { 
        _params.even_pattern_b = value;
        store();
    }

private:
    void store() {
        _params.tag = 3;
        size_t size = sizeof(_params);
	    size_t address = (size_t)(&params_qspi);
	    
	    _hw->qspi.Erase(address, address + size);
	    _hw->qspi.Write(address, size, (uint8_t*)(&_params));    
    }

    void clear() {
        size_t size = sizeof(_params);
	    size_t address = (size_t)(&params_qspi);
	    _hw->qspi.Erase(address, address + size);
    }

    daisy::DaisySeed* _hw;
    StoredParams _params;
};
