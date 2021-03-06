NeoClock
========

Neopixel clock based on neopixel rings from Adafruit.

![NeoClock](./pictures/neoclock.png)

Code
---

The code can be found in the `./code/neoclock/neoclock.ino` file.

This project relies on code from several authors:

- https://github.com/adafruit/Adafruit_NeoPixel
- https://github.com/zeroeth/time_loop
- https://github.com/sparkfun/DeadOn_RTC

Some of the code is copied directly and some is only slightly modified.
Credit is due to the original authors who put in the work that allowed
me to build the clock.  Examples of code used are in the
`./code/examples/` directory.

Usage
---

The clock has two buttons for operation.  These are the Mode button and the
Set button.  The Mode button lets you choose between these choices:

1. Set Hour
2. Set Minute
3. Set Second
4. Set Color Scheme

The clock stops time while you set it allowing you to specify exactly the time
you want.  Finally you can choose one of four pre-programmed color sets for
the different hands of the clock.

Animations
---

Every fifteen minutes there is an animation sequence.  The top of the hour
runs a rainbow cycle.  The other three animations cycle one color through
all the pixels in the clock (Red, Green, Blue).

Laser Cut Files
---

The files used to laser cut the body of the clock are found in the `ponoko`
directory.  The final files sent to Ponoko were:

- `neoclock_ponoko_wood_final.eps`
- `neoclock_ponoko_acrylic_final.eps`

The original SVG files are provided for convenience.

The wood used was a `Veneer MDF Cherry 5.7mm` at 384mm square.

The acrylic used was an `Acrylic Opal 5.6mm` at 181mm square.

Electronics
---

The list of parts can be found in the `electronics` directory.
