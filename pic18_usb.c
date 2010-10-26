/////////////////////////////////////////////////////////////////////////
////                          pic18_usb.c                            ////
////                                                                 ////
//// Hardware layer for CCS's USB library.  See pic18_usb.h more     ////
//// documentation about the PIC18 hardware layer.                   ////
////                                                                 ////
//// This file is part of CCS's PIC USB driver code.  See USB.H      ////
//// for more documentation and a list of examples.                  ////
////                                                                 ////
/////////////////////////////////////////////////////////////////////////
////                                                                 ////
//// Version History:                                                ////
////                                                                 ////
//// Nov 13th, 2009:                                                 ////
////  usb_disable_endpoint() won't touch BD status registers for     ////
////     endpoints that aren't allocated.                            ////
////                                                                 ////
//// June 9th, 2009:                                                 ////
////  Some problems with Vista and fast hubs/hosts fixed on PIC18.   ////
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
////        (C) Copyright 1996,2009 Custom Computer Services         ////
//// This source code may only be used by licensed users of the CCS  ////
//// C compiler.  This source code may only be distributed to other  ////
//// licensed users of the CCS C compiler.  No other use,            ////
//// reproduction or distribution is permitted without written       ////
//// permission.  Derivative programs created using this software    ////
//// in object code form are not restricted in any way.              ////
/////////////////////////////////////////////////////////////////////////

#IFNDEF __PIC18_USB_C__
#DEFINE __PIC18_USB_C__

#INCLUDE "usb.h"

//if you are worried that the PIC is not receiving packets because a bug in the
//DATA0/DATA1 synch code, you can set this to TRUE to ignore the DTS on
//receiving.
#ifndef USB_IGNORE_RX_DTS
 #define USB_IGNORE_RX_DTS FALSE
#endif

#ifndef USB_IGNORE_TX_DTS
 #define USB_IGNORE_TX_DTS FALSE
#endif

#if ((getenv("DEVICE")=="PIC18F87J50") || (getenv("DEVICE")=="PIC18F86J55") || \
     (getenv("DEVICE")=="PIC18F86J50") || (getenv("DEVICE")=="PIC18F85J50") || \
     (getenv("DEVICE")=="PIC18F67J50") || (getenv("DEVICE")=="PIC18F66J55") || \
     (getenv("DEVICE")=="PIC18F66J50") || (getenv("DEVICE")=="PIC18F65J50"))
 #define __USB_87J50__
 #define USB_TOTAL_RAM_SPACE  ((int16)getenv("RAM")-0x400)
 #define USB_RAM_START  0x400
 #define USB_NUM_UEP 16
 #define USB_LAST_ALLOWED_ENDPOINT  16
#elif ((getenv("DEVICE")=="PIC18F24J50") || (getenv("DEVICE")=="PIC18F25J50") || \
       (getenv("DEVICE")=="PIC18F26J50") || (getenv("DEVICE")=="PIC18F44J50") || \
       (getenv("DEVICE")=="PIC18F45J50") || (getenv("DEVICE")=="PIC18F46J50"))
 #define __USB_46J50__
 #define USB_TOTAL_RAM_SPACE  ((int16)getenv("RAM")-0x400)
 #define USB_RAM_START  0x400
 #define USB_NUM_UEP 16
 #define USB_LAST_ALLOWED_ENDPOINT  16
#elif ((getenv("DEVICE")=="PIC18F2450") || (getenv("DEVICE")=="PIC18F4450"))
 #define __USB_4450__
 #define USB_TOTAL_RAM_SPACE  ((int16)0x100)
 #define USB_RAM_START  0x400
 #define USB_NUM_UEP 16
 #define USB_LAST_ALLOWED_ENDPOINT  3
#elif ((getenv("DEVICE")=="PIC18F13K50") || (getenv("DEVICE")=="PIC18F14K50"))
 #define __USB_K50__
 #define USB_TOTAL_RAM_SPACE  ((int16)0x100)
 #define USB_RAM_START  0x200
 #define USB_NUM_UEP 8
 //technically this device supports 8 endpoints, but for RAM reasons you should
 //attempt to only use the first 3 endpoints.
 #define USB_LAST_ALLOWED_ENDPOINT  3
#elif ((getenv("DEVICE")=="PIC18F2455") || (getenv("DEVICE")=="PIC18F2550") || \
       (getenv("DEVICE")=="PIC18F4455") || (getenv("DEVICE")=="PIC18F4550") || \
       (getenv("DEVICE")=="PIC18F2458") || (getenv("DEVICE")=="PIC18F2553") || \
       (getenv("DEVICE")=="PIC18F4458") || (getenv("DEVICE")=="PIC18F4553") \
       )
 #define __USB_4550__
 #define USB_TOTAL_RAM_SPACE  ((int16)0x400)
 #define USB_RAM_START  0x400
 #define USB_NUM_UEP 16
 #define USB_LAST_ALLOWED_ENDPOINT  16
#else
 #error Unknown PIC device, USB not supported in this library.
#endif

#if USB_EP15_TX_SIZE || USB_EP15_RX_SIZE
 #define USB_LAST_DEFINED_ENDPOINT  15
#elif USB_EP14_TX_SIZE || USB_EP14_RX_SIZE
 #define USB_LAST_DEFINED_ENDPOINT  14
#elif USB_EP13_TX_SIZE || USB_EP13_RX_SIZE
 #define USB_LAST_DEFINED_ENDPOINT  13
#elif USB_EP12_TX_SIZE || USB_EP12_RX_SIZE
 #define USB_LAST_DEFINED_ENDPOINT  12
#elif USB_EP11_TX_SIZE || USB_EP11_RX_SIZE
 #define USB_LAST_DEFINED_ENDPOINT  11
#elif USB_EP10_TX_SIZE || USB_EP10_RX_SIZE
 #define USB_LAST_DEFINED_ENDPOINT  10
#elif USB_EP9_TX_SIZE || USB_EP9_RX_SIZE
 #define USB_LAST_DEFINED_ENDPOINT  9
#elif USB_EP8_TX_SIZE || USB_EP8_RX_SIZE
 #define USB_LAST_DEFINED_ENDPOINT  8
#elif USB_EP7_TX_SIZE || USB_EP7_RX_SIZE
 #define USB_LAST_DEFINED_ENDPOINT  7
#elif USB_EP6_TX_SIZE || USB_EP6_RX_SIZE
 #define USB_LAST_DEFINED_ENDPOINT  6
#elif USB_EP5_TX_SIZE || USB_EP5_RX_SIZE
 #define USB_LAST_DEFINED_ENDPOINT  5
#elif USB_EP4_TX_SIZE || USB_EP4_RX_SIZE
 #define USB_LAST_DEFINED_ENDPOINT  4
#elif USB_EP3_TX_SIZE || USB_EP3_RX_SIZE
 #define USB_LAST_DEFINED_ENDPOINT  3
#elif USB_EP2_TX_SIZE || USB_EP2_RX_SIZE
 #define USB_LAST_DEFINED_ENDPOINT  2
#elif USB_EP1_TX_SIZE || USB_EP1_RX_SIZE
 #define USB_LAST_DEFINED_ENDPOINT  1
#else
 #define USB_LAST_DEFINED_ENDPOINT  0
#endif

#define USB_CONTROL_REGISTER_SIZE   ((USB_LAST_DEFINED_ENDPOINT+1)*8)

#define USB_DATA_BUFFER_NEEDED (USB_EP0_TX_SIZE+USB_EP0_RX_SIZE+USB_EP1_TX_SIZE+\
                           USB_EP1_RX_SIZE+USB_EP2_TX_SIZE+USB_EP2_RX_SIZE+\
                           USB_EP3_TX_SIZE+USB_EP3_RX_SIZE+USB_EP4_TX_SIZE+\
                           USB_EP4_RX_SIZE+USB_EP5_TX_SIZE+USB_EP5_RX_SIZE+\
                           USB_EP6_TX_SIZE+USB_EP6_RX_SIZE+USB_EP7_TX_SIZE+\
                           USB_EP7_RX_SIZE+USB_EP8_TX_SIZE+USB_EP8_RX_SIZE+\
                           USB_EP9_TX_SIZE+USB_EP9_RX_SIZE+USB_EP10_TX_SIZE+\
                           USB_EP10_RX_SIZE+USB_EP11_TX_SIZE+USB_EP11_RX_SIZE+\
                           USB_EP12_TX_SIZE+USB_EP12_RX_SIZE+USB_EP13_TX_SIZE+\
                           USB_EP13_RX_SIZE+USB_EP14_TX_SIZE+USB_EP14_RX_SIZE+\
                           USB_EP15_TX_SIZE+USB_EP15_RX_SIZE)

#if ((USB_DATA_BUFFER_NEEDED+USB_CONTROL_REGISTER_SIZE) > USB_TOTAL_RAM_SPACE)
 #error You are trying to allocate more memory for endpoints than the PIC can handle
#endif

#if (USB_LAST_DEFINED_ENDPOINT > USB_LAST_ALLOWED_ENDPOINT)
 #error You are trying ot use an invalid endpoint for this hardware!
#endif

#define USB_DATA_BUFFER_LOCATION ((int16)USB_RAM_START+USB_CONTROL_REGISTER_SIZE)

typedef struct
{
   int8 stat;
   int8 cnt;
   int16 addr;
} STRUCT_BD;

struct
{
   struct
   {
      STRUCT_BD out;    //pc -> pic
      STRUCT_BD in;     //pc <- pic
   } bd[USB_LAST_DEFINED_ENDPOINT+1];
   union
   {
      struct
      {
         int8 ep0_rx_buffer[USB_MAX_EP0_PACKET_LENGTH];
         int8 ep0_tx_buffer[USB_MAX_EP0_PACKET_LENGTH];
         
         //these buffer definitions needed for CDC library
        #if USB_EP1_RX_SIZE
         int8 ep1_rx_buffer[USB_EP1_RX_SIZE];
        #endif
        #if USB_EP1_TX_SIZE
         int8 ep1_tx_buffer[USB_EP1_TX_SIZE];
        #endif
        #if USB_EP2_RX_SIZE
         int8 ep2_rx_buffer[USB_EP2_RX_SIZE];
        #endif
        #if USB_EP2_TX_SIZE
         int8 ep2_tx_buffer[USB_EP2_TX_SIZE];
        #endif
      };
      int8 general[USB_DATA_BUFFER_NEEDED];
   } buffer;
} g_USBRAM;
#locate g_USBRAM=USB_RAM_START

#define usb_ep0_rx_buffer g_USBRAM.buffer.ep0_rx_buffer
#define usb_ep0_tx_buffer g_USBRAM.buffer.ep0_tx_buffer

//these buffer definitions needed for CDC library
#define usb_ep1_rx_buffer g_USBRAM.buffer.ep1_rx_buffer
#define usb_ep1_tx_buffer g_USBRAM.buffer.ep1_tx_buffer
#define usb_ep2_rx_buffer g_USBRAM.buffer.ep2_rx_buffer
#define usb_ep2_tx_buffer g_USBRAM.buffer.ep2_tx_buffer

//if you enable this it will keep a counter of the 6 possible errors the
//pic can detect.  disabling this will save you ROM, RAM and execution time.
#if !defined(USB_USE_ERROR_COUNTER)
   #define USB_USE_ERROR_COUNTER FALSE
#endif

#define USB_PING_PONG_MODE_OFF   0  //no ping pong
#define USB_PING_PONG_MODE_E0    1  //ping pong endpoint 0 only
#define USB_PING_PONG_MODE_ON    2  //ping pong all endpoints

//NOTE - PING PONG MODE IS NOT SUPPORTED BY CCS!
#if !defined(USB_PING_PONG_MODE)
   #define USB_PING_PONG_MODE USB_PING_PONG_MODE_OFF
#endif

#if USB_USE_ERROR_COUNTER
   int ERROR_COUNTER[6];
#endif

//---pic18fxx5x memory locations
#if defined(__USB_4550__) || defined(__USB_4450__)
   #byte UFRML   =  0xF66
   #byte UFRMH   =  0xF67
   #byte UIR     =  0xF68
   #byte UIE     =  0xF69
   #byte UEIR    =  0xF6A
   #byte UEIE    =  0xF6B
   #byte U1STAT   =  0xF6C
   #byte UCON    =  0xF6D
   #byte UADDR   =  0xF6E
   #byte UCFG    =  0xF6F
   #define  UEP0_LOC 0xF70
#elif defined(__USB_46J50__)
   #byte UFRML   =  0xF60
   #byte UFRMH   =  0xF61
   #byte UIR     =  0xF62
   #byte UIE     =  0xF36
   #byte UEIR    =  0xF63
   #byte UEIE    =  0xF37
   #byte U1STAT   =  0xF64
   #byte UCON    =  0xF65
   #byte UADDR   =  0xF38
   #byte UCFG    =  0xF39
   #define  UEP0_LOC 0xF26
#elif defined(__USB_K50__)
   #byte UFRML   =  0xF5D
   #byte UFRMH   =  0xF5E
   #byte UIR     =  0xF62
   #byte UIE     =  0xF60
   #byte UEIR    =  0xF5F
   #byte UEIE    =  0xF5B
   #byte U1STAT   =  0xF63
   #byte UCON    =  0xF64
   #byte UADDR   =  0xF5C
   #byte UCFG    =  0xF61
   #define  UEP0_LOC 0xF53
#else
   #byte UFRML   =  0xF60
   #byte UFRMH   =  0xF61
   #byte UIR     =  0xF62
   #byte UIE     =  0xF5C
   #byte UEIR    =  0xF63
   #byte UEIE    =  0xF5D
   #byte U1STAT   =  0xF64
   #byte UCON    =  0xF65
   #byte UADDR   =  0xF5E
   #byte UCFG    =  0xF5F
   #define  UEP0_LOC 0xF4C
#endif

int8 USTATCopy;

int8 g_UEP[USB_NUM_UEP];
#locate g_UEP=UEP0_LOC
#define UEP(x) g_UEP[x]

#define BIT_SOF   6
#define BIT_STALL 5
#define BIT_IDLE  4
#define BIT_TRN   3
#define BIT_ACTV  2
#define BIT_UERR  1
#define BIT_URST  0

#BIT UIR_SOF = UIR.BIT_SOF
#BIT UIR_STALL = UIR.BIT_STALL
#BIT UIR_IDLE = UIR.BIT_IDLE
#BIT UIR_TRN = UIR.BIT_TRN
#BIT UIR_ACTV = UIR.BIT_ACTV
#BIT UIR_UERR = UIR.BIT_UERR
#BIT UIR_URST = UIR.BIT_URST

#BIT UIE_SOF = UIE.6
#BIT UIE_STALL = UIE.5
#BIT UIE_IDLE = UIE.4
#BIT UIE_TRN = UIE.3
#BIT UIE_ACTV = UIE.2
#BIT UIE_UERR = UIE.1
#BIT UIE_URST = UIE.0

#bit UCON_PBRST=UCON.6
#bit UCON_SE0=UCON.5
#bit UCON_PKTDIS=UCON.4
#bit UCON_USBEN=UCON.3
#bit UCON_RESUME=UCON.2
#bit UCON_SUSPND=UCON.1

#if (USB_PING_PONG_MODE==USB_PING_PONG_MODE_OFF)
 #define EP_BDxST_O(x)    g_USBRAM.bd[x].out.stat
 #define EP_BDxCNT_O(x)   g_USBRAM.bd[x].out.cnt
 #define EP_BDxADR_O(x)   g_USBRAM.bd[x].out.addr
 #define EP_BDxST_I(x)    g_USBRAM.bd[x].in.stat
 #define EP_BDxCNT_I(x)   g_USBRAM.bd[x].in.cnt
 #define EP_BDxADR_I(x)   g_USBRAM.bd[x].in.addr
#else
#error Right now this driver only supports no ping pong
#endif

//See UEPn (0xF70-0xF7F)
#define ENDPT_DISABLED   0x00   //endpoint not used
#define ENDPT_IN_ONLY   0x02    //endpoint supports IN transactions only
#define ENDPT_OUT_ONLY   0x04    //endpoint supports OUT transactions only
#define ENDPT_CONTROL   0x06    //Supports IN, OUT and CONTROL transactions - Only use with EP0
#define ENDPT_NON_CONTROL 0x0E  //Supports both IN and OUT transactions

//Define the states that the USB interface can be in
enum {USB_STATE_DETACHED=0, USB_STATE_ATTACHED=1, USB_STATE_POWERED=2, USB_STATE_DEFAULT=3,
    USB_STATE_ADDRESS=4, USB_STATE_CONFIGURED=5} usb_state=0;

//--BDendST has their PIDs upshifed 2
#define USB_PIC_PID_IN       0x24  //device to host transactions
#define USB_PIC_PID_OUT      0x04  //host to device transactions
#define USB_PIC_PID_SETUP    0x34  //host to device setup transaction

#define USTAT_IN_E0        4
#define USTAT_OUT_SETUP_E0 0

#define __USB_UIF_RESET    0x01
#define __USB_UIF_ERROR    0x02
#define __USB_UIF_ACTIVE   0x04
#define __USB_UIF_TOKEN    0x08
#define __USB_UIF_IDLE     0x10
#define __USB_UIF_STALL    0x20
#define __USB_UIF_SOF      0x40

#if USB_USE_ERROR_COUNTER
 #define STANDARD_INTS __USB_UIF_STALL|__USB_UIF_IDLE|__USB_UIF_TOKEN|__USB_UIF_ACTIVE|__USB_UIF_ERROR|__USB_UIF_RESET
#else
 #define STANDARD_INTS __USB_UIF_STALL|__USB_UIF_IDLE|__USB_UIF_TOKEN|__USB_UIF_ACTIVE|__USB_UIF_RESET
#endif

#define __USB_UCFG_UTEYE   0x80
#if defined(__USB_4550__)
 #define __USB_UCFG_UOEMON  0x40
#endif
#define __USB_UCFG_UPUEN   0x10
#if !defined(__USB_K50__)
 #define __USB_UCFG_UTRDIS  0x08
#endif
#define __USB_UCFG_FSEN    0x04

#if defined(USB_EXTERNAL_PULLUPS)
 #define __USB_UCFG_MY_UPUEN  0
#endif

#if defined(USB_INTERNAL_PULLUPS)
 #define __USB_UCFG_MY_UPUEN  __USB_UCFG_UPUEN
#endif

#if !defined(__USB_UCFG_MY_UPUEN)
 #define __USB_UCFG_MY_UPUEN  __USB_UCFG_UPUEN
#endif

#if USB_USE_FULL_SPEED
   #define __UCFG_VAL_ENABLED__ (__USB_UCFG_MY_UPUEN | __USB_UCFG_FSEN | USB_PING_PONG_MODE)
#else
   #define __UCFG_VAL_ENABLED__ (__USB_UCFG_MY_UPUEN | USB_PING_PONG_MODE);
#endif

#if defined(__USB_UCFG_UTRDIS)
 #define __UCFG_VAL_DISABLED__    __USB_UCFG_UTRDIS
#else
 #define __UCFG_VAL_DISABLED__   0
#endif


int8 __setup_0_tx_size;

//interrupt handler, specific to PIC18Fxx5x peripheral only
void usb_handle_interrupt();
void usb_isr_rst();
void usb_isr_uerr();
void usb_isr_sof(void);
void usb_isr_activity();
void usb_isr_uidle();
void usb_isr_tok_dne();
void usb_isr_stall(void);
void usb_init_ep0_setup(void);

//// BEGIN User Functions:

// see usb_hw_layer.h for more documentation
int1 usb_kbhit(int8 en)
{
   return((UEP(en)!=ENDPT_DISABLED)&&(!bit_test(EP_BDxST_O(en),7)));
}

// see usb_hw_layer.h for documentation
int1 usb_tbe(int8 en)
{
   return((UEP(en)!=ENDPT_DISABLED)&&(!bit_test(EP_BDxST_I(en),7)));
}

// see usb_hw_layer.h for documentation
void usb_detach(void)
{
   UCON = 0;  //disable USB hardware
   UIE = 0;   //disable USB interrupts
   UCFG = __UCFG_VAL_DISABLED__;
   
   // set D+/D- to inputs
  #if defined(__USB_87J50__)
   set_tris_f(get_tris_f() | 0x18);
  #elif defined(__USB_K50__)
   set_tris_a(get_tris_a() | 0x3);
  #else
   set_tris_c(get_tris_c() | 0x30);
  #endif
  
   usb_state = USB_STATE_DETACHED;
   
   usb_token_reset();              //clear the chapter9 stack
   //__usb_kbhit_status=0;
}

// see usb_hw_layer.h for documentation
void usb_attach(void) 
{
   usb_token_reset();
   UCON = 0;
   UCFG = __UCFG_VAL_ENABLED__;
   UIE = 0;                                // Mask all USB interrupts
   UCON_USBEN = 1;                     // Enable module & attach to bus
   usb_state = USB_STATE_ATTACHED;      // Defined in usbmmap.c & .h
}

// see usb_hw_layer.h for documentation
void usb_init_cs(void)
{
   usb_detach();
}

// see usb_hw_layer.h for documentation
void usb_task(void) 
{
   if (usb_attached()) 
   {
      if (UCON_USBEN==0) 
      {
         usb_attach();
      }
   }
   else 
   {
      if (UCON_USBEN==1)  
      {
         usb_detach();
      }
   }

   if ((usb_state == USB_STATE_ATTACHED)&&(!UCON_SE0)) 
   {
      UIR=0;
      UIE=0;
      //enable_interrupts(INT_USB);
      //enable_interrupts(GLOBAL);
      UIE=__USB_UIF_IDLE | __USB_UIF_RESET;  //enable IDLE and RESET USB ISR
      usb_state=USB_STATE_POWERED;
   }
}

// see usb_hw_layer.h for documentation
void usb_init(void) 
{
   usb_init_cs();

   do 
   {
      usb_task();
   } while (usb_state != USB_STATE_POWERED);
}


// see pic18_usb.h for documentation
int1 usb_flush_in(int8 endpoint, int16 len, USB_DTS_BIT tgl) 
{
   int8 i;

   if (usb_tbe(endpoint)) 
   {
      EP_BDxCNT_I(endpoint)=len;

     #if USB_IGNORE_TX_DTS
      i=0x80;
     #else
      if (tgl == USB_DTS_TOGGLE) 
      {
         i = EP_BDxST_I(endpoint);
         if (bit_test(i,6))
            tgl = USB_DTS_DATA0;  //was DATA1, goto DATA0
         else
            tgl = USB_DTS_DATA1;  //was DATA0, goto DATA1
      }
      else if (tgl == USB_DTS_USERX) 
      {
         i = EP_BDxST_O(endpoint);
         if (bit_test(i,6))
            tgl = USB_DTS_DATA1;
         else
            tgl = USB_DTS_DATA0;
      }
      if (tgl == USB_DTS_DATA1) 
         i=0xC8;  //DATA1, UOWN
      else //if (tgl == USB_DTS_DATA0) 
         i=0x88; //DATA0, UOWN
     #endif

      //set BC8 and BC9
      if (bit_test(len,8)) {bit_set(i,0);}
      if (bit_test(len,9)) {bit_set(i,1);}

      EP_BDxST_I(endpoint) = i;//save changes
      
      //putc('!');
      
      return(1);
   }
   return(0);
}

// see usb_hw_layer.h for documentation
int1 usb_put_packet(int8 endpoint, int8 * ptr, int16 len, USB_DTS_BIT tgl) 
{
   int8 * buff_add;    

   if (usb_tbe(endpoint)) 
   {
      buff_add = EP_BDxADR_I(endpoint);
      memcpy(buff_add, ptr, len);     
      
      return(usb_flush_in(endpoint, len, tgl));
   }

   return(0);
}

// see pic18_usb.h for documentation
void usb_flush_out(int8 endpoint, USB_DTS_BIT tgl) 
{
   int8 i;
   int16 len;

  #if USB_IGNORE_RX_DTS
   if (tgl == USB_DTS_STALL) 
   {
      EP_BDxCNT_O(endpoint) = 0x84;
      EP_BDxST_I(endpoint) = 0x84;
      return;
   }
   else
      i=0x80;
  #else
   i = EP_BDxST_O(endpoint);
   if (tgl == USB_DTS_TOGGLE) 
   {
      if (bit_test(i,6))
         tgl = USB_DTS_DATA0;  //was DATA1, goto DATA0
      else
         tgl = USB_DTS_DATA1;  //was DATA0, goto DATA1
   }
   if (tgl == USB_DTS_STALL) 
   {
      i = 0x84;
      EP_BDxST_I(endpoint) = 0x84; //stall both in and out endpoints
   }
   else if (tgl == USB_DTS_DATA1)
      i = 0xC8;  //DATA1, UOWN
   else //if (tgl == USB_DTS_DATA0) 
      i = 0x88; //DATA0, UOWN
  #endif

   //bit_clear(__usb_kbhit_status,endpoint);

   len = usb_ep_rx_size[endpoint];
   EP_BDxCNT_O(endpoint) = len;
   if (bit_test(len,8)) {bit_set(i,0);}
   if (bit_test(len,9)) {bit_set(i,1);}

   EP_BDxST_O(endpoint) = i;
}

// see pic18_usb.h for documentation
int16 usb_rx_packet_size(int8 endpoint) 
{
   return(EP_BDxCNT_O(endpoint));
}

/// END User Functions


/// BEGIN Hardware layer functions required by USB.

/*****************************************************************************
/* usb_get_packet_buffer(endpoint, *ptr, max)
/*
/* Input: endpoint - endpoint to get data from
/*        ptr - where to save data to local PIC RAM
/*        max - max amount of data to receive from buffer
/*
/* Output: the amount of data taken from the buffer.
/*
/* Summary: Gets a packet of data from the USB buffer and puts into local PIC 
/*          RAM.
/*          Does not mark the endpoint as ready for more data.  Once you are
/*          done with data, call usb_flush_out() to mark the endpoint ready
/*          to receive more data.
/*
/*****************************************************************************/
static int16 usb_get_packet_buffer(int8 endpoint, int8 *ptr, int16 max) 
{
   int8 * al;
   int8 st;
   int16 i;

   al = EP_BDxADR_O(endpoint);
   i = EP_BDxCNT_O(endpoint);
   st = EP_BDxST_O(endpoint);

   //read BC8 and BC9
   if (bit_test(st,0)) {bit_set(i,8);}
   if (bit_test(st,1)) {bit_set(i,9);}

   if (i < max) {max = i;}
   
   memcpy(ptr, al ,max);

   return(max);
}

// see usb_hw_layer.h for documentation
unsigned int16 usb_get_packet(int8 endpoint, int8 * ptr, unsigned int16 max)
{
   max = usb_get_packet_buffer(endpoint, ptr, max);
   usb_flush_out(endpoint, USB_DTS_TOGGLE);

   return(max);
}

// see usb_hw_layer.h for documentation
void usb_stall_ep(int8 endpoint) 
{
   int1 direction;
   
   direction = bit_test(endpoint,7);
   endpoint &= 0x7F;
   
   if (direction) 
   {
      EP_BDxST_I(endpoint) = 0x84;
   }
   else 
   {
      EP_BDxST_O(endpoint) = 0x84;
   }
}

// see usb_hw_layer.h for documentation
void usb_unstall_ep(int8 endpoint) 
{
   int1 direction;
   
   direction = bit_test(endpoint,7);
   endpoint &= 0x7F;
   
   if (direction) 
   {
     #if USB_IGNORE_RX_DTS
      EP_BDxST_I(endpoint) = 0x80;
     #else
      EP_BDxST_I(endpoint) = 0x88;
     #endif
   }
   else 
   {
      EP_BDxST_O(endpoint) = 0x00;
   }
}

// see usb_hw_layer.h for documentation
int1 usb_endpoint_stalled(int8 endpoint) 
{
   int1 direction;
   int8 st;
   
   direction = bit_test(endpoint,7);
   endpoint &= 0x7F;
   
   if (direction) 
   {
      st=EP_BDxST_I(endpoint);
   }
   else 
   {
      st=EP_BDxST_O(endpoint);
   }
   
   return(bit_test(st,7) && bit_test(st,2));
}

// see usb_hw_layer.h for documentation
void usb_set_address(int8 address) 
{
   UADDR = address;
   
   if (address) 
   {
      usb_state = USB_STATE_ADDRESS;
   }
   else 
   {
      usb_state = USB_STATE_POWERED;
   }
}

// see usb_hw_layer.h for documentation
void usb_set_configured(int8 config) 
{
   int8 en;
   int16 addy;
   int8 new_uep;
   int16 len;
   int8 i;
   
   /*if (config == 0)
   {
      // if config=0 then set addressed state
      usb_state = USB_STATE_ADDRESS;
      usb_disable_endpoints();
   }
   else */
   {
      // else set configed state
      usb_state = USB_STATE_CONFIGURED; 
      addy = (int16)USB_DATA_BUFFER_LOCATION+(2*USB_MAX_EP0_PACKET_LENGTH);
      for (en=1; en<USB_NUM_UEP; en++) 
      {
         // enable and config endpoints based upon user configuration
         usb_disable_endpoint(en);
         new_uep = 0;
         if (usb_ep_rx_type[en] != USB_ENABLE_DISABLED) 
         {
            new_uep = 0x04;
            len = usb_ep_rx_size[en];
            EP_BDxCNT_O(en) = len;
            EP_BDxADR_O(en) = addy;
            addy += usb_ep_rx_size[en];
           #if USB_IGNORE_RX_DTS
            i = 0x80;
           #else
            i = 0x88;
           #endif
            if (bit_test(len,8)) {bit_set(i,0);}
            if (bit_test(len,9)) {bit_set(i,1);}
            EP_BDxST_O(en) = i;
         }
         if (usb_ep_tx_type[en] != USB_ENABLE_DISABLED) 
         {
            new_uep |= 0x02;
            EP_BDxADR_I(en) = addy;
            addy += usb_ep_tx_size[en];
            EP_BDxST_I(en) = 0x40;
         }
         if (new_uep == 0x06) {new_uep = 0x0E;}
         if (usb_ep_tx_type[en] != USB_ENABLE_ISOCHRONOUS) {new_uep |= 0x10;}
         
         UEP(en) = new_uep;
      }
   }
}

// see usb_hw_layer.h for documentation
void usb_disable_endpoint(int8 en) 
{
   UEP(en) = ENDPT_DISABLED;
   
   if (usb_endpoint_is_valid(en))
   {
      EP_BDxST_O(en) = 0;   //clear state, deque if necessary      
      EP_BDxST_I(en) = 0;   //clear state, deque if necessary
   }
}

// see usb_hw_layer.h for documentation
void usb_disable_endpoints(void) 
{
   int8 i;
   
   for (i=1; i<USB_NUM_UEP; i++)
      usb_disable_endpoint(i);
      
   //__usb_kbhit_status=0;
}

/// END Hardware layer functions required by USB.C


/// BEGIN USB Interrupt Service Routine

static void usb_clear_trn(void)
{
   //UIR_TRN = 0;
   //delay_cycles(6);
   UIR &= ~(1 << BIT_TRN);
}

/*****************************************************************************
/* usb_handle_interrupt()
/*
/* Summary: Checks the interrupt, and acts upon event.  Processing finished
/*          tokens is the majority of this code, and is handled by usb.c
/*
/* NOTE: If you wish to change to a polling method (and not an interrupt 
/*       method), then you must call this function rapidly.  If there is more 
/*       than 10ms latency the PC may think the USB device is stalled and
/*       disable it.
/*       To switch to a polling method, remove the #int_usb line above this 
/*       fuction.  Also, goto usb_init() and remove the code that enables the 
/*       USB interrupt.
/******************************************************************************/
//#int_usb NOCLEAR
void usb_isr() 
{
   int8 TRNAttempts;
   
   clear_interrupt(INT_USB);
   
   if (usb_state == USB_STATE_DETACHED) return;   //should never happen, though
   if (UIR) 
   {
      //activity detected.  (only enable after sleep)
      if (UIR_ACTV && UIE_ACTV) {usb_isr_activity();}

      if (UCON_SUSPND) return;

      if (UIR_STALL && UIE_STALL) {usb_isr_stall();}        //a stall handshake was sent

      if (UIR_UERR && UIE_UERR) {usb_isr_uerr();}          //error has been detected

      if (UIR_URST && UIE_URST) {usb_isr_rst();}        //usb reset has been detected

      if (UIR_IDLE && UIE_IDLE) {usb_isr_uidle();}        //idle time, we can go to sleep
      
      if (UIR_SOF && UIE_SOF) {usb_isr_sof();}

      TRNAttempts = 0;
      do
      {
         if (UIR_TRN && UIE_TRN) 
         {
            USTATCopy = U1STAT;
            usb_clear_trn();
            usb_isr_tok_dne();
         }
         else
            break;
      } while (TRNAttempts++ < 4);
   }
}

// SOF interrupt not handled.  user must add this depending on application
void usb_isr_sof(void) 
{
   UIR &= ~(1 << BIT_SOF);
}

/*****************************************************************************
/* usb_isr_rst()
/*
/* Summary: The host (computer) sent us a RESET command.  Reset USB device
/*          and token handler code to initial state.
/*
/******************************************************************************/
void usb_isr_rst(void) 
{
   UEIR = 0;
   UIR = 0;
   UEIE = 0x9F;
   UIE = STANDARD_INTS & ~__USB_UIF_ACTIVE;

   UADDR = 0;

   usb_disable_endpoints();
   
   usb_token_reset();

   UEP(0) = ENDPT_CONTROL | 0x10;

   while (UIR_TRN) 
   {
      usb_clear_trn();
   }

   UCON_PKTDIS = 0; //SIE token and packet processing enabled

   usb_init_ep0_setup();

   usb_state = USB_STATE_DEFAULT; //put usb mcu into default state
}

/*****************************************************************************
/* usb_init_ep0_setup()
/*
/* Summary: Configure EP0 to receive setup packets
/*
/*****************************************************************************/
void usb_init_ep0_setup(void) 
{
    EP_BDxCNT_O(0) = USB_MAX_EP0_PACKET_LENGTH;
    EP_BDxADR_O(0) = USB_DATA_BUFFER_LOCATION;
   #if USB_IGNORE_RX_DTS
    EP_BDxST_O(0) = 0x80; //give control to SIE, data toggle synch off
   #else
    EP_BDxST_O(0) = 0x88; //give control to SIE, DATA0, data toggle synch on
   #endif

    EP_BDxST_I(0) = 0;
    EP_BDxADR_I(0) = USB_DATA_BUFFER_LOCATION + (int16)USB_MAX_EP0_PACKET_LENGTH;
}

/*******************************************************************************
/* usb_isr_uerr()
/*
/* Summary: The USB peripheral had an error.  If user specified, error counter
/*          will incerement.  If having problems check the status of these 8 bytes.
/*
/* NOTE: This code is not enabled by default.
/********************************************************************************/
void usb_isr_uerr(void)
{
  #if USB_USE_ERROR_COUNTER
   int ints;
  #endif

  #if USB_USE_ERROR_COUNTER
   ints=UEIR & UEIE; //mask off the flags with the ones that are enabled

   if ( bit_test(ints,0) ) 
   { 
      //increment pid_error counter
      ERROR_COUNTER[0]++;
   }

   if ( bit_test(ints,1) ) 
   {  
      //increment crc5 error counter
      ERROR_COUNTER[1]++;
   }

   if ( bit_test(ints,2) ) 
   {
      //increment crc16 error counter
      ERROR_COUNTER[2]++;
   }

   if ( bit_test(ints,3) ) 
   {  
      //increment dfn8 error counter
      ERROR_COUNTER[3]++;
   }

   if ( bit_test(ints,4) ) 
   {  
      //increment bto error counter
      ERROR_COUNTER[4]++;
   }

   if ( bit_test(ints,7) ) 
   { 
      //increment bts error counter
      ERROR_COUNTER[5]++;
   }
  #endif

   UEIR = 0;
   
   //UIR_UERR = 0;
   UIR &= ~(1 << BIT_UERR);
}

/*****************************************************************************
/* usb_isr_uidle()
/*
/* Summary: USB peripheral detected IDLE.  Put the USB peripheral to sleep.
/*
/*****************************************************************************/
void usb_isr_uidle(void)
{
   UIE_ACTV = 1;   //enable activity interrupt flag. (we are now suspended until we get an activity interrupt. nice)
   
   //UIR_IDLE = 0; //clear idle interrupt flag
   UIR &= ~(1 << BIT_IDLE);
   
   UCON_SUSPND = 1; //set suspend. we are now suspended
}


/******************************************************************************
/* usb_isr_activity()
/*
/* Summary: USB peripheral detected activity on the USB device.  Wake-up the USB
/*          peripheral.
/*
/*****************************************************************************/
void usb_isr_activity(void)
{
   UCON_SUSPND = 0; //turn off low power suspending
   UIE_ACTV = 0; //clear activity interupt enabling
   
   while(UIR_ACTV)
   {
      UIR &= ~(1 << BIT_ACTV);
   }
}

/******************************************************************************
/* usb_isr_stall()
/*
/* Summary: Stall handshake detected.
/*
/*****************************************************************************/
void usb_isr_stall(void) 
{
   if (bit_test(UEP(0),0)) 
   {
      usb_init_ep0_setup();
      bit_clear(UEP(0), 0);
   }

   UIR &= ~(1 << BIT_STALL);
}

// see usb_hw_layer.h for documentation
void usb_request_send_response(unsigned int8 len) {__setup_0_tx_size = len;}
void usb_request_get_data(void)  {__setup_0_tx_size = 0xFE;}
void usb_request_stall(void)  {__setup_0_tx_size = 0xFF;}

/*****************************************************************************
/* usb_isr_tok_dne()
/*
/* Summary: A Token (IN/OUT/SETUP) has been received by the USB peripheral.
/*          If a setup token on EP0 was received, run the chapter 9 code and
/*          handle the request.
/*          If an IN token on EP0 was received, continue transmitting any
/*          unfinished requests that may take more than one packet to transmit
/*          (if necessary).
/*          If an OUT token on any other EP was received, mark that EP as ready
/*          for a usb_get_packet().
/*          Does not handle any IN or OUT tokens on EP0.
/*
/*****************************************************************************/
void usb_isr_tok_dne(void) 
{
   int8 en;

   en = USTATCopy>>3;

   if (USTATCopy == USTAT_OUT_SETUP_E0) 
   {
      //new out or setup token in the buffer
      int8 pidKey;
      
      pidKey = EP_BDxST_O(0) & 0x3C;  //save PID
      
      EP_BDxST_O(0) &= 0x43;  //clear pid, prevent bdstal/pid confusion
      
      if (pidKey == USB_PIC_PID_SETUP) 
      {
         if ((EP_BDxST_I(0) & 0x80) != 0x00)
            EP_BDxST_I(0)=0;   // return the in buffer to us (dequeue any pending requests)

         usb_isr_tok_setup_dne();

         UCON_PKTDIS=0;       // UCON,PKT_DIS ; Assuming there is nothing to dequeue, clear the packet disable bit

         //if setup_0_tx_size==0xFF - stall ep0 (unhandled request) (see usb_request_stall())
         //if setup_0_tx_size==0xFE - get EP0OUT ready for a data packet, leave EP0IN alone (see usb_request_get_data())
         //else setup_0_tx_size=size of response, get EP0OUT ready for a setup packet, mark EPOIN ready for transmit (see usb_request_send_response())
         if (__setup_0_tx_size == 0xFF)
            usb_flush_out(0, USB_DTS_STALL);
         else 
         {
            usb_flush_out(0, USB_DTS_TOGGLE);
            if (__setup_0_tx_size != 0xFE)
               usb_flush_in(0 ,__setup_0_tx_size, USB_DTS_USERX);
         }
      }
      else if (pidKey == USB_PIC_PID_OUT) 
      {
         usb_isr_tok_out_dne(0);
         usb_flush_out(0, USB_DTS_TOGGLE);
         if ((__setup_0_tx_size!=0xFE) && (__setup_0_tx_size!=0xFF))
         {
            usb_flush_in(0,__setup_0_tx_size,USB_DTS_DATA1);   //send response (usually a 0len)
         }
      }
   }
   else if (USTATCopy == USTAT_IN_E0) 
   {   
      //pic -> host transfer completed
      EP_BDxST_I(0) = EP_BDxST_I(0) & 0x43;   //clear up any BDSTAL confusion
      __setup_0_tx_size = 0xFF;
      usb_isr_tok_in_dne(0);
      if (__setup_0_tx_size!=0xFF)
         usb_flush_in(0, __setup_0_tx_size, USB_DTS_TOGGLE);
      else
      {
         //usb_init_ep0_setup(); //REMOVED JUN/9/2009
      }  
   }
   else 
   {
      if (!bit_test(USTATCopy, 2)) 
      {
         EP_BDxST_O(en) = EP_BDxST_O(en) & 0x43;   //clear up any BDSTAL confusion
         usb_isr_tok_out_dne(en);
      }
      else 
      {
         EP_BDxST_I(en) = EP_BDxST_I(en) & 0x43;   //clear up any BDSTAL confusion
         usb_isr_tok_in_dne(en);
      }
   }
}
/// END USB Interrupt Service Routine
#ENDIF
