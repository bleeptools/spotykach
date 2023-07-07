# Build Instructions (work in progress)

### Daisy Seed
- Place and solder Daisy Seed female headers as shown at the photo (add photo). Tip: You can use painter's tape to hold them in place while soldering.
- Use multimeter to check there are no short circuits between pins.
- Insert male headers into female counterparts, place Daisy on top and solder.
- Wire power and ground as following:
  | Breakout Number | Target Footprint |
  |-----------------|------------------|
  | 29 (3v3 Analog) |        VCC       |
  |    20 (AGND)    |        GND       |
  |    38 (DGND)    |        GND       |

### LEDs
Please ensure polarity of LEDs is right. An anode (longer leg) should go into signal hole while a katode (shorter leg) goes into the ground hole.
Tip: you can use painters tape to hold leds in place while soldering.
- Place and solder red LED into the slot 33 as shown at the photo. 
- Place and solder white LEDs into slots 28 and 38 as shown at the photo.
- Wire LED slots to the board breakout using 220 Ohm resistors as following:

  |  LED  | Slot Number | Breakout Number |
  |-------|-------------|-----------------|
  | white |     28      |        6        |
  |  red  |     33      |        7        |
  | white |     38      |        8        |

- Open [hardware-test.ino](hardware-test/hardware-test.ino) and uncomment the following calls in ```setup()```:
  ```
  testLED_A();
  testLED_B();
  testLED_R();
  ```
- Connect Daisy with a computer and run hardware-test.ino. All three leds should lit up.

### MPR121 touch sensor
- Place and solder MPR121 female header at the bottom of the board as shown in photo.
- Place and solder front pannel female header.
- Place and solder male header to the front panel as shown in the photo.
[TODO]

### Pots, Switches and Sockets
- Place pots, switches and sockets on the board as shown on the photo. Don't solder yet.
- Put the front panel in place and screw 2-3 nuts on the pots to hold it in place.
- Make sure all the components are at their place.
- Turn the assembly upside down and solder components. Soldering switches you'll need to push them from the opposite side to hold them in place.
- Remove front panel
- Turn all the pots to the middle and vheck potential short circuits with the multimeter.
- Wire controls slots to the breakout as following:



