CCS_COMPILER			= ccsc
CCS_SOURCE				= main.c
CCS_FLAGS_NBL			= +FH +Y9 -L -A -E -M -P -J -D
CCS_FLAGS_WBLHID		= $(CCS_FLAGS_NBL) +GWBOOTLOADERHID="true"
CCS_FLAGS_WBLMCHP		= $(CCS_FLAGS_NBL) +GWBOOTLOADERMCHP="true"
CCS_FLAGS_LEDS			= +GLEDR1="PIN_B4" +GLEDR2="PIN_B1" +GLEDR3="PIN_C0" +GLEDG1="PIN_B5" +GLEDG2="PIN_C1"
ZIP						= zip -r
BUILD_DIR				= build
PAYLOAD_DIR				= PL3
CLEAN_FILES				= *.err *.esym *.cod *.sym *.hex *.zip $(PAYLOAD_DIR)/*_pic_*.h build

SUPPORTED_FIRMWARES_PIC	= 3.41 3.40 3.30 3.21 3.15 3.10 3.01 2.76
FIRMWARES_PIC1			= $(SUPPORTED_FIRMWARES_PIC:2.%=2_%)
FIRMWARES_PIC2			= $(FIRMWARES_PIC1:3.%=3_%)
FIRMWARES_PIC3			=	$(foreach fw,$(FIRMWARES_PIC2), \
							$(fw))

PAYLOADS_PIC	=	default_payload \
					payload_dev \
					payload_no_unauth_syscall

PAYLOADS_PIC_CAPS	=	DEFAULT_PAYLOAD \
						PAYLOAD_DEV \
						PAYLOAD_NO_UNAUTH_SYSCALL

BOOTLOADER_BUILDS	=	nBTL \
						wBTL_HID \
						wBTL_MCHP

VERSION = $(shell cd $(PAYLOAD_DIR) && git rev-parse HEAD && cd ..)

B2HTARGET_PIC = $(CURDIR)/tools/bin2header

all: clean payloads psgroopic

psgroopic:
		#HEX with HID Bootloader.
		$(foreach fw_pic, $(FIRMWARES_PIC3), $(foreach pl_pic, $(PAYLOADS_PIC), ( echo "HID Bootloader -> Firmware: $(fw_pic) | Payload: $(pl_pic)"; $(CCS_COMPILER) $(CCS_FLAGS_WBLHID) $(CCS_FLAGS_LEDS) +GFW$(fw_pic)="true" +GPAYLOAD="$(pl_pic)" +GPAYLOAD_DIR=$(PAYLOAD_DIR) $(CCS_SOURCE)); ))

		#HEX with MCHP Bootloader.
		$(foreach fw_pic, $(FIRMWARES_PIC3), $(foreach pl_pic, $(PAYLOADS_PIC), ( echo "MCHP Bootloader -> Firmware: $(fw_pic) | Payload: $(pl_pic)"; $(CCS_COMPILER) $(CCS_FLAGS_WBLMCHP) $(CCS_FLAGS_LEDS) +GFW$(fw_pic)="true" +GPAYLOAD="$(pl_pic)" +GPAYLOAD_DIR=$(PAYLOAD_DIR) $(CCS_SOURCE)); ))

		#HEX without Bootloader.
		$(foreach fw_pic, $(FIRMWARES_PIC3), $(foreach pl_pic, $(PAYLOADS_PIC), ( echo "No Bootloader -> Firmware: $(fw_pic) | Payload: $(pl_pic)"; $(CCS_COMPILER) $(CCS_FLAGS_NBL) $(CCS_FLAGS_LEDS) +GFW$(fw_pic)="true" +GPAYLOAD="$(pl_pic)" +GPAYLOAD_DIR=$(PAYLOAD_DIR) $(CCS_SOURCE)); ))

		#Create build structure.
		mkdir $(BUILD_DIR);
		$(foreach pl_pic, $(PAYLOADS_PIC), mkdir $(BUILD_DIR)/$(pl_pic); )
		$(foreach bl_pic, $(BOOTLOADER_BUILDS), $(foreach pl_pic, $(PAYLOADS_PIC), mkdir $(BUILD_DIR)/$(pl_pic)/$(bl_pic); ))

		#Fix MCHP Bootloader
		sed -i '1i :020000040000FA..' PSGrooPIC_*_wBTL_MCHP.hex

		#Move each payload to its directory.
		$(foreach bl_pic, $(BOOTLOADER_BUILDS), $(foreach pl_pic, $(PAYLOADS_PIC_CAPS), mv *_$(pl_pic)_*_$(bl_pic).hex $(BUILD_DIR)/$(pl_pic)/$(bl_pic); ))

		#Zip all HEX.
		cd $(BUILD_DIR) && $(ZIP) "PSGrooPIC_$(PAYLOAD_DIR)_$(VERSION)" *
		mv build/PSGrooPIC_$(PAYLOAD_DIR)_$(VERSION).zip ./

payloads:
		#Make bin2header.
		$(MAKE) -C tools

		#Make Payload.
		$(MAKE) -C $(PAYLOAD_DIR)

		#Make custom Payloads.
		$(foreach fw_pic, $(FIRMWARES_PIC3), $(foreach pl_pic, $(PAYLOADS_PIC), ($(B2HTARGET_PIC) $(PAYLOAD_DIR)/$(pl_pic)_$(fw_pic).bin $(PAYLOAD_DIR)/$(pl_pic)_pic_$(fw_pic).h $(pl_pic)_$(fw_pic)); ))

clean: 
		#Clean files.
		rm -f -r $(CLEAN_FILES)

		#Remove compilations.
		$(MAKE) -C $(PAYLOAD_DIR)/ clean
		$(MAKE) -C tools/ clean