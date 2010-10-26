/////////////////////////////////////////////////////////////////////////
////                          pic18_usb.h                            ////
////                                                                 ////
//// Hardware layer for CCS's USB library.  This hardware layer      ////
//// supports the USB peripheral on the PIC18 family chips.  Current ////
//// supported families are:                                         ////
////     PIC18F2455/2550/4455/4550                                   ////
////     PIC18F2450/4450                                             ////
////     PIC18F2458/2553/4458/4553                                   ////
////     PIC18F13K50/14K50                                           ////
////     PIC18F2xJ50/PIC18F4xJ50                                     ////
////     PIC18F65J50/66J50/66J55/67J50/85J50/86J50/86J55/87J50       ////
////                                                                 ////
//// This file is part of CCS's PIC USB driver code.  See USB.H      ////
//// for more documentation and a list of examples.                  ////
////                                                                 ////
//// See the section labeled EXTRA USER FUNCTIONS for functions      ////
//// that are specific to only this HW peripheral that you may       ////
//// find useful to use in your application.                         ////
////                                                                 ////
////   *********** NOTE ABOUT 18F2450/4450 LIMITATIONS **********    ////
////  Due to the limited USB RAM of this family, a limitation of     ////
////  this driver is that there are only 3 endpoints (0, 1 and 2).   ////
////  The HW actually supports more endpoints, but to simplify       ////
////  driver development this driver will only support the first 3   ////
////  so there is an easier memory block to work with.               ////
////                                                                 ////
////  USB_MAX_EP0_PACKET_LENGTH will also be set to 8 regardless     ////
////  of USB speed, to save RAM.                                     ////
////                                                                 ////
////   ************** NOTE ABOUT HW REQUIREMENTS ****************    ////
////  If you are not using internal pullups, you will need to put    ////
////  an internal pullup resistor on D+ or D+ depending on if you    ////
////  want to use slow speed or full speed.  This code configures    ////
////  the device to use internal pullups, see usb_init() if you      ////
////  want to change that.                                           ////
////                                                                 ////
////  You need approximately 470nF cap on Vusb, even if you are      ////
////  using the internal 3.3V USB regulator.                         ////
////                                                                 ////
////  To run at full speed, you must use the oscillator              ////
////  configuration (PLLx) to set the PLL divide to 4MHz.  You can   ////
////  configure the MCU clock to any speed (up to 48MHz) but the     ////
////  PLL must run at 4Mhz to provide the USB peripheral with a      ////
////  96MHz clock.  See the datasheet for details.                   ////
////                                                                 ////
////  To run at slow speed you must configure your MCU to run at     ////
////  24Mhz.  See the datasheet for details.                         ////
////                                                                 ////
////   ****************  NOTE ABOUT INTERRUPTS  ******************   ////
//// This driver uses INT_USB.  It requires INT_USB to interrupt the ////
//// PIC when an event has happened on the USB Bus.  Therfore        ////
//// this code enables interrupts.  A user modification can be made  ////
//// to poll the USB interrupt flag instead of relying on an         ////
//// interrupt.                                                      ////
////                                                                 ////
/////////////////////////////////////////////////////////////////////////
////                                                                 ////
//// Version History:                                                ////
////                                                                 ////
////                                                                 ////
//// May 8th, 2009:                                                  ////
////  Problems with 18F14K50 fixed.                                  ////
////  Added 18F46J50 family.                                         ////
////                                                                 ////
//// March 5th, 2009:                                                ////
////   18F14K50 family added.                                        ////
////   Cleanup for Wizard.                                           ////
////   PIC24 Initial release.                                        ////
////                                                                 ////
//// Nov 3rd, 2008:                                                  ////
////  * 4553 family added.                                           ////
////                                                                 ////
//// Dec 18, 2007:                                                   ////
////  * usb_kbhit() moved to device driver.                          ////
////  * USB Token handler changed to workaround a flaw in the USB    ////
////     peripheral to prevent the USB peripheral from               ////
////     inadvertantly STALLing the endpoint.  Happened most often   ////
////     in bulk demo applications, especially CDC.                  ////
////                                                                 ////
////   11-6-07:  Fixed a bug where usb_data_buffer[] was reserving   ////
////                 too much RAM.                                   ////
////             USB_MAX_EP0_PACKET_LENGTH value can be overwritten. //// 
////                 For the 18F4450/2450 family this should be 8    ////
////                 because of limited RAM.  Reducing this value    //// 
////                 frees RAM for application.                      ////
////             Based upon which endpoints have been configured for ////
////                 use, will free up unused USB data RAM for       ////
////                 application dynamically.  This should free up   ////
////                 at least 128 bytes of RAM.                      ////
////             CDC now fits on a 18F4450/2450                      ////
////                                                                 ////
////   09-19-07: Fixed problems with 18F4450 family.                 ////
////                                                                 ////
////   07-17-07: Added 18F4450,2450 support                          ////
////                                                                 ////
////   07-13-07: Added 87J50 family support                          ////
////                                                                 ////
////   11-01-05: usb_detach(), usb_attach() and usb_init_cs()        ////
////               changed for the better.                           ////
////                                                                 ////
////   10-28-05: Added usb_rx_packet_size()                          ////
////                                                                 ////
////   07-13-05: usb_put_packet() changed for 16bit packet sizes     ////
////             usb_flush_in() changed for 16bit packet sizes       ////
////             usb_get_packet() changed for 16bit packet sizes     ////
////             usb_flush_out() changed for 16bit packet sizes      ////
////             usb_set_configured() changed for 16bit packet sizes ////
////                                                                 ////
////   06-30-05: usb_tbe() added                                     ////
////             The way endpoint 0 DTS is set has been changed.     ////
////                                                                 ////
////   06-20-05: Initial Release                                     ////
////                                                                 ////
////   05-13-05: Beta Release (Full Speed works)                     ////
////                                                                 ////
////   03-21-05: Initial Alpha Release                               ////
////                                                                 ////
/////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2005 Custom Computer Services         ////
//// This source code may only be used by licensed users of the CCS  ////
//// C compiler.  This source code may only be distributed to other  ////
//// licensed users of the CCS C compiler.  No other use,            ////
//// reproduction or distribution is permitted without written       ////
//// permission.  Derivative programs created using this software    ////
//// in object code form are not restricted in any way.              ////
/////////////////////////////////////////////////////////////////////////

#IFNDEF __PIC18_USB_H__
#DEFINE __PIC18_USB_H__

#DEFINE __USB_HARDWARE__

//let the USB Stack know that we are using a PIC with internal USB peripheral
#DEFINE __PIC__   1

#ifndef USB_USE_FULL_SPEED
 #define USB_USE_FULL_SPEED   TRUE
#endif

#ifndef USB_MAX_EP0_PACKET_LENGTH
#if ((getenv("DEVICE")=="PIC18F2450") || (getenv("DEVICE")=="PIC18F4450") || (getenv("DEVICE")=="PIC18F13K50") || (getenv("DEVICE")=="PIC18F14K50"))
   //due to limited ram, force max packet length to 8 for this chip
   #define USB_MAX_EP0_PACKET_LENGTH   8
#else
   #if USB_USE_FULL_SPEED==FALSE
      //slow speed requires 8byte max packet size for endpoint 0
      #DEFINE USB_MAX_EP0_PACKET_LENGTH   8
   #else
      //for full speed you can still use 8bytes, but 64 will be faster
      #DEFINE USB_MAX_EP0_PACKET_LENGTH   64
   #endif
#endif
#endif

#if (!USB_USE_FULL_SPEED && (USB_MAX_EP0_PACKET_LENGTH!=8))
 #error Slow speed devices must use a max packet size of 8 for endpoint 0!
#endif

#if USB_MAX_EP0_PACKET_LENGTH < 8
 #error Max Endpoint 0 length can't be less than 8!
#endif

#if USB_MAX_EP0_PACKET_LENGTH > 64
 #error Max Endpoint 0 length can't be greater than 64!
#endif

#include "usb_hw_layer.h"

//////////////////////// EXTRA USER FUNCTIONS /////////////////////////////////

/**************************************************************
/* usb_flush_in()
/*
/* Input: endpoint - which endpoint to mark for transfer
/*        len - length of data that is being tramsferred
/*        tgl - Data toggle synchronization for this packet
/*
/* Output: TRUE if success, FALSE if error (we don't control the endpoint)
/*
/* Summary: Marks the endpoint ready for transmission.  You must
/*          have already loaded the endpoint buffer with data.
/*          (IN is PIC -> PC).
/*          This routine is useful if you want to setup an endpoint by
/*          writing to the buffer directly.
/*          This routine is not necessary if you use usb_put_packet().
/***************************************************************/
int1 usb_flush_in(int8 endpoint, int16 len, USB_DTS_BIT tgl);

/**************************************************************
/* usb_flush_out()
/*
/* Input: endpoint - which endpoint to mark for release
/*        tgl - Data toggle synchronization to expect in the next packet
/*
/* Output: NONE
/*
/* Summary: Clears the previously received packet, and then marks this
/*          endpoint's receive buffer as ready for more data.
/*          (OUT is PC -> PIC).  This routine is useful if you do not
/*          want to use usb_get_packet(), instead you want to handle with
/*          the endpoint buffer directly.  Also see usb_rx_packet_size().
/*          This routine is not necessary if you use usb_get_packet().
/***************************************************************/
void usb_flush_out(int8 endpoint, USB_DTS_BIT tgl);

/**************************************************************
/* usb_rx_packet_size()
/*
/* Input: endpoint - which endpoint to mark to check
/*
/* Output: Returns the number of bytes in the endpoint's receive buffer.
/*
/* Summary: Read the number of data stored in the receive buffer.  When you
/*    have handled the data, use usb_flush_out() to clear the buffer.  The
/*    result may be invalid if usb_kbhit() returns false for this endpoint.
/*    This routine is not necessary if you use usb_get_packet().
/***************************************************************/
int16 usb_rx_packet_size(int8 endpoint);

#ENDIF
