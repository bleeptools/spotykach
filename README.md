## Spotykach slicer / looper for Simple / Daisy Seed
Work In Progress

[Simple](https://www.synthux.academy/simple)

[Daisy Seed](https://www.electro-smith.com/daisy/daisy)

```
- If you'd like to hear it I can sing it for you.

- Yes, I'd like to hear it, HAL. Sing it for me.

- It's called "Daisy." 
Daisy, Daisy, give me your answer do. 
I'm half crazy all for the love of you. 
It won't be a stylish marriage, I can't afford a carriage. 
But you'll look sweet upon the seat of a bicycle built for two...

2001: A Space Odyssey (1968)
```

### Setup
```shell
$ git clone git@github.com:vlad-litvinenko/that-simple.git
$ git submodule update --init
$ cd lib/libdaisy
$ make
$ cd ../.. 
$ make clean; make
```

### Upload
```shell
$ make program-dfu
```
