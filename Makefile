CCS_COMPILER			= ccsc
CCS_SOURCE				= main.c
CCS_FLAGS_NBL			= +FH +Y9 -L -A -E -M -P -J -D
CCS_FLAGS_WBL			= $(CCS_FLAGS_NBL) +GWBOOTLOADER="true"
CCS_FLAGS_LEDS			= +GLEDR="PIN_B4" +GLEDG="PIN_B5"
ZIP						= zip -r
BUILD_DIR				= build
PAYLOAD_DIR				= PL3
CLEAN_FILES				= *.err *.esym *.cod *.sym *.hex *.zip PL3/*_pic_*.h build

SUPPORTED_FIRMWARES_PIC	= 3.41 3.40 3.21 3.15 3.10 3.01 2.76
FIRMWARES_PIC1			= $(SUPPORTED_FIRMWARES_PIC:2.%=2_%)
FIRMWARES_PIC2			= $(FIRMWARES_PIC1:3.%=3_%)
FIRMWARES_PIC3			=	$(foreach fw,$(FIRMWARES_PIC2), \
							$(fw))

PAYLOADS_PIC	=	default_payload \
					payload_dev \
					payload_no_unauth_syscall

GITHEAD = $(shell cd $(PAYLOAD_DIR) && git rev-parse HEAD && cd ..)

B2HTARGET_PIC = $(CURDIR)/tools/bin2header

all:
		#Remove existing builds.
		rm -f -r build

		#Make bin2header.
		$(MAKE) -C tools

		#Make PL3.
		$(MAKE) -C $(PAYLOAD_DIR)

		#Make custom Payloads.
		$(foreach fw_pic, $(FIRMWARES_PIC3), $(foreach pl_pic, $(PAYLOADS_PIC), ($(B2HTARGET_PIC) $(PAYLOAD_DIR)/$(pl_pic)_$(fw_pic).bin $(PAYLOAD_DIR)/$(pl_pic)_pic_$(fw_pic).h $(pl_pic)_$(fw_pic)); ))

		#HEX with bootloader.
		$(foreach fw_pic, $(FIRMWARES_PIC3), $(foreach pl_pic, $(PAYLOADS_PIC), ($(CCS_COMPILER) $(CCS_FLAGS_WBL) $(CCS_FLAGS_LEDS) +GFW$(fw_pic)="true" +GPAYLOAD="$(pl_pic)" $(CCS_SOURCE)); ))

		#HEX without bootloader.
		$(foreach fw_pic, $(FIRMWARES_PIC3), $(foreach pl_pic, $(PAYLOADS_PIC), ($(CCS_COMPILER) $(CCS_FLAGS_NBL) $(CCS_FLAGS_LEDS) +GFW$(fw_pic)="true" +GPAYLOAD="$(pl_pic)" $(CCS_SOURCE)); ))

		#Create build structure.
		mkdir $(BUILD_DIR)
			mkdir $(BUILD_DIR)/DEF
				mkdir $(BUILD_DIR)/DEF/wBTL
				mkdir $(BUILD_DIR)/DEF/nBTL
			mkdir $(BUILD_DIR)/DEV
				mkdir $(BUILD_DIR)/DEV/wBTL
				mkdir $(BUILD_DIR)/DEV/nBTL
			mkdir $(BUILD_DIR)/NUS
				mkdir $(BUILD_DIR)/NUS/wBTL
				mkdir $(BUILD_DIR)/NUS/nBTL

		#Move each payload to its directory.
		mv *_DEFAULT_PAYLOAD_*_wBTL.hex $(BUILD_DIR)/DEF/wBTL
		mv *_DEFAULT_PAYLOAD_*_nBTL.hex $(BUILD_DIR)/DEF/nBTL
		mv *_PAYLOAD_DEV_*_wBTL.hex $(BUILD_DIR)/DEV/wBTL
		mv *_PAYLOAD_DEV_*_nBTL.hex $(BUILD_DIR)/DEV/nBTL
		mv *_PAYLOAD_NO_UNAUTH_SYSCALL_*_wBTL.hex $(BUILD_DIR)/NUS/wBTL
		mv *_PAYLOAD_NO_UNAUTH_SYSCALL_*_nBTL.hex $(BUILD_DIR)/NUS/nBTL

		#Zip all HEX.
		cd $(BUILD_DIR) && $(ZIP) "PSGrooPIC_PL3_$(GITHEAD)" *
		mv build/PSGrooPIC_PL3_*.zip ./

clean: 
		#Clean files.
		rm -f -r $(CLEAN_FILES)

		#Remove compilations.
		$(MAKE) -C PL3/ clean
		$(MAKE) -C tools/ clean