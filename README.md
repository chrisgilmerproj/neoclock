NeoClock
========

Neopixel clock based on neopixel rings from Adafruit

Relies on code from Adafruit and Zeroeth, see
https://github.com/zeroeth/time_loop

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
all the pixels in the clock.
