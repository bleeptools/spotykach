# Build Instructions (work in progress)

### Daisy Seed
- Place and solder Daisy Seed female headers as shown at the photo (add photo). Tip: You can use painter's tape to hold them in place while soldering.
- Insert male headers into female counterparts, place daisy on top and solder as shown at the photo.

### LEDs
Please ensure polarity of the LEDs is right. An anode (longer leg) should go into signal hole while a katode (shorter leg) goes into the ground hole.
Tip: you can use painters tape to hold leds in place while soldering.
- Place and solder red LED into the slot 33 as shown at the photo. 
- Place and solder white LEDs into slots 28 and 38 as shown at the photo.
- Wire LED slots to the board breakout using 220 redidtoes
- as following:

  |  LED  | Slot Number | Breakout Number |
  |-------|-------------|-----------------|
  | white |     28      |        6        |
  | white |     38      |        8        |
  |  red  |     33      |        7        |

- Open hardwaretest.ino and uncomment the following calls in ```setup()```:
  ```
  testLED_A();
  testLED_B();
  testLED_R();
  ```
- Connect Daisy with a computer and run hardwaretest.ino. All three leds should 
