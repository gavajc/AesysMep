# AUTHOR

**Juan Carlos García Vázquez**

+   Date: July 15, 2020.
+ E-Mail: gavajc@live.com

# GENERAL INFO

The MEP Library was developed to facilitate interaction with Aesys
devices that use the Modular Extensible Protocol (MEP) for PPTP and
UoPTB structures.

MEP protocol is used in many AESYS devices such traffic lights, verbaTRAFFIC,
VMS displays and many others devices.

This library create and parse MEP messages only and can be used for serial
communications like RS-232, RS-485, ... or in Ethernet comunications with an
external communication library. In the interactive tests we add a simple header
for create a TCP socket for send and receive a MEP message (Unix and Windows).
 
   The library has 3 types of functions:
 
       - << DECODE >>
         Prototypes for manipulate Aesys MEP Frames and parse MEP commands.
         It's recommended to call one of the following functions before using
         any other defined in this library.
 
               AesysMepDecodeUPTBFrame
               AesysMepCncopyPPTPFrame
 
         These functions validate and create a copy of the MEP frame to be used
         with the defined structures in this library.
 
       - << INFORMATION >>
         Prototypes for retrive device information like:
 
               Temperature
               Humidity
               Brightness
               Hardware Info
               ...
 
       - << MANIPULATION >>
         Prototypes for:
 
               Show text.
               Show pictograms.
               Reset device
               Set Brightness
               ...

# Usage

For use this library, only include in your project the files in src:

    1.- aesys_mep.c
    2.- aesys_mep.h

For test the library we include a QT project file (mep_inttest.pro)
in .qt folder. This qt project, compile the interactive test.
The program arguments are:

    1.- The type of MEP message to use (PPTP or UoPTB). 0 is used for PPTP messages and 1  for UoPTB messages.
    2.- The IP address of the Aesys device.

If you have any question, please send me an email.
