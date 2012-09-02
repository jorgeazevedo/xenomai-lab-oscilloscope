Xenomai Lab Oscilloscope
========================

An oscilloscope for Xenomai Lab. This will be merged into the main rep someday.

Installation
------------

The source code must be put in ~/.xenomailab/blocks/oscilloscope

    $ cd ~/.xenomailab/blocks/
    $ mkdir oscilloscope
    $ cd oscilloscope
    $ git clone git://github.com/jorgeazevedo/xenomai-lab-oscilloscope.git .

and an entry should be added to ~/.xenomailab/blocks/blocks.conf

    $ [Non Real-Time]
    $ oscilloscope=square

The oscilloscope requires Qt and Qwt development libraries. If all dependencies are installed, then

    $ make

should produce an executable.

The block will appear on the block list in the main Xenomai Lab window under "Non real-time".
