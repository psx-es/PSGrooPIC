CCS_COMPILER	= ccsc
CCS_SOURCE		= main.c
CCS_FLAGS_NBL	= +FH +Y9 -L -A -E -M -P -J -D
CCS_FLAGS_WBL	= $(CCS_FLAGS_NBL) +GWBOOTLOADER="true"
CCS_FLAGS_DEV	= +GDEV_PAYLOAD="true"
ZIP				= zip
CLEAN_FILES		= *.err *.esym *.hex *.zip

GITHEAD = $(shell cd PL3 && git rev-parse HEAD && cd ..)

B2HTARGET = $(CURDIR)/tools/bin2header

all:
		#Clean files.
		rm -f $(CLEAN_FILES)

		#Make bin2header.
		$(MAKE) -C tools

		#Make PL3.
		$(MAKE) -C PL3

		#HEX with bootloader.
		$(CCS_COMPILER) $(CCS_FLAGS_WBL) +GFW301="true" $(CCS_SOURCE)
		$(CCS_COMPILER) $(CCS_FLAGS_WBL) +GFW310="true" $(CCS_SOURCE)
		$(CCS_COMPILER) $(CCS_FLAGS_WBL) +GFW315="true" $(CCS_SOURCE)
		$(CCS_COMPILER) $(CCS_FLAGS_WBL) +GFW341="true" $(CCS_SOURCE)

		#HEX without bootloader.
		$(CCS_COMPILER) $(CCS_FLAGS_NBL) +GFW301="true" $(CCS_SOURCE)
		$(CCS_COMPILER) $(CCS_FLAGS_NBL) +GFW310="true" $(CCS_SOURCE)
		$(CCS_COMPILER) $(CCS_FLAGS_NBL) +GFW315="true" $(CCS_SOURCE)
		$(CCS_COMPILER) $(CCS_FLAGS_NBL) +GFW341="true" $(CCS_SOURCE)

		#Zip all HEX.
		$(ZIP) "PSGrooPIC_PL3_$(GITHEAD)" *.hex

dev:
		#Clean files.
		rm -f $(CLEAN_FILES)

		#HEX with bootloader.
		$(CCS_COMPILER) $(CCS_FLAGS_WBL) $(CCS_FLAGS_DEV) +GFW301="true" $(CCS_SOURCE)
		$(CCS_COMPILER) $(CCS_FLAGS_WBL) $(CCS_FLAGS_DEV) +GFW310="true" $(CCS_SOURCE)
		$(CCS_COMPILER) $(CCS_FLAGS_WBL) $(CCS_FLAGS_DEV) +GFW315="true" $(CCS_SOURCE)
		$(CCS_COMPILER) $(CCS_FLAGS_WBL) $(CCS_FLAGS_DEV) +GFW341="true" $(CCS_SOURCE)

		#HEX without bootloader.
		$(CCS_COMPILER) $(CCS_FLAGS_NBL) $(CCS_FLAGS_DEV) +GFW301="true" $(CCS_SOURCE)
		$(CCS_COMPILER) $(CCS_FLAGS_NBL) $(CCS_FLAGS_DEV) +GFW310="true" $(CCS_SOURCE)
		$(CCS_COMPILER) $(CCS_FLAGS_NBL) $(CCS_FLAGS_DEV) +GFW315="true" $(CCS_SOURCE)
		$(CCS_COMPILER) $(CCS_FLAGS_NBL) $(CCS_FLAGS_DEV) +GFW341="true" $(CCS_SOURCE)

		#Zip all HEX.
		$(ZIP) "PSGrooPIC_PL3_DEV_$(GITHEAD)" *.hex

clean: 
		#Clean files.
		rm -f $(CLEAN_FILES)

		#Remove compilations.
		$(MAKE) -C PL3/ clean
		$(MAKE) -C tools/ clean