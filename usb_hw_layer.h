///////////////////////////////////////////////////////////////////////////
////                         usb_hw_layer.h                            ////
////                                                                   ////
//// HW Layer for the USB Stack.                                       ////
////                                                                   ////
//// HW drivers for the USB stack must support this API.               ////
////                                                                   ////
//// The HW driver must also have a token handler that will call       ////
//// usb_token_reset() when the device is init/reset, will call        ////
//// usb_isr_tok_setup_dne() when it receives a setup packet,          ////
//// will call usb_isr_tok_out_dne() when a new packet is received,    ////
//// and will call usb_isr_tok_in_dne() when a new packet is sent.     ////
//// See usb.h for documentation of these functions.                   ////
////                                                                   ////
///////////////////////////////////////////////////////////////////////////
////                                                                   ////
//// Version History:                                                  ////
////                                                                   ////
//// March 5th, 2009:                                                  ////
////   Cleanup for Wizard.                                             ////
////   PIC24 Initial release.                                          ////
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

#ifndef __USB_HW_LAYER_H__
#define __USB_HW_LAYER_H__

enum USB_DTS_BIT {USB_DTS_DATA1=1, USB_DTS_TOGGLE=2, USB_DTS_DATA0=0,
                   USB_DTS_STALL=3, USB_DTS_USERX=4};


// USER LEVEL FUNCTIONS:

/*****************************************************************************
/* usb_init()
/*
/* Summary: See API section of USB.H for documentation.
/*
/*****************************************************************************/
void usb_init(void);

/*****************************************************************************
/* usb_init_cs()
/*
/* Summary: See API section of USB.H for documentation.
/*
/*****************************************************************************/
void usb_init_cs(void);

/*****************************************************************************
/* usb_task()
/*
/* Summary: See API section of USB.H for documentation.
/*
/*****************************************************************************/
void usb_task(void);

/******************************************************************************
/* usb_attach()
/*
/* Summary: Attach the D+/D- lines to the USB bus.  Enable the USB peripheral.
/*
/* You should wait until UCON_SE0 is clear before enabling reset/idle interrupt.
/* 
/* Shouldn't be called by user, let usb_task() handle this.
/*
/* NOTE: If you are writing a new HW driver, this routine should call 
/*       usb_token_reset() to reset the CH9 stack.
/*
/*****************************************************************************/
void usb_attach(void);

/******************************************************************************
/* usb_detach()
/*
/* Summary: Remove the D+/D- lines from the USB bus.  Basically, disable USB.
/*
/* Shouldn't be called by user, let usb_task() handle this.
/*
/* If the user does call this function while USB is connected, the USB 
/* peripheral will be disabled only momentarily because usb_task() will
/* reconnect.  But this is a great way to cause the PC/HOST to cause a USB
/* reset of the device.
/*
/* NOTE: If you are writing a new HW driver, this routine should call 
/*       usb_token_reset() to reset the CH9 stack.
/*
/*****************************************************************************/
void usb_detach(void);

/******************************************************************************
/* usb_put_packet()
/*
/* Input: endpoint - endpoint to send packet to (0..15).
/*        ptr - points to data to send.
/*        len - amount of data to send.
/*        toggle - whether to send data with a DATA0 pid, a DATA1 pid, 
/*                 or toggle from the last DATAx pid.
/*
/* Output: TRUE if data was sent correctly, FALSE if it was not.  The only 
/*         reason it will return FALSE is if because the TX buffer is still full 
/*         from the last time it tried to send a packet.
/*
/* Summary: Sends one packet out the EP to the host.  Notice that there is a 
/*          difference between a packet and a message.  If you wanted to send a 
/*          512 byte message on an endpoint that only supported 64 byte packets,
/*          this would be accomplished this by sending 8 64-byte packets, 
/*          followed by a 0 length packet.  If the last (or only packet) being 
/*          sent is less than the max packet size defined in your descriptor 
/*          then you do not need to send a 0 length packet to identify
/*          an end of message.
/*
/*          usb_puts() (provided in usb.c) will send a multi-packet message 
/*          correctly and know if a 0 lenght packet needs to be sent.
/*
/* The result will be invalid if the specified input has not been configured for
/* use by the API with USB_EPx_TX_SIZE
/*
/*****************************************************************************/
int1 usb_put_packet(int8 endpoint, int8 * ptr, unsigned int16 len, USB_DTS_BIT tgl);

/*******************************************************************************
/* usb_get_packet(endpoint, *ptr, max)
/*
/* Input: endpoint - endpoint to get data from
/*        ptr - where to save data to local PIC RAM
/*        max - max amount of data to receive from buffer
/*
/* Output: the amount of data taken from the buffer and saved to ptr.
/*
/*         NOTE - IF THERE IS NO PACKET TO GET YOU WILL GET INVALID RESULTS!
/*                VERIFY WITH USB_KBHIT() BEFORE YOU CALL USB_GET_PACKET()!
/*
/* Summary: Gets a packet of data from the USB endpoint buffer.
/*          Until this is called, if there is data in the receive buffer the
/*          PC/HOST will be prevented from sending more data.  Upon completion
/*          of this call, the endpoint will be freed and the PC/HOST will be
/*          allowed to send a new packet.  Only receives one packet, if you need 
/*          to receive multiple packets or handle 0-length terminator packets,
/*          then use usb_gets().
/*
/* The result will be invalid if the specified input has not been configured for
/* use by the API with USB_EPx_RX_SIZE
/*
/********************************************************************************/
unsigned int16 usb_get_packet(int8 endpoint, int8 * ptr, unsigned int16 max);

/****************************************************************************
/* usb_kbhit(endpoint)
/*
/* Input: endpoint - endpoint to check (0..15)
/*
/* Output: TRUE if there is new data in RX buffer, FALSE if there is not.
/*
/* Summary: Similar to kbhit(), sees if there is new data in the RX USB buffers.
/*          See API section of USB.H for more information.
/*
/* The result will be invalid if the specified input has not been configured for
/* use by the API with USB_EPx_RX_SIZE
/*
/*****************************************************************************/
int1 usb_kbhit(int8 en);

/******************************************************************************
/* usb_tbe(endpoint)
/*
/* Input: endpoint - endpoint to check
/*        ptr - where to save data to local PIC RAM
/*        max - max amount of data to receive from buffer
/*
/* Output: returns TRUE if this endpoint's IN buffer (PIC-PC) is empty and ready
/*         returns FALSE if this endpoint's IN buffer is still processing the last
/*         transmit or if this endpoint is invalid.
/*
/* The result will be invalid if the specified input has not been configured for
/* use by the API with USB_EPx_TX_SIZE
/*
/*****************************************************************************/
int1 usb_tbe(int8 en);

//////////////// END USER LEVEL FUNCTIONS /////////////////////////////////////


///
/// BEGIN STACK LEVEL FUNCTIONS: These functions are meant to be called by
///            the USB stack, and are not meant to be called by the 
///            user/application.
///

/*****************************************************************************
/* usb_stall_ep()
/*
/* Input: endpoint - endpoint to stall.
/*                   top most bit indicates direction (set is IN, clear is OUT)
/*
/* Summary: Stalls specified endpoint.  If endpoint is stalled it will
/*          send STALL packet if the host tries to access this endpoint's 
/*          buffer.
/*
/* NOTE: WE ASSUME ENDPOINT IS VALID.  USB.C SHOULD CHECK IF VALID BEFORE
/*       CALLING THIS ROUTINE.
/*
/*****************************************************************************/
void usb_stall_ep(int8 endpoint);


/*****************************************************************************
/* usb_unstall_ep(endpoint, direction)
/*
/* Input: endpoint - endpoint to un-stall.
/*                   top most bit indicates direction (set is IN, clear is OUT)
/*
/* Summary: Un-stalls endpoint.
/*
/* NOTE: WE ASSUME ENDPOINT IS VALID.  USB.C SHOULD CHECK IF VALID BEFORE
/*       CALLING THIS ROUTINE.
/*
/*****************************************************************************/
void usb_unstall_ep(int8 endpoint);

/*****************************************************************************
/* usb_endpoint_stalled()
/*
/* Input: endpoint - endpoint to check
/*                   top most bit indicates direction (set is IN, clear is OUT)
/*
/* Output: returns a TRUE if endpoint is stalled, FALSE if it is not.
/*
/* Summary: Looks to see if an endpoint is stalled, or not.  Does not look to
/*          see if endpoint has been issued a STALL, just whether or not it is
/*          configured to STALL on the next packet.  See Set_Feature and 
/*          Clear_Feature Chapter 9 requests.
/*
/* NOTE: WE ASSUME ENDPOINT IS VALID.  USB.C SHOULD CHECK IF VALID BEFORE
/*       CALLING THIS ROUTINE.
/*
/*****************************************************************************/
int1 usb_endpoint_stalled(int8 endpoint);

/*****************************************************************************
/* usb_set_address(address)
/*
/* Input: address - address the host specified that we use
/*
/* Summary: Configures the USB Peripheral for the specified device address.  
/*          The host will now talk to use with the following address.
/*
/*****************************************************************************/
void usb_set_address(int8 address);

/******************************************************************************
/* usb_set_configured()
/*
/* Input: config - Configuration to use.  0 to uncofigure device.
/*
/* Summary: Configures or unconfigures device.  If configuring device it will
/*          enable all the endpoints the user specified for this configuration.
/*          If un-configuring device it will disable all endpoints.
/*
/*          NOTE: CCS only provides code to handle 1 configuration.
/*
/*****************************************************************************/
void usb_set_configured(int8 config);

/******************************************************************************
/* usb_disable_endpoints()
/*
/* Summary: Disables endpoints (all endpoints but 0)
/*
/*****************************************************************************/
void usb_disable_endpoints(void);


/*******************************************************************************
/* usb_disable_endpoint()
/*
/* Input: Endpoint to disable (0..15)
/*
/* Summary: Disables specified endpoint
/*
/********************************************************************************/
void usb_disable_endpoint(int8 en);

/**************************************************************
/* usb_request_send_response(len)
/* usb_request_get_data()
/* usb_request_stall()
/*
/* Input: len - size of packet to send
/*
/* Summary: One of these 3 functions will be called by the USB stack after
/*          handling a SETUP packet.
/*          
/*          After we process a SETUP request, we have 1 of three responses:
/*            1.) send a response IN packet
/*            2.) wait for followup OUT packet(s) with data
/*            3.) stall because we don't support that SETUP request
/*
/*          If we are sending data, the array usb_ep0_tx_buffer[] will hold
/*          the response and the USB Request handler code will call
/*          usb_request_send_response() to let us know how big the packet is.
/*
/*          If we are waiting for more data, usb_request_get_data() will
/*          be called by the USB request handler code to configure the EP0 OUT
/*          endpoint to be ready for more data
/*
/*          If we don't support a request, usb_request_stall() will be called
/*          by the USB request handler code to stall the endpoint 0.
/*
/*          The operation of these functions depends on how SETUP packets
/*          are handled in the hardware layer.
/*
/***************************************************************/
void usb_request_send_response(unsigned int8 len);
void usb_request_get_data(void);
void usb_request_stall(void);


#endif
