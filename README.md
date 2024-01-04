## Spotykach
#### A dual sampler and looper designed for dynamic rhythms, generative melodies and rich textures.
In collaboration with [Roey Tsemah](https://www.linkedin.com/in/roeytsemah/?originalSubdomain=nl) and [Kasia Łukaszuk](https://www.instagram.com/kasia.luk._/) (UX & Interface Design).

[Product page](https://www.synthux.academy/simple/spotykach)

<img src="pic/final.assembly.jpg" width="800px"/>



### Build Instructions
are [here](build.md)

### Setup
```shell
$ git clone --recurse-submodules https://github.com/bleeptools/spotykach.git
$ cd spotykach/lib/libDaisy
$ make
$ cd ../.. 
$ make clean; make
```

### Upload
```shell
$ make program-dfu
```
