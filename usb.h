///////////////////////////////////////////////////////////////////////////
////                              usb.h                                ////
////                                                                   ////
//// Function protypes, defintions and globals used by CCS USB driver  ////
////                                                                   ////
//// This file is part of CCS's USB driver code                        ////
////                                                                   ////
//// The following USB examples are provided by CCS:                   ////
////   ex_usb_mouse.c - A HID Mouse.                                   ////
////   ex_usb_hid.c - A custom application using HID protocol.         ////
////   ex_usb_kbmouse.c - A HID Mouse/Keyboard combo using multiple    ////
////                      interfaces.                                  ////
////   ex_usb_kbmouse2.c - A HID Mouse/Keyboard combo using multiple   ////
////                      HID Reports.                                 ////
////   ex_usb_scope.c - A digital oscilloscope using a custom          ////
////                    protocol requiring custom Windows drivers.     ////
////   ex_usb_serial.c -                                               ////
////   ex_usb_serial2.c - Two examples of using the CDC driver for     ////
////     a virtual COM port.                                           ////
////   ex_usb_bootloader.c - CDC/Virtual COM port bootloader.          ////
////   ex_usb_loadmouse.c - A conversion of ex_usb_mouse.c that is     ////
////        compatible with ex_usb_mouse.c.                            ////
////                                                                   ////
////        ********************** API *********************           ////
////                                                                   ////
//// These are the functions that are meant to be called by the user:  ////
////                                                                   ////
//// usb_init() - Initializes the USB stack, the USB peripheral and    ////
////              attaches the unit to the usb bus.  Enables           ////
////              interrupts.  Will wait in an infinite loop until     ////
////              the device enumerates - if you are using             ////
////              connection sense or if the processor should run      ////
////              even if it's not connected to USB then use           ////
////              usb_init_cs() instead.                               ////
////                                                                   ////
//// usb_init_cs() - A smaller usb_init(), does not attach unit        ////
////              to usb bus or enable interrupts.  Since this does    ////
////              not attach to the USB, you must periodically call    ////
////              usb_task().  See usb_task() for more information.    ////
////                                                                   ////
//// usb_task() - If usb_init_cs() was used to initiate the USB        ////
////        peripheral, usb_task() should then be called periodically  ////
////        to check the connection sense pin.  If the connection      ////
////        sense pin denotes USB is connected and the USB peripheral  ////
////        is not attached, this will attach the USB peripheral       ////
////        so the PC can start the enumeration process (and it        ////
////        will enable interrupts).  If the connection sense pin      ////
////        denotes USB is not attached and the USB peripheral is      ////
////        running, this will reset the USB peripheral and wait       ////
////        for USB to reconnect (and usb_enumerated() will start      ////
////        returning FALSE).  If connection sense (USB_CON_SENSE_PIN) ////
////        is not defined the usb_task() assumes that USB is always   ////
////        connected.                                                 ////
////                                                                   ////
//// usb_attached() - Returns TRUE if the device is attached to a      ////
////                  USB cable.  A macro that looks at the defined    ////
////                  connection sense pin.  If this returns TRUE      ////
////                  it does not mean the PC has connected to it,     ////
////                  you need to use usb_enumerated() to check this.  ////
////                                                                   ////
//// usb_enumerated() - Returns TRUE if device has been enumerated     ////
////                    (configured) by host, FALSE if it has not.     ////
////                    Do not try to use the USB peripheral for       ////
////                    sending and receiving packets until you        ////
////                    are enumerated.                                ////
////                                                                   ////
//// usb_wait_for_enumeration() - Sits in an infinte loop until device ////
////                              is enumerated.                       ////
////                                                                   ////
//// usb_tbe(endpoint) - Returns TRUE if the endpoint transmit buffer  ////
////     is free and ready to accept a new packet for transmission.    ////
////                                                                   ////
//// usb_put_packet(endpoint, ptr, len, tgl) - Sends one packet to the ////
////                    host.  If you need to send a message that      ////
////                    spans more than one packet then use            ////
////                    usb_puts().  Fore more detailed documentation  ////
////                    see usb_hw_layer.h                             ////
////                                                                   ////
//// usb_puts(endpoint, ptr, len, timeout) - Sends a multiple packet   ////
////     message to the host.  If you only need to send one packet,    ////
////     it is more effecient to use usb_put_packet().  This is        ////
////     documented in more detail above the prototype in USB.H.       ////
////                                                                   ////
//// usb_kbhit(endpoint) - Returns true if OUT endpoint contains data  ////
////               from host.  This will remain TRUE until             ////
////               usb_put_packet() or usb_flush_out() used.           ////
////               This function will return an invalid response       ////
////               if specified endpoint is not enabled for            ////
////               receiving data.                                     ////
////                                                                   ////
//// len = usb_get_packet(endpoint, ptr, max) - Gets one packet that   ////
////        from the host/PC.  usb_kbhit() must return TRUE before you ////
////        call this routine or your data may not be valid.  This     ////
////        only receives one packet, if you are trying to receive a   //// 
////        multi-packet message use usb_gets().  For more detailed    ////
////        documentation see usb_hw_layer.h.                          ////
////                                                                   ////
//// len = usb_gets(endpoint, ptr, max, timeout) - Gets multiple       ////
////        packets from the host, you would use this instead          ////
////        of usb_get_packet() if you wanted to handle multi-packet   ////
////        messages.  This is documented in more detail above the     ////
////        prototype in USB.H.                                        ////
////                                                                   ////
////                                                                   ////
////        ********* DEFINITIONS / CONFIGURATION **********           ////
////                                                                   ////
//// The following definitions are declared here, but can be           ////
//// overwritten in your code.  Unless needed otherwise, leave         ////
//// to default value.  If confused about a definition read the        ////
//// comments at each defintion                                        ////
////                                                                   ////
//// USB_HID_IDLE (TRUE) - Set to TRUE if your device supports         ////
////                            Set_Idle HID class request.  Set to    ////
////                            False if you do not (device will       ////
////                            send a Wrong-state if computer         ////
////                            sends a Set_Idle / Get_Idle command)   ////
////                            NOTE: If you set to TRUE you must      ////
////                            provide your own code.  See            ////
////                            usb_isr_tkn_setup_ClassInterface() in  ////
////                            usb.c                                  ////
////                                                                   ////
//// USB_HID_BOOT_PROTOCOL (FALSE) - Set to TRUE if your device supports   ////
////                            Set_Protocl HID class request. Set to  ////
////                            False if you do not (device will       ////
////                            send a Wrong-state if computer         ////
////                            sends a Set_Protocl / Get_Protocol     ////
////                            command).                              ////
////                            NOTE: If you set to TRUE you must      ////
////                            provide your own code in the           ////
////                            application that properly send boot    ////
////                            or HID packets.                        ////
////                                                                   ////
//// USB_MAX_EP0_PACKET_LENGTH (8) - Max Packet size for Endpoint 0.   ////
////                            The hardware level driver (ex          ////
////                            pic18_usb.h will define this value if  ////
////                            not already overwritten).  Increasing  ////
////                            this size will speed up the            ////
////                            enumeration process.                   ////
////                                                                   ////
//// USB_EPx_RX_ENABLE (USB_ENABLE_DISABLED) - Where x is the          ////
////                         endpoint number. Change this define       ////
////                         to specify what kind of transfer method   ////
////                         this RX (PC to device) endpoint uses.     ////
////                       Here is the list of valid transfer methods: ////
////                             USB_ENABLE_CONTROL                    ////
////                             USB_ENABLE_ISOCHRONOUS                ////
////                             USB_ENABLE_BULK                       ////
////                             USB_ENABLE_INTERRUPT                  ////
////                             USB_ENABLE_DISABLED                   ////
////                        Don't forget that you must specify the     ////
////                        transfer method properly in your endpoint  ////
////                        descriptor, too.                           ////
////                                                                   ////
//// USB_EPx_RX_SIZE (0) - For some hardware it is important to know   ////
////                       how much memory to reserve for receiving    ////
////                       packets.                                    ////
////                                                                   ////
////  *** NOTE *** You must have both USB_EPx_RX_ENABLE defined and    ////
////               USB_EPx_RX_SIZE defined (non 0) to enable a RX      ////
////               endpoint.                                           ////
////  *** NOTE *** Endpoint 0 is always enabled.                       ////
////                                                                   ////
//// USB_EPx_TX_ENABLE (USB_ENABLE_DISABLED) - Where x is the          ////
////                         endpoint number. Change this define       ////
////                         to specify what kind of transfer method   ////
////                         this TX (device to PC) endpoint uses.     ////
////                       Here is the list of valid transfer methods: ////
////                             USB_ENABLE_CONTROL                    ////
////                             USB_ENABLE_ISOCHRONOUS                ////
////                             USB_ENABLE_BULK                       ////
////                             USB_ENABLE_INTERRUPT                  ////
////                             USB_ENABLE_DISABLED                   ////
////                        Don't forget that you must specify the     ////
////                        transfer method properly in your endpoint  ////
////                        descriptor, too.                           ////
////                                                                   ////
//// USB_EPx_TX_SIZE (0) - For some hardware it is important to know   ////
////                       how much memory to reserve for transmitting ////
////                       packets.                                    ////
////                                                                   ////
////  *** NOTE *** You must have both USB_EPx_TX_ENABLE defined and    ////
////               USB_EPx_TX_SIZE defined (non 0) to enable a TX      ////
////               endpoint.                                           ////
////                                                                   ////
////                                                                   ////
//// USB_HID_DEVICE (TRUE) - HID devices require extra code to handle  ////
////                         HID requests.  You can disable to save    ////
////                         ROM space if you are not using a HID      ////
////                         device.  If you are not using a HID       ////
////                         device you must provide your own O/S      ////
////                         (Windows) driver.                         ////
////                                                                   ////
//// The other definitions should not be changed.                      ////
////                                                                   ////
///////////////////////////////////////////////////////////////////////////
////                                                                   ////
//// Version History:                                                  ////
////                                                                   ////
//// Nov 13th, 2009:                                                   ////
////  usb_endpoint_is_valid() prototyped.                              ////
////                                                                   ////
//// March 5th, 2009:                                                  ////
////   Cleanup for Wizard.                                             ////
////   PIC24 Initial release.                                          ////
////                                                                   ////
//// July 13th, 2005:                                                  ////
////  usb_ep_tx_size[] and usb_ep_rx_size[] changed to 16bits          ////
////                                                                   ////
//// June 20th, 2005:                                                  ////
////  Initial 18fxx5x release                                          ////
////                                                                   ////
//// May 13th, 2005:                                                   ////
////  Beta release, only works with 18Fxx5x hardware layer.            ////
////  Now supports multiple interfaces (many defines in descriptors    ////
////   will have to be changed, see examples)                          ////
////                                                                   ////
//// Mar 21st, 2005:                                                   ////
////  Initial Alpha Release with PIC18Fxx5x support.  (ONLY TESTED     ////
////   WITH 18F4550)                                                   ////
////  usb_gets() and usb_puts() changed (see usb.c)                    ////
////                                                                   ////
//// June 24th, 2004:                                                  ////
////  Optimization and cleanup.                                        ////
////                The following definitions changed:                 ////
////  USB_EPx_TX_ENABLE and USB_EPx_RX_ENABLE have changed. See usb.h  ////
////  USB_CONFIG_DESCRIPTORS[] removed                                 ////
////  USB_CONFIG_DESC_LEN changed to USB_DESC_CONFIG_LEN               ////
////  USB_INTERFACE_DESC_LEN changed to USB_DESC_INTERFACE_LEN         ////
////  USB_CLASS_DESC_LEN changed to USB_DESC_CLASS_LEN                 ////
////  USB_ENDPOINT_DESC_LEN changed to USB_DESC_ENDPOINT_LEN           ////
////  USB_CONFIG_DESC_KEY changed to USB_DESC_CONFIG_TYPE              ////
////  USB_INTERFACE_DESC_KEY changed to USB_DESC_INTERFACE_TYPE        ////
////  USB_CLASS_DESC_KEY changed to USB_DESC_CLASS_TYPE                ////
////  USB_ENDPOINT_DESC_KEY changed to USB_DESC_ENDPOINT_TYPE          ////
////  USB_STRING_X[] arrays removed, see USB_STRING_DESC[] and         ////
////                                 USB_STRING_DESC_OFFSET[]          ////
////  dev_req, curr_config, status_device and getdesc_type global      ////
////        variables moved into struct USB_stack_status               ////
////                                                                   ////
//// October 15th, 2003: Support for boot protocol added.              ////
////         Set USB_HID_BOOT_PROTOCOL to TRUE to support this.        ////
////         The array hid_protocol[] saves which protocol mode each   ////
////         interface is in.  It is your applications job to send     ////
////         data that either fit the boot protocol or HID protocol.   ////
////                                                                   ////
//// May 6th, 2003: Fixed a potential stack overflow using PCM         ////
////                                                                   ////
//// October 29th, 2002: New definition added to USB_STATES            ////
////                                                                   ////
//// August 2nd, 2002: Initial Public Release                          ////
////                                                                   ////
///////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2009 Custom Computer Services           ////
//// This source code may only be used by licensed users of the CCS    ////
//// C compiler.  This source code may only be distributed to other    ////
//// licensed users of the CCS C compiler.  No other use,              ////
//// reproduction or distribution is permitted without written         ////
//// permission.  Derivative programs created using this software      ////
//// in object code form are not restricted in any way.                ////
///////////////////////////////////////////////////////////////////////////

#IFNDEF __USB_PROTOTYPES__
#DEFINE __USB_PROTOTYPES__

//// CONFIGURATION ////////////////////////////////////////////////////////////

#ifndef USB_CON_SENSE_PIN
 #define USB_CON_SENSE_PIN  0
#endif

#IFNDEF USB_HID_BOOT_PROTOCOL
   #DEFINE USB_HID_BOOT_PROTOCOL FALSE
#ENDIF

#IFNDEF USB_HID_IDLE
   #DEFINE USB_HID_IDLE FALSE
#ENDIF

//should the compiler add the extra HID handler code?  Defaults to yes.
#IFNDEF USB_HID_DEVICE
   #DEFINE USB_HID_DEVICE TRUE
#ENDIF

#IFNDEF USB_CDC_DEVICE
   #DEFINE USB_CDC_DEVICE FALSE
#ENDIF

//set to false to opt for less RAM, true to opt for less ROM
#ifndef USB_OPT_FOR_ROM
   #define USB_OPT_FOR_ROM TRUE
#endif

#IFNDEF USB_MAX_EP0_PACKET_LENGTH
  #DEFINE USB_MAX_EP0_PACKET_LENGTH 8
#ENDIF


////// USER-LEVEL API /////////////////////////////////////////////////////////

/**************************************************************
/* usb_enumerated()
/*
/* Input: Global variable USB_Curr_Config
/* Returns: Returns a 1 if device is configured / enumerated,
/*          Returns a 0 if device is un-configured / not enumerated.
/*
/* Summary: See API section of USB.H for more documentation.
/***************************************************************/
int1 usb_enumerated(void);

/**************************************************************
/* usb_wait_for_enumeration()
/*
/* Input: Global variable USB_Curr_Config
/*
/* Summary: Waits in-definately until device is configured / enumerated.
/*          See API section of USB.H for more information.
/***************************************************************/
void usb_wait_for_enumeration(void);

/****************************************************************************
/* usb_gets(endpoint, ptr, max, timeout)
/*
/* Input: endpoint - endpoint to get data from
/*        ptr - place / array to store data to
/*        max - max amount of data to get from USB and store into ptr
/*         timeout - time in milliseconds, for each packet, to wait before 
/*                   timeout.  set to 0 for no timeout.
/*
/* Output: Amount of data returned.  It may be less than max.
/*
/* Summary: Gets data from the host.  Will get multiple-packet messages
/*          and finish when either it receives a 0-len packet or a packet
/*          of less size than maximum.
/*
/*****************************************************************************/
unsigned int16 usb_gets(int8 endpoint, int8 * ptr, unsigned int16 max, unsigned int16 timeout);

/****************************************************************************
/* usb_puts()
/*
/* Inputs: endpoint - endpoint to send data out
/*         ptr - points to array of data to send
/*         len - amount of data to send
/*         timeout - time in milli-seconds, for each packet, to wait before 
/*                   timeout.  set to 0 for no timeout.
/*
/* Outputs: Returns TRUE if message sent succesfully, FALSE if it was not
/*    sent before timeout period expired.
/*
/* Summary: Used for sending multiple packets of data as one message.  This
/*       function can still be used to send messages consiting of only one 
/*       packet.  See usb_put_packet() documentation for the rules about when 
/*       multiple packet messages or 0-lenght packets are needed.
/*
/*****************************************************************************/
int1 usb_puts(int8 endpoint, int8 * ptr, unsigned int16 len, unsigned int8 timeout);

/******************************************************************************
/* usb_attached()
/*
/* Summary: Returns TRUE if the device is attached to a USB cable.
/*          See the API section of USB.H for more documentation.
/*
/*****************************************************************************/
#if USB_CON_SENSE_PIN
 #define usb_attached() input(USB_CON_SENSE_PIN)
#else
 #define usb_attached() TRUE
#endif

/**************************************************************
/* usb_endpoint_is_valid(endpoint)
/*
/* Input: endpoint - endpoint to check.
/*                   bit 7 is direction (set is IN, clear is OUT)
/*
/* Output: TRUE if endpoint is valid, FALSE if not
/*
/* Summary: Checks the dynamic configuration to see if requested
/*          endpoint is a valid endpoint.
/***************************************************************/
int1 usb_endpoint_is_valid(int8 endpoint);


////// END USER-LEVEL API /////////////////////////////////////////////////////


////// STACK-LEVEL API USED BY HW DRIVERS ////////////////////////////////////

enum USB_STATES {GET_DESCRIPTOR=1,SET_ADDRESS=2,NONE=0};

enum USB_GETDESC_TYPES {USB_GETDESC_CONFIG_TYPE=0,USB_GETDESC_HIDREPORT_TYPE=1,USB_GETDESC_STRING_TYPE=2,USB_GETDESC_DEVICE_TYPE=3};

#if USB_OPT_FOR_ROM
typedef struct {
   USB_STATES dev_req;   //what did the last setup token set us up to do?.  init at none
   int  curr_config;  //our current config.  start at none/powered (NOT THAT THIS LIMITS US TO 3 CONFIGURATIONS)
   int status_device; //Holds our state for Set_Feature and Clear_Feature
   USB_GETDESC_TYPES getdesc_type;   //which get_descriptor() we are handling
} TYPE_USB_STACK_STATUS;
#else
typedef struct {
   USB_STATES dev_req:2;   //what did the last setup token set us up to do?.  init at none
   int  Curr_config:2;  //our current config.  start at none/powered (NOT THAT THIS LIMITS US TO 3 CONFIGURATIONS)
   int status_device:2; //Holds our state for Set_Feature and Clear_Feature
   USB_GETDESC_TYPES getdesc_type:2;   //which get_descriptor() we are handling
} TYPE_USB_STACK_STATUS;
#endif

extern TYPE_USB_STACK_STATUS USB_stack_status;

/**************************************************************
/* usb_token_reset()
/*
/* Output:  No output (but many global registers are modified)
/*
/* Summary: Resets the token handler to initial (unconfigured) state.
/***************************************************************/
void usb_token_reset(void);

/**************************************************************
/* usb_isr_tok_setup_dne()
/*
/* Input: usb_ep0_rx_buffer[] contains the the setup packet.
/*
/* Output: None (many globals are changed)
/*
/* Summary: This function is that handles the setup token.
/*          We must handle all relevant requests, such as Set_Configuration, 
/*          Get_Descriptor, etc.
/*
/*  usb_ep0_rx_buffer[] contains setup data packet, which has the 
/*  following records:
/*  -------------------------------------------------------------------------------------------
/*  usb_ep0_rx_buffer[ 0 ]=bmRequestType; Where the setup packet goes
/*                              bit7   (0) host-to-device
/*                                     (1) device-to-host
/*                              bit6-5 (00) usb standard request;
/*                                     (01) class request;
/*                                     (10) vendor request
/*                                     (11) reserved
/*                              bit4-0 (0000) device
/*                                     (0001) interface
/*                                     (0010) endpoint
/*                                     (0011) other element
/*                                     (0100) to (1111) reserved
/*  usb_ep0_rx_buffer[ 1 ]=bRequest ; the request
/*  usb_ep0_rx_buffer[2,3]=wValue ; a value which corresponds to request
/*  usb_ep0_rx_buffer[4,5]=wIndex ; could correspond to interface or endpoint...
/*  usb_ep0_rx_buffer[6,7]=wLength ; number of bytes in next data packet;
/*    for host-to-device, this exactly how many bytes in data packet.
/*    for device-to-host, this is the maximum bytes that can fit one packet.
/***************************************************************/
void usb_isr_tok_setup_dne(void);

/**************************************************************
/* usb_isr_tok_out_dne()
/*
/* Input: endpoint contains which endpoint we are receiving data (0..15)
/*
/* Summary: Processes out tokens (out is respective of the host, so actualy 
/*          incoming to the pic), but not out setup tokens.  Normally when
/*          data is received it is left in the buffer (user would use
/*          usb_kbhit() and usb_get_packet() to receive data), but certain
/*          libraries (like CDC) have to answer setup packets.
/*          
/***************************************************************/
void usb_isr_tok_out_dne(int8 endpoint);

/**************************************************************
/* usb_isr_tok_in_dne(endpoint)
/*
/* Input: endpoint - which endpoint we are processing a setup token.
/*
/* Summary: This handles an IN packet (HOST <- PIC).  For endpoint 0, this
/*    is usually to setup a response packet to a setup packet.  Endpoints 1..15
/*    are generally ignored, and the user has to use usb_tbe() to determine if
/*    if the buffer is ready for a new transmit packet (there are special cases,
/*    like CDC which handles the CDC protocl).
/*
/***************************************************************/
void usb_isr_tok_in_dne(int8 endpoint);

////// END STACK-LEVEL API USED BY HW DRIVERS /////////////////////////////////


//CCS only supports one configuration at this time
#DEFINE USB_NUM_CONFIGURATIONS 1 //DO NOT CHANGE

//PID values for tokens (see page 48 of USB Complete ed.1)
#define PID_IN       0x09  //device to host transactions
#define PID_OUT      0x01  //host to device transactions
#define PID_SETUP    0x0D  //host to device setup transaction
#define PID_ACK      0x02  //receiver accepts error-free data packet
#define PID_DATA0    0x03  //data packet with even sync bit
#define PID_SOF      0x05  //start of framer marker and frame number
#define PID_NAK      0x0A  //receiver can't accept data or sender cant send data or has no data to transmit
#define PID_DATA1    0x0B  //data packet with odd sync bit
#define PID_PRE      0x0C  //preamble issued by host.  enables downstream traffic to low-speed device
#define PID_STALL    0x0E  //a control request isnt supported or the endpoint is halted

//Key which identifies descritpors
#DEFINE USB_DESC_DEVICE_TYPE     0x01  //#DEFINE USB_DEVICE_DESC_KEY      0x01
#DEFINE USB_DESC_CONFIG_TYPE     0x02  //#DEFINE USB_CONFIG_DESC_KEY      0x02
#DEFINE USB_DESC_STRING_TYPE     0x03  //#DEFINE USB_STRING_DESC_KEY      0x03
#DEFINE USB_DESC_INTERFACE_TYPE  0x04  //#DEFINE USB_INTERFACE_DESC_KEY   0x04
#DEFINE USB_DESC_ENDPOINT_TYPE   0x05  //#DEFINE USB_ENDPOINT_DESC_KEY    0x05
#DEFINE USB_DESC_CLASS_TYPE      0x21  //#DEFINE USB_CLASS_DESC_KEY       0x21
#DEFINE USB_DESC_HIDREPORT_TYPE  0x22

//The length of each descriptor
#DEFINE USB_DESC_DEVICE_LEN      18 //#DEFINE USB_DEVICE_DESC_LEN      18
#DEFINE USB_DESC_CONFIG_LEN      9  //#DEFINE USB_CONFIG_DESC_LEN      9
#DEFINE USB_DESC_INTERFACE_LEN   9  //#DEFINE USB_INTERFACE_DESC_LEN   9
#DEFINE USB_DESC_CLASS_LEN       9  //#DEFINE USB_CLASS_DESC_LEN       9
#DEFINE USB_DESC_ENDPOINT_LEN    7  //#DEFINE USB_ENDPOINT_DESC_LEN    7

//Standard USB Setup bRequest Codes
#define USB_STANDARD_REQUEST_GET_STATUS         0x00
#define USB_STANDARD_REQUEST_CLEAR_FEATURE      0x01
#define USB_STANDARD_REQUEST_SET_FEATURE        0x03
#define USB_STANDARD_REQUEST_SET_ADDRESS        0x05
#define USB_STANDARD_REQUEST_GET_DESCRIPTOR     0x06
#define USB_STANDARD_REQUEST_SET_DESCRIPTOR     0x07
#define USB_STANDARD_REQUEST_GET_CONFIGURATION  0x08
#define USB_STANDARD_REQUEST_SET_CONFIGURATION  0x09
#define USB_STANDARD_REQUEST_GET_INTERFACE      0x0A
#define USB_STANDARD_REQUEST_SET_INTERFACE      0x0B
#define USB_STANDARD_REQUEST_SYNCH_FRAME        0x0C

//HID Class Setup bRequest Codes
#define USB_HID_REQUEST_GET_REPORT     0x01
#define USB_HID_REQUEST_GET_IDLE       0x02
#define USB_HID_REQUEST_GET_PROTOCOL   0x03
#define USB_HID_REQUEST_SET_REPORT     0x09
#define USB_HID_REQUEST_SET_IDLE       0x0A
#define USB_HID_REQUEST_SET_PROTOCOL   0x0B

//types of endpoints as defined in the descriptor
#define USB_ENDPOINT_TYPE_CONTROL      0x00
#define USB_ENDPOINT_TYPE_ISOCHRONOUS  0x01
#define USB_ENDPOINT_TYPE_BULK         0x02
#define USB_ENDPOINT_TYPE_INTERRUPT    0x03

//types of endpoints used internally in this api
#define USB_ENABLE_DISABLED     -1
#define USB_ENABLE_BULK         USB_ENDPOINT_TYPE_BULK
#define USB_ENABLE_ISOCHRONOUS  USB_ENDPOINT_TYPE_ISOCHRONOUS
#define USB_ENABLE_INTERRUPT    USB_ENDPOINT_TYPE_INTERRUPT
#define USB_ENABLE_CONTROL      USB_ENDPOINT_TYPE_CONTROL


//*** ENABLE RX ENDPOINTS AND BUFFERS

//--------- endpoint 0 defines ----------
#define USB_EP0_TX_ENABLE  USB_ENABLE_CONTROL
#define USB_EP0_RX_ENABLE  USB_ENABLE_CONTROL
#define USB_EP0_RX_SIZE    USB_MAX_EP0_PACKET_LENGTH  //endpoint 0 is setup, and should always be the MAX_PACKET_LENGTH.  Slow speed specifies 8
#define USB_EP0_TX_SIZE    USB_MAX_EP0_PACKET_LENGTH  //endpoint 0 is setup, and should always be the MAX_PACKET_LENGTH.  Slow speed specifies 8

//--------- endpoint 1 defines ----------
#ifndef USB_EP1_TX_ENABLE
 #define USB_EP1_TX_ENABLE USB_ENABLE_DISABLED
#endif
#ifndef USB_EP1_RX_ENABLE
 #define USB_EP1_RX_ENABLE USB_ENABLE_DISABLED
#endif

#if USB_EP1_RX_ENABLE==USB_ENABLE_DISABLED
 #ifdef USB_EP1_RX_SIZE
  #undef USB_EP1_RX_SIZE
 #endif
 #define USB_EP1_RX_SIZE 0
#else
 #ifndef USB_EP1_RX_SIZE
  #error You enabled EP1 for RX but didn't specify endpoint size
 #endif
#endif

#if USB_EP1_TX_ENABLE==USB_ENABLE_DISABLED
 #ifdef USB_EP1_TX_SIZE
  #undef USB_EP1_TX_SIZE
 #endif
 #define USB_EP1_TX_SIZE 0
#else
 #ifndef USB_EP1_TX_SIZE
  #error You enabled EP1 for TX but didn't specify endpoint size
 #endif
#endif


//--------- endpoint 2 defines ----------
#ifndef USB_EP2_TX_ENABLE
 #define USB_EP2_TX_ENABLE USB_ENABLE_DISABLED
#endif
#ifndef USB_EP2_RX_ENABLE
 #define USB_EP2_RX_ENABLE USB_ENABLE_DISABLED
#endif

#if USB_EP2_RX_ENABLE==USB_ENABLE_DISABLED
 #ifdef USB_EP2_RX_SIZE
  #undef USB_EP2_RX_SIZE
 #endif
 #define USB_EP2_RX_SIZE 0
#else
 #ifndef USB_EP2_RX_SIZE
  #error You enabled EP2 for RX but didn't specify endpoint size
 #endif
#endif

#if USB_EP2_TX_ENABLE==USB_ENABLE_DISABLED
 #ifdef USB_EP2_TX_SIZE
  #undef USB_EP2_TX_SIZE
 #endif
 #define USB_EP2_TX_SIZE 0
#else
 #ifndef USB_EP2_TX_SIZE
  #error You enabled EP2 for TX but didn't specify endpoint size
 #endif
#endif


//--------- endpoint 3 defines ----------
#ifndef USB_EP3_TX_ENABLE
 #define USB_EP3_TX_ENABLE USB_ENABLE_DISABLED
#endif
#ifndef USB_EP3_RX_ENABLE
 #define USB_EP3_RX_ENABLE USB_ENABLE_DISABLED
#endif

#if USB_EP3_RX_ENABLE==USB_ENABLE_DISABLED
 #ifdef USB_EP3_RX_SIZE
  #undef USB_EP3_RX_SIZE
 #endif
 #define USB_EP3_RX_SIZE 0
#else
 #ifndef USB_EP3_RX_SIZE
  #error You enabled EP3 for RX but didn't specify endpoint size
 #endif
#endif

#if USB_EP3_TX_ENABLE==USB_ENABLE_DISABLED
 #ifdef USB_EP3_TX_SIZE
  #undef USB_EP3_TX_SIZE
 #endif
 #define USB_EP3_TX_SIZE 0
#else
 #ifndef USB_EP3_TX_SIZE
  #error You enabled EP3 for TX but didn't specify endpoint size
 #endif
#endif


//--------- endpoint 4 defines ----------
#ifndef USB_EP4_TX_ENABLE
 #define USB_EP4_TX_ENABLE USB_ENABLE_DISABLED
#endif
#ifndef USB_EP4_RX_ENABLE
 #define USB_EP4_RX_ENABLE USB_ENABLE_DISABLED
#endif

#if USB_EP4_RX_ENABLE==USB_ENABLE_DISABLED
 #ifdef USB_EP4_RX_SIZE
  #undef USB_EP4_RX_SIZE
 #endif
 #define USB_EP4_RX_SIZE 0
#else
 #ifndef USB_EP4_RX_SIZE
  #error You enabled EP4 for RX but didn't specify endpoint size
 #endif
#endif

#if USB_EP4_TX_ENABLE==USB_ENABLE_DISABLED
 #ifdef USB_EP4_TX_SIZE
  #undef USB_EP4_TX_SIZE
 #endif
 #define USB_EP4_TX_SIZE 0
#else
 #ifndef USB_EP4_TX_SIZE
  #error You enabled EP4 for TX but didn't specify endpoint size
 #endif
#endif


//--------- endpoint 5 defines ----------
#ifndef USB_EP5_TX_ENABLE
 #define USB_EP5_TX_ENABLE USB_ENABLE_DISABLED
#endif
#ifndef USB_EP5_RX_ENABLE
 #define USB_EP5_RX_ENABLE USB_ENABLE_DISABLED
#endif

#if USB_EP5_RX_ENABLE==USB_ENABLE_DISABLED
 #ifdef USB_EP5_RX_SIZE
  #undef USB_EP5_RX_SIZE
 #endif
 #define USB_EP5_RX_SIZE 0
#else
 #ifndef USB_EP5_RX_SIZE
  #error You enabled EP5 for RX but didn't specify endpoint size
 #endif
#endif

#if USB_EP5_TX_ENABLE==USB_ENABLE_DISABLED
 #ifdef USB_EP5_TX_SIZE
  #undef USB_EP5_TX_SIZE
 #endif
 #define USB_EP5_TX_SIZE 0
#else
 #ifndef USB_EP5_TX_SIZE
  #error You enabled EP5 for TX but didn't specify endpoint size
 #endif
#endif


//--------- endpoint 6 defines ----------
#ifndef USB_EP6_TX_ENABLE
 #define USB_EP6_TX_ENABLE USB_ENABLE_DISABLED
#endif
#ifndef USB_EP6_RX_ENABLE
 #define USB_EP6_RX_ENABLE USB_ENABLE_DISABLED
#endif

#if USB_EP6_RX_ENABLE==USB_ENABLE_DISABLED
 #ifdef USB_EP6_RX_SIZE
  #undef USB_EP6_RX_SIZE
 #endif
 #define USB_EP6_RX_SIZE 0
#else
 #ifndef USB_EP6_RX_SIZE
  #error You enabled EP6 for RX but didn't specify endpoint size
 #endif
#endif

#if USB_EP6_TX_ENABLE==USB_ENABLE_DISABLED
 #ifdef USB_EP6_TX_SIZE
  #undef USB_EP6_TX_SIZE
 #endif
 #define USB_EP6_TX_SIZE 0
#else
 #ifndef USB_EP6_TX_SIZE
  #error You enabled EP6 for TX but didn't specify endpoint size
 #endif
#endif


//--------- endpoint 7 defines ----------
#ifndef USB_EP7_TX_ENABLE
 #define USB_EP7_TX_ENABLE USB_ENABLE_DISABLED
#endif
#ifndef USB_EP7_RX_ENABLE
 #define USB_EP7_RX_ENABLE USB_ENABLE_DISABLED
#endif

#if USB_EP7_RX_ENABLE==USB_ENABLE_DISABLED
 #ifdef USB_EP7_RX_SIZE
  #undef USB_EP7_RX_SIZE
 #endif
 #define USB_EP7_RX_SIZE 0
#else
 #ifndef USB_EP7_RX_SIZE
  #error You enabled EP7 for RX but didn't specify endpoint size
 #endif
#endif

#if USB_EP7_TX_ENABLE==USB_ENABLE_DISABLED
 #ifdef USB_EP7_TX_SIZE
  #undef USB_EP7_TX_SIZE
 #endif
 #define USB_EP7_TX_SIZE 0
#else
 #ifndef USB_EP7_TX_SIZE
  #error You enabled EP7 for TX but didn't specify endpoint size
 #endif
#endif


//--------- endpoint 8 defines ----------
#ifndef USB_EP8_TX_ENABLE
 #define USB_EP8_TX_ENABLE USB_ENABLE_DISABLED
#endif
#ifndef USB_EP8_RX_ENABLE
 #define USB_EP8_RX_ENABLE USB_ENABLE_DISABLED
#endif

#if USB_EP8_RX_ENABLE==USB_ENABLE_DISABLED
 #ifdef USB_EP8_RX_SIZE
  #undef USB_EP8_RX_SIZE
 #endif
 #define USB_EP8_RX_SIZE 0
#else
 #ifndef USB_EP8_RX_SIZE
  #error You enabled EP8 for RX but didn't specify endpoint size
 #endif
#endif

#if USB_EP8_TX_ENABLE==USB_ENABLE_DISABLED
 #ifdef USB_EP8_TX_SIZE
  #undef USB_EP8_TX_SIZE
 #endif
 #define USB_EP8_TX_SIZE 0
#else
 #ifndef USB_EP8_TX_SIZE
  #error You enabled EP8 for TX but didn't specify endpoint size
 #endif
#endif


//--------- endpoint 9 defines ----------
#ifndef USB_EP9_TX_ENABLE
 #define USB_EP9_TX_ENABLE USB_ENABLE_DISABLED
#endif
#ifndef USB_EP9_RX_ENABLE
 #define USB_EP9_RX_ENABLE USB_ENABLE_DISABLED
#endif

#if USB_EP9_RX_ENABLE==USB_ENABLE_DISABLED
 #ifdef USB_EP9_RX_SIZE
  #undef USB_EP9_RX_SIZE
 #endif
 #define USB_EP9_RX_SIZE 0
#else
 #ifndef USB_EP9_RX_SIZE
  #error You enabled EP9 for RX but didn't specify endpoint size
 #endif
#endif

#if USB_EP9_TX_ENABLE==USB_ENABLE_DISABLED
 #ifdef USB_EP9_TX_SIZE
  #undef USB_EP9_TX_SIZE
 #endif
 #define USB_EP9_TX_SIZE 0
#else
 #ifndef USB_EP9_TX_SIZE
  #error You enabled EP9 for TX but didn't specify endpoint size
 #endif
#endif


//--------- endpoint 10 defines ----------
#ifndef USB_EP10_TX_ENABLE
 #define USB_EP10_TX_ENABLE USB_ENABLE_DISABLED
#endif
#ifndef USB_EP10_RX_ENABLE
 #define USB_EP10_RX_ENABLE USB_ENABLE_DISABLED
#endif

#if USB_EP10_RX_ENABLE==USB_ENABLE_DISABLED
 #ifdef USB_EP10_RX_SIZE
  #undef USB_EP10_RX_SIZE
 #endif
 #define USB_EP10_RX_SIZE 0
#else
 #ifndef USB_EP10_RX_SIZE
  #error You enabled EP10 for RX but didn't specify endpoint size
 #endif
#endif

#if USB_EP10_TX_ENABLE==USB_ENABLE_DISABLED
 #ifdef USB_EP10_TX_SIZE
  #undef USB_EP10_TX_SIZE
 #endif
 #define USB_EP10_TX_SIZE 0
#else
 #ifndef USB_EP10_TX_SIZE
  #error You enabled EP10 for TX but didn't specify endpoint size
 #endif
#endif


//--------- endpoint 11 defines ----------
#ifndef USB_EP11_TX_ENABLE
 #define USB_EP11_TX_ENABLE USB_ENABLE_DISABLED
#endif
#ifndef USB_EP11_RX_ENABLE
 #define USB_EP11_RX_ENABLE USB_ENABLE_DISABLED
#endif

#if USB_EP11_RX_ENABLE==USB_ENABLE_DISABLED
 #ifdef USB_EP11_RX_SIZE
  #undef USB_EP11_RX_SIZE
 #endif
 #define USB_EP11_RX_SIZE 0
#else
 #ifndef USB_EP11_RX_SIZE
  #error You enabled EP11 for RX but didn't specify endpoint size
 #endif
#endif

#if USB_EP11_TX_ENABLE==USB_ENABLE_DISABLED
 #ifdef USB_EP11_TX_SIZE
  #undef USB_EP11_TX_SIZE
 #endif
 #define USB_EP11_TX_SIZE 0
#else
 #ifndef USB_EP11_TX_SIZE
  #error You enabled EP11 for TX but didn't specify endpoint size
 #endif
#endif


//--------- endpoint 12 defines ----------
#ifndef USB_EP12_TX_ENABLE
 #define USB_EP12_TX_ENABLE USB_ENABLE_DISABLED
#endif
#ifndef USB_EP12_RX_ENABLE
 #define USB_EP12_RX_ENABLE USB_ENABLE_DISABLED
#endif

#if USB_EP12_RX_ENABLE==USB_ENABLE_DISABLED
 #ifdef USB_EP12_RX_SIZE
  #undef USB_EP12_RX_SIZE
 #endif
 #define USB_EP12_RX_SIZE 0
#else
 #ifndef USB_EP12_RX_SIZE
  #error You enabled EP12 for RX but didn't specify endpoint size
 #endif
#endif

#if USB_EP12_TX_ENABLE==USB_ENABLE_DISABLED
 #ifdef USB_EP12_TX_SIZE
  #undef USB_EP12_TX_SIZE
 #endif
 #define USB_EP12_TX_SIZE 0
#else
 #ifndef USB_EP12_TX_SIZE
  #error You enabled EP12 for TX but didn't specify endpoint size
 #endif
#endif


//--------- endpoint 13 defines ----------
#ifndef USB_EP13_TX_ENABLE
 #define USB_EP13_TX_ENABLE USB_ENABLE_DISABLED
#endif
#ifndef USB_EP13_RX_ENABLE
 #define USB_EP13_RX_ENABLE USB_ENABLE_DISABLED
#endif

#if USB_EP13_RX_ENABLE==USB_ENABLE_DISABLED
 #ifdef USB_EP13_RX_SIZE
  #undef USB_EP13_RX_SIZE
 #endif
 #define USB_EP13_RX_SIZE 0
#else
 #ifndef USB_EP13_RX_SIZE
  #error You enabled EP13 for RX but didn't specify endpoint size
 #endif
#endif

#if USB_EP13_TX_ENABLE==USB_ENABLE_DISABLED
 #ifdef USB_EP13_TX_SIZE
  #undef USB_EP13_TX_SIZE
 #endif
 #define USB_EP13_TX_SIZE 0
#else
 #ifndef USB_EP13_TX_SIZE
  #error You enabled EP13 for TX but didn't specify endpoint size
 #endif
#endif


//--------- endpoint 14 defines ----------
#ifndef USB_EP14_TX_ENABLE
 #define USB_EP14_TX_ENABLE USB_ENABLE_DISABLED
#endif
#ifndef USB_EP14_RX_ENABLE
 #define USB_EP14_RX_ENABLE USB_ENABLE_DISABLED
#endif

#if USB_EP14_RX_ENABLE==USB_ENABLE_DISABLED
 #ifdef USB_EP14_RX_SIZE
  #undef USB_EP14_RX_SIZE
 #endif
 #define USB_EP14_RX_SIZE 0
#else
 #ifndef USB_EP14_RX_SIZE
  #error You enabled EP14 for RX but didn't specify endpoint size
 #endif
#endif

#if USB_EP14_TX_ENABLE==USB_ENABLE_DISABLED
 #ifdef USB_EP14_TX_SIZE
  #undef USB_EP14_TX_SIZE
 #endif
 #define USB_EP14_TX_SIZE 0
#else
 #ifndef USB_EP14_TX_SIZE
  #error You enabled EP14 for TX but didn't specify endpoint size
 #endif
#endif


//--------- endpoint 15 defines ----------
#ifndef USB_EP15_TX_ENABLE
 #define USB_EP15_TX_ENABLE USB_ENABLE_DISABLED
#endif
#ifndef USB_EP15_RX_ENABLE
 #define USB_EP15_RX_ENABLE USB_ENABLE_DISABLED
#endif

#if USB_EP15_RX_ENABLE==USB_ENABLE_DISABLED
 #ifdef USB_EP15_RX_SIZE
  #undef USB_EP15_RX_SIZE
 #endif
 #define USB_EP15_RX_SIZE 0
#else
 #ifndef USB_EP15_RX_SIZE
  #error You enabled EP15 for RX but didn't specify endpoint size
 #endif
#endif

#if USB_EP15_TX_ENABLE==USB_ENABLE_DISABLED
 #ifdef USB_EP15_TX_SIZE
  #undef USB_EP15_TX_SIZE
 #endif
 #define USB_EP15_TX_SIZE 0
#else
 #ifndef USB_EP15_TX_SIZE
  #error You enabled EP15 for TX but didn't specify endpoint size
 #endif
#endif

const int8 usb_ep_tx_type[16]={
  USB_EP0_TX_ENABLE, USB_EP1_TX_ENABLE, USB_EP2_TX_ENABLE,
  USB_EP3_TX_ENABLE, USB_EP4_TX_ENABLE, USB_EP5_TX_ENABLE,
  USB_EP6_TX_ENABLE, USB_EP7_TX_ENABLE, USB_EP8_TX_ENABLE,
  USB_EP9_TX_ENABLE, USB_EP10_TX_ENABLE, USB_EP11_TX_ENABLE,
  USB_EP12_TX_ENABLE, USB_EP13_TX_ENABLE, USB_EP14_TX_ENABLE,
  USB_EP15_TX_ENABLE
};

const int8 usb_ep_rx_type[16]={
  USB_EP0_RX_ENABLE, USB_EP1_RX_ENABLE, USB_EP2_RX_ENABLE,
  USB_EP3_RX_ENABLE, USB_EP4_RX_ENABLE, USB_EP5_RX_ENABLE,
  USB_EP6_RX_ENABLE, USB_EP7_RX_ENABLE, USB_EP8_RX_ENABLE,
  USB_EP9_RX_ENABLE, USB_EP10_RX_ENABLE, USB_EP11_RX_ENABLE,
  USB_EP12_RX_ENABLE, USB_EP13_RX_ENABLE, USB_EP14_RX_ENABLE,
  USB_EP15_RX_ENABLE
};

const unsigned int16 usb_ep_tx_size[16]={
  USB_EP0_TX_SIZE, USB_EP1_TX_SIZE, USB_EP2_TX_SIZE,
  USB_EP3_TX_SIZE, USB_EP4_TX_SIZE, USB_EP5_TX_SIZE,
  USB_EP6_TX_SIZE, USB_EP7_TX_SIZE, USB_EP8_TX_SIZE,
  USB_EP9_TX_SIZE, USB_EP10_TX_SIZE, USB_EP11_TX_SIZE,
  USB_EP12_TX_SIZE, USB_EP13_TX_SIZE, USB_EP14_TX_SIZE,
  USB_EP15_TX_SIZE
};

const unsigned int16 usb_ep_rx_size[16]={
  USB_EP0_RX_SIZE, USB_EP1_RX_SIZE, USB_EP2_RX_SIZE,
  USB_EP3_RX_SIZE, USB_EP4_RX_SIZE, USB_EP5_RX_SIZE,
  USB_EP6_RX_SIZE, USB_EP7_RX_SIZE, USB_EP8_RX_SIZE,
  USB_EP9_RX_SIZE, USB_EP10_RX_SIZE, USB_EP11_RX_SIZE,
  USB_EP12_RX_SIZE, USB_EP13_RX_SIZE, USB_EP14_RX_SIZE,
  USB_EP15_RX_SIZE
};

#ENDIF
