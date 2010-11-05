PSGroove
========

This is the PSGrooPIC, an open-source reimplementation of the psjailbreak exploit for
PIC18F microcontrollers.

It is known to work on:

- PIC18F2455 (28 pins / 24KB Flash)
- PIC18F2550 (28 pins / 32KB Flash)
- PIC18F4455 (40 pins / 24KB Flash)
- PIC18F4550 (40 pins / 32KB Flash)

**This software is not intended to enable piracy, and such features
have been disabled.  This software is intended to allow the execution
of unsigned third-party apps and games on the PS3.**

No one involved in maintaining the psgroopic git is responsible for or has any involvement with any existing usb dongles sporting "psgroove" / "psgroopic" in its name. Thank you for your understanding.


Cloning
-------
The repository uses the PL3 Payload as a submodule.  To clone, use something like:

    git clone git://github.com/Noltari/PSGrooPIC.git
    cd PSGrooPIC
    git submodule init
    git submodule update

If you don't have PPU-GCC installed, make might get confused and refuse to build. To fix this do something like:

    cd PL3
    make clean
    git checkout .
    cd ..

Make should now work as expected and use the precompiled PL3 payloads.


Configuring
-----------

This version of PSGrooPIC has been modified to directly use PL3 payloads instead of a single hardcoded Payload for much greater flexablity.


Building
--------

    make clean
    make


Using
-----
To use this exploit:
  
* Hard power cycle your PS3 (using the switch in back, or unplug it)
* Plug the dongle into your PS3.
* Press the PS3 power button, followed quickly by the eject button.

After a few seconds, the first LED on your dongle should light up.
After about 5 seconds, the second LED will light up (or the LED will
just go off, if you only have one).  This means the exploit worked!
You can see the new "Install Package Files" menu option in the game
menu.


Notes
-----
A programmed dongle won't enumerate properly on a PC, so don't worry
about that.


Credits
-------
Special thanks to the developers of PSGroove (http://github.com/psgroove/psgroove), who did the real hard work by reversing the code from the original PSJailbreak.

Thanks to the developers of PSGrooPIC, which made possible the port to the PIC18F, to the people who keep it updated by integrating the latest payloads and also to the ones that have contributed to integrate PL3 on it. (J1M, ElSemi, TSC, laichung, Noltari).

And finally, thanks to kakarotoks for creating and maintaining PL3, the open source payload which makes simpler the update of our devices.