PEM - Peripheral EMulator - Documentation
=========================================

Overview
--------
PEM is meant to facilitate the automation of certain tasks, typically needing
human intervention. It is designed to allow a host PC to emulate the behavior
of certain peripheral typically connected to another device.
Currently it supports the emulation of an USB keyboard with US layout.
To perform the emulation, it employes an Arduino unit, which is connected
inbetween the host PC and the device recipient of the emulation.


Working Modes
-------------
The emulator supports two working modes: recording and playback.
During recording, the user enters keys on the host, which records them and
forwards them to the Arduino. If the device is attached to the other end of
the Arduino, inputs are transparently forwarded, allowing for precise recording
of both key sequences and the related timing. THe sequences are then saved to
file and later on played back.
Playback mode is simply the replay of sequences previously recorded.


Supported HW - Arduino
----------------------
At the moment, the only supported HW is the Arduino UNO R3. This is due to the
fact that one notable use case for PEM is to configure the BIOS of the target
device upon boot. While the device is in BIOS mode, it supports only keyboards
that implement 6-key rollover (6KRO), as specified by the PC 97 standard:

http://en.wikipedia.org/wiki/USB_human_interface_device_class#Keyboards

The UNO R3 relies on a secondary chip for implementing USB communications:
typically it acts as an USB-to-Serial adapter, however it can be re-programmed
to prentend being an USB keybaord with the 6KRO feature:

http://wiki.sgmk-ssam.ch/index.php?title=Arduino_Uno_R3_as_HID

All other types of Arduino boards rely on a SW emulation library, which does
not implement 6KRO, therefore preventing the Arduino from interacting with the
device while it boots.


Additional USB-to-Serial adapter
--------------------------------
The USB port of the Arduino is used for emulating the keyboard, so another
way is needed to interface it to the host PC.
This is achieved by means of an USB-to-Serial adapter.
Because the uC has only 1 serial port HW block, used to communicate with the
built-in USB interface, the serial communication with the host is handled by
SW emulation.


Communication Protocol
----------------------
The protocol is described in detail in the source code, however it is worth to 
mention that communications errors will not go unnoticed: the communication is
initiated by the host, each message has a checksum and an ID, while the
Arduino replies with ACKs (and possibly additional data), using the same ID.
Also the messages coming from the Arduino have checksums.
In case no ACK is returned, the host will retry a maximum amount of times and
eventually return an error, should the communication fail more times than it 
is allowed.
To ensure that the host has always an up-to-date knowledge of the status of 
the Arduino, when the host is idle, the Arduino will periodically notify its
presence. This functionality is used by the host to detect when the arduino is
present and ready to receive commands.
