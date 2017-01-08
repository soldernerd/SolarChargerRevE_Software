#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Include project Makefile
ifeq "${IGNORE_LOCAL}" "TRUE"
# do not include local makefile. User is passing all local related variables already
else
include Makefile
# Include makefile containing local settings
ifeq "$(wildcard nbproject/Makefile-local-default.mk)" "nbproject/Makefile-local-default.mk"
include nbproject/Makefile-local-default.mk
endif
endif

# Environment
MKDIR=gnumkdir -p
RM=rm -f 
MV=mv 
CP=cp 

# Macros
CND_CONF=default
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
IMAGE_TYPE=debug
OUTPUT_SUFFIX=elf
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/PIC18_USBTest.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=hex
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/PIC18_USBTest.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Source Files Quoted if spaced
SOURCEFILES_QUOTED_IF_SPACED=C:/microchip/mla/v2016_11_07/bsp/pic18f46j50_pim/buttons.c C:/microchip/mla/v2016_11_07/bsp/pic18f46j50_pim/leds.c C:/microchip/mla/v2016_11_07/framework/usb/src/usb_device.c C:/microchip/mla/v2016_11_07/framework/usb/src/usb_device_hid.c C:/microchip/mla/v2016_11_07/framework/usb/src/usb_device_msd.c "Z:/My Documents/EmbeddedSystems/MPLAB/PIC18_USBTest/usb_descriptors.c" "Z:/My Documents/EmbeddedSystems/MPLAB/PIC18_USBTest/usb_events.c" main.c system.c app_device_custom_hid.c app_device_msd.c app_led_usb_status.c files.c C:/microchip/mla/v2016_11_07/framework/fileio/drivers/internal_flash/internal_flash.c os.c i2c.c ui.c display.c rtcc.c buck.c adc.c flash.c

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/_ext/2104742252/buttons.p1 ${OBJECTDIR}/_ext/2104742252/leds.p1 ${OBJECTDIR}/_ext/1821838623/usb_device.p1 ${OBJECTDIR}/_ext/1821838623/usb_device_hid.p1 ${OBJECTDIR}/_ext/1821838623/usb_device_msd.p1 ${OBJECTDIR}/_ext/1746714840/usb_descriptors.p1 ${OBJECTDIR}/_ext/1746714840/usb_events.p1 ${OBJECTDIR}/main.p1 ${OBJECTDIR}/system.p1 ${OBJECTDIR}/app_device_custom_hid.p1 ${OBJECTDIR}/app_device_msd.p1 ${OBJECTDIR}/app_led_usb_status.p1 ${OBJECTDIR}/files.p1 ${OBJECTDIR}/_ext/1127373753/internal_flash.p1 ${OBJECTDIR}/os.p1 ${OBJECTDIR}/i2c.p1 ${OBJECTDIR}/ui.p1 ${OBJECTDIR}/display.p1 ${OBJECTDIR}/rtcc.p1 ${OBJECTDIR}/buck.p1 ${OBJECTDIR}/adc.p1 ${OBJECTDIR}/flash.p1
POSSIBLE_DEPFILES=${OBJECTDIR}/_ext/2104742252/buttons.p1.d ${OBJECTDIR}/_ext/2104742252/leds.p1.d ${OBJECTDIR}/_ext/1821838623/usb_device.p1.d ${OBJECTDIR}/_ext/1821838623/usb_device_hid.p1.d ${OBJECTDIR}/_ext/1821838623/usb_device_msd.p1.d ${OBJECTDIR}/_ext/1746714840/usb_descriptors.p1.d ${OBJECTDIR}/_ext/1746714840/usb_events.p1.d ${OBJECTDIR}/main.p1.d ${OBJECTDIR}/system.p1.d ${OBJECTDIR}/app_device_custom_hid.p1.d ${OBJECTDIR}/app_device_msd.p1.d ${OBJECTDIR}/app_led_usb_status.p1.d ${OBJECTDIR}/files.p1.d ${OBJECTDIR}/_ext/1127373753/internal_flash.p1.d ${OBJECTDIR}/os.p1.d ${OBJECTDIR}/i2c.p1.d ${OBJECTDIR}/ui.p1.d ${OBJECTDIR}/display.p1.d ${OBJECTDIR}/rtcc.p1.d ${OBJECTDIR}/buck.p1.d ${OBJECTDIR}/adc.p1.d ${OBJECTDIR}/flash.p1.d

# Object Files
OBJECTFILES=${OBJECTDIR}/_ext/2104742252/buttons.p1 ${OBJECTDIR}/_ext/2104742252/leds.p1 ${OBJECTDIR}/_ext/1821838623/usb_device.p1 ${OBJECTDIR}/_ext/1821838623/usb_device_hid.p1 ${OBJECTDIR}/_ext/1821838623/usb_device_msd.p1 ${OBJECTDIR}/_ext/1746714840/usb_descriptors.p1 ${OBJECTDIR}/_ext/1746714840/usb_events.p1 ${OBJECTDIR}/main.p1 ${OBJECTDIR}/system.p1 ${OBJECTDIR}/app_device_custom_hid.p1 ${OBJECTDIR}/app_device_msd.p1 ${OBJECTDIR}/app_led_usb_status.p1 ${OBJECTDIR}/files.p1 ${OBJECTDIR}/_ext/1127373753/internal_flash.p1 ${OBJECTDIR}/os.p1 ${OBJECTDIR}/i2c.p1 ${OBJECTDIR}/ui.p1 ${OBJECTDIR}/display.p1 ${OBJECTDIR}/rtcc.p1 ${OBJECTDIR}/buck.p1 ${OBJECTDIR}/adc.p1 ${OBJECTDIR}/flash.p1

# Source Files
SOURCEFILES=C:/microchip/mla/v2016_11_07/bsp/pic18f46j50_pim/buttons.c C:/microchip/mla/v2016_11_07/bsp/pic18f46j50_pim/leds.c C:/microchip/mla/v2016_11_07/framework/usb/src/usb_device.c C:/microchip/mla/v2016_11_07/framework/usb/src/usb_device_hid.c C:/microchip/mla/v2016_11_07/framework/usb/src/usb_device_msd.c Z:/My Documents/EmbeddedSystems/MPLAB/PIC18_USBTest/usb_descriptors.c Z:/My Documents/EmbeddedSystems/MPLAB/PIC18_USBTest/usb_events.c main.c system.c app_device_custom_hid.c app_device_msd.c app_led_usb_status.c files.c C:/microchip/mla/v2016_11_07/framework/fileio/drivers/internal_flash/internal_flash.c os.c i2c.c ui.c display.c rtcc.c buck.c adc.c flash.c


CFLAGS=
ASFLAGS=
LDLIBSOPTIONS=

############# Tool locations ##########################################
# If you copy a project from one host to another, the path where the  #
# compiler is installed may be different.                             #
# If you open this project with MPLAB X in the new host, this         #
# makefile will be regenerated and the paths will be corrected.       #
#######################################################################
# fixDeps replaces a bunch of sed/cat/printf statements that slow down the build
FIXDEPS=fixDeps

.build-conf:  ${BUILD_SUBPROJECTS}
ifneq ($(INFORMATION_MESSAGE), )
	@echo $(INFORMATION_MESSAGE)
endif
	${MAKE}  -f nbproject/Makefile-default.mk dist/${CND_CONF}/${IMAGE_TYPE}/PIC18_USBTest.${IMAGE_TYPE}.${OUTPUT_SUFFIX}

MP_PROCESSOR_OPTION=18F46J50
# ------------------------------------------------------------------------------------
# Rules for buildStep: compile
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/_ext/2104742252/buttons.p1: C:/microchip/mla/v2016_11_07/bsp/pic18f46j50_pim/buttons.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/2104742252" 
	@${RM} ${OBJECTDIR}/_ext/2104742252/buttons.p1.d 
	@${RM} ${OBJECTDIR}/_ext/2104742252/buttons.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  -D__DEBUG=1 --debugger=pickit3  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,+asmfile,-speed,+space,-debug --addrqual=ignore --mode=free -P -N255 -I"." -I"usb" -I"fileio" -I"C:/microchip/mla/v2016_11_07/framework/usb/inc" -I"C:/microchip/mla/v2016_11_07/framework/fileio/inc" -I"C:/microchip/mla/v2016_11_07/framework/fileio/drivers/internal_flash" -I"C:/microchip/mla/v2016_11_07/bsp/pic18f46j50_pim" --warn=-3 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,+plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -o${OBJECTDIR}/_ext/2104742252/buttons.p1  C:/microchip/mla/v2016_11_07/bsp/pic18f46j50_pim/buttons.c 
	@-${MV} ${OBJECTDIR}/_ext/2104742252/buttons.d ${OBJECTDIR}/_ext/2104742252/buttons.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/2104742252/buttons.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/2104742252/leds.p1: C:/microchip/mla/v2016_11_07/bsp/pic18f46j50_pim/leds.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/2104742252" 
	@${RM} ${OBJECTDIR}/_ext/2104742252/leds.p1.d 
	@${RM} ${OBJECTDIR}/_ext/2104742252/leds.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  -D__DEBUG=1 --debugger=pickit3  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,+asmfile,-speed,+space,-debug --addrqual=ignore --mode=free -P -N255 -I"." -I"usb" -I"fileio" -I"C:/microchip/mla/v2016_11_07/framework/usb/inc" -I"C:/microchip/mla/v2016_11_07/framework/fileio/inc" -I"C:/microchip/mla/v2016_11_07/framework/fileio/drivers/internal_flash" -I"C:/microchip/mla/v2016_11_07/bsp/pic18f46j50_pim" --warn=-3 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,+plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -o${OBJECTDIR}/_ext/2104742252/leds.p1  C:/microchip/mla/v2016_11_07/bsp/pic18f46j50_pim/leds.c 
	@-${MV} ${OBJECTDIR}/_ext/2104742252/leds.d ${OBJECTDIR}/_ext/2104742252/leds.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/2104742252/leds.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1821838623/usb_device.p1: C:/microchip/mla/v2016_11_07/framework/usb/src/usb_device.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1821838623" 
	@${RM} ${OBJECTDIR}/_ext/1821838623/usb_device.p1.d 
	@${RM} ${OBJECTDIR}/_ext/1821838623/usb_device.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  -D__DEBUG=1 --debugger=pickit3  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,+asmfile,-speed,+space,-debug --addrqual=ignore --mode=free -P -N255 -I"." -I"usb" -I"fileio" -I"C:/microchip/mla/v2016_11_07/framework/usb/inc" -I"C:/microchip/mla/v2016_11_07/framework/fileio/inc" -I"C:/microchip/mla/v2016_11_07/framework/fileio/drivers/internal_flash" -I"C:/microchip/mla/v2016_11_07/bsp/pic18f46j50_pim" --warn=-3 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,+plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -o${OBJECTDIR}/_ext/1821838623/usb_device.p1  C:/microchip/mla/v2016_11_07/framework/usb/src/usb_device.c 
	@-${MV} ${OBJECTDIR}/_ext/1821838623/usb_device.d ${OBJECTDIR}/_ext/1821838623/usb_device.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/1821838623/usb_device.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1821838623/usb_device_hid.p1: C:/microchip/mla/v2016_11_07/framework/usb/src/usb_device_hid.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1821838623" 
	@${RM} ${OBJECTDIR}/_ext/1821838623/usb_device_hid.p1.d 
	@${RM} ${OBJECTDIR}/_ext/1821838623/usb_device_hid.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  -D__DEBUG=1 --debugger=pickit3  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,+asmfile,-speed,+space,-debug --addrqual=ignore --mode=free -P -N255 -I"." -I"usb" -I"fileio" -I"C:/microchip/mla/v2016_11_07/framework/usb/inc" -I"C:/microchip/mla/v2016_11_07/framework/fileio/inc" -I"C:/microchip/mla/v2016_11_07/framework/fileio/drivers/internal_flash" -I"C:/microchip/mla/v2016_11_07/bsp/pic18f46j50_pim" --warn=-3 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,+plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -o${OBJECTDIR}/_ext/1821838623/usb_device_hid.p1  C:/microchip/mla/v2016_11_07/framework/usb/src/usb_device_hid.c 
	@-${MV} ${OBJECTDIR}/_ext/1821838623/usb_device_hid.d ${OBJECTDIR}/_ext/1821838623/usb_device_hid.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/1821838623/usb_device_hid.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1821838623/usb_device_msd.p1: C:/microchip/mla/v2016_11_07/framework/usb/src/usb_device_msd.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1821838623" 
	@${RM} ${OBJECTDIR}/_ext/1821838623/usb_device_msd.p1.d 
	@${RM} ${OBJECTDIR}/_ext/1821838623/usb_device_msd.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  -D__DEBUG=1 --debugger=pickit3  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,+asmfile,-speed,+space,-debug --addrqual=ignore --mode=free -P -N255 -I"." -I"usb" -I"fileio" -I"C:/microchip/mla/v2016_11_07/framework/usb/inc" -I"C:/microchip/mla/v2016_11_07/framework/fileio/inc" -I"C:/microchip/mla/v2016_11_07/framework/fileio/drivers/internal_flash" -I"C:/microchip/mla/v2016_11_07/bsp/pic18f46j50_pim" --warn=-3 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,+plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -o${OBJECTDIR}/_ext/1821838623/usb_device_msd.p1  C:/microchip/mla/v2016_11_07/framework/usb/src/usb_device_msd.c 
	@-${MV} ${OBJECTDIR}/_ext/1821838623/usb_device_msd.d ${OBJECTDIR}/_ext/1821838623/usb_device_msd.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/1821838623/usb_device_msd.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1746714840/usb_descriptors.p1: Z:/My\ Documents/EmbeddedSystems/MPLAB/PIC18_USBTest/usb_descriptors.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1746714840" 
	@${RM} ${OBJECTDIR}/_ext/1746714840/usb_descriptors.p1.d 
	@${RM} ${OBJECTDIR}/_ext/1746714840/usb_descriptors.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  -D__DEBUG=1 --debugger=pickit3  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,+asmfile,-speed,+space,-debug --addrqual=ignore --mode=free -P -N255 -I"." -I"usb" -I"fileio" -I"C:/microchip/mla/v2016_11_07/framework/usb/inc" -I"C:/microchip/mla/v2016_11_07/framework/fileio/inc" -I"C:/microchip/mla/v2016_11_07/framework/fileio/drivers/internal_flash" -I"C:/microchip/mla/v2016_11_07/bsp/pic18f46j50_pim" --warn=-3 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,+plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -o${OBJECTDIR}/_ext/1746714840/usb_descriptors.p1  "Z:/My Documents/EmbeddedSystems/MPLAB/PIC18_USBTest/usb_descriptors.c" 
	@-${MV} ${OBJECTDIR}/_ext/1746714840/usb_descriptors.d ${OBJECTDIR}/_ext/1746714840/usb_descriptors.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/1746714840/usb_descriptors.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1746714840/usb_events.p1: Z:/My\ Documents/EmbeddedSystems/MPLAB/PIC18_USBTest/usb_events.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1746714840" 
	@${RM} ${OBJECTDIR}/_ext/1746714840/usb_events.p1.d 
	@${RM} ${OBJECTDIR}/_ext/1746714840/usb_events.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  -D__DEBUG=1 --debugger=pickit3  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,+asmfile,-speed,+space,-debug --addrqual=ignore --mode=free -P -N255 -I"." -I"usb" -I"fileio" -I"C:/microchip/mla/v2016_11_07/framework/usb/inc" -I"C:/microchip/mla/v2016_11_07/framework/fileio/inc" -I"C:/microchip/mla/v2016_11_07/framework/fileio/drivers/internal_flash" -I"C:/microchip/mla/v2016_11_07/bsp/pic18f46j50_pim" --warn=-3 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,+plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -o${OBJECTDIR}/_ext/1746714840/usb_events.p1  "Z:/My Documents/EmbeddedSystems/MPLAB/PIC18_USBTest/usb_events.c" 
	@-${MV} ${OBJECTDIR}/_ext/1746714840/usb_events.d ${OBJECTDIR}/_ext/1746714840/usb_events.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/1746714840/usb_events.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/main.p1: main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/main.p1.d 
	@${RM} ${OBJECTDIR}/main.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  -D__DEBUG=1 --debugger=pickit3  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,+asmfile,-speed,+space,-debug --addrqual=ignore --mode=free -P -N255 -I"." -I"usb" -I"fileio" -I"C:/microchip/mla/v2016_11_07/framework/usb/inc" -I"C:/microchip/mla/v2016_11_07/framework/fileio/inc" -I"C:/microchip/mla/v2016_11_07/framework/fileio/drivers/internal_flash" -I"C:/microchip/mla/v2016_11_07/bsp/pic18f46j50_pim" --warn=-3 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,+plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -o${OBJECTDIR}/main.p1  main.c 
	@-${MV} ${OBJECTDIR}/main.d ${OBJECTDIR}/main.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/main.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/system.p1: system.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/system.p1.d 
	@${RM} ${OBJECTDIR}/system.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  -D__DEBUG=1 --debugger=pickit3  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,+asmfile,-speed,+space,-debug --addrqual=ignore --mode=free -P -N255 -I"." -I"usb" -I"fileio" -I"C:/microchip/mla/v2016_11_07/framework/usb/inc" -I"C:/microchip/mla/v2016_11_07/framework/fileio/inc" -I"C:/microchip/mla/v2016_11_07/framework/fileio/drivers/internal_flash" -I"C:/microchip/mla/v2016_11_07/bsp/pic18f46j50_pim" --warn=-3 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,+plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -o${OBJECTDIR}/system.p1  system.c 
	@-${MV} ${OBJECTDIR}/system.d ${OBJECTDIR}/system.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/system.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/app_device_custom_hid.p1: app_device_custom_hid.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/app_device_custom_hid.p1.d 
	@${RM} ${OBJECTDIR}/app_device_custom_hid.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  -D__DEBUG=1 --debugger=pickit3  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,+asmfile,-speed,+space,-debug --addrqual=ignore --mode=free -P -N255 -I"." -I"usb" -I"fileio" -I"C:/microchip/mla/v2016_11_07/framework/usb/inc" -I"C:/microchip/mla/v2016_11_07/framework/fileio/inc" -I"C:/microchip/mla/v2016_11_07/framework/fileio/drivers/internal_flash" -I"C:/microchip/mla/v2016_11_07/bsp/pic18f46j50_pim" --warn=-3 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,+plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -o${OBJECTDIR}/app_device_custom_hid.p1  app_device_custom_hid.c 
	@-${MV} ${OBJECTDIR}/app_device_custom_hid.d ${OBJECTDIR}/app_device_custom_hid.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/app_device_custom_hid.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/app_device_msd.p1: app_device_msd.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/app_device_msd.p1.d 
	@${RM} ${OBJECTDIR}/app_device_msd.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  -D__DEBUG=1 --debugger=pickit3  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,+asmfile,-speed,+space,-debug --addrqual=ignore --mode=free -P -N255 -I"." -I"usb" -I"fileio" -I"C:/microchip/mla/v2016_11_07/framework/usb/inc" -I"C:/microchip/mla/v2016_11_07/framework/fileio/inc" -I"C:/microchip/mla/v2016_11_07/framework/fileio/drivers/internal_flash" -I"C:/microchip/mla/v2016_11_07/bsp/pic18f46j50_pim" --warn=-3 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,+plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -o${OBJECTDIR}/app_device_msd.p1  app_device_msd.c 
	@-${MV} ${OBJECTDIR}/app_device_msd.d ${OBJECTDIR}/app_device_msd.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/app_device_msd.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/app_led_usb_status.p1: app_led_usb_status.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/app_led_usb_status.p1.d 
	@${RM} ${OBJECTDIR}/app_led_usb_status.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  -D__DEBUG=1 --debugger=pickit3  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,+asmfile,-speed,+space,-debug --addrqual=ignore --mode=free -P -N255 -I"." -I"usb" -I"fileio" -I"C:/microchip/mla/v2016_11_07/framework/usb/inc" -I"C:/microchip/mla/v2016_11_07/framework/fileio/inc" -I"C:/microchip/mla/v2016_11_07/framework/fileio/drivers/internal_flash" -I"C:/microchip/mla/v2016_11_07/bsp/pic18f46j50_pim" --warn=-3 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,+plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -o${OBJECTDIR}/app_led_usb_status.p1  app_led_usb_status.c 
	@-${MV} ${OBJECTDIR}/app_led_usb_status.d ${OBJECTDIR}/app_led_usb_status.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/app_led_usb_status.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/files.p1: files.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/files.p1.d 
	@${RM} ${OBJECTDIR}/files.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  -D__DEBUG=1 --debugger=pickit3  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,+asmfile,-speed,+space,-debug --addrqual=ignore --mode=free -P -N255 -I"." -I"usb" -I"fileio" -I"C:/microchip/mla/v2016_11_07/framework/usb/inc" -I"C:/microchip/mla/v2016_11_07/framework/fileio/inc" -I"C:/microchip/mla/v2016_11_07/framework/fileio/drivers/internal_flash" -I"C:/microchip/mla/v2016_11_07/bsp/pic18f46j50_pim" --warn=-3 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,+plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -o${OBJECTDIR}/files.p1  files.c 
	@-${MV} ${OBJECTDIR}/files.d ${OBJECTDIR}/files.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/files.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1127373753/internal_flash.p1: C:/microchip/mla/v2016_11_07/framework/fileio/drivers/internal_flash/internal_flash.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1127373753" 
	@${RM} ${OBJECTDIR}/_ext/1127373753/internal_flash.p1.d 
	@${RM} ${OBJECTDIR}/_ext/1127373753/internal_flash.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  -D__DEBUG=1 --debugger=pickit3  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,+asmfile,-speed,+space,-debug --addrqual=ignore --mode=free -P -N255 -I"." -I"usb" -I"fileio" -I"C:/microchip/mla/v2016_11_07/framework/usb/inc" -I"C:/microchip/mla/v2016_11_07/framework/fileio/inc" -I"C:/microchip/mla/v2016_11_07/framework/fileio/drivers/internal_flash" -I"C:/microchip/mla/v2016_11_07/bsp/pic18f46j50_pim" --warn=-3 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,+plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -o${OBJECTDIR}/_ext/1127373753/internal_flash.p1  C:/microchip/mla/v2016_11_07/framework/fileio/drivers/internal_flash/internal_flash.c 
	@-${MV} ${OBJECTDIR}/_ext/1127373753/internal_flash.d ${OBJECTDIR}/_ext/1127373753/internal_flash.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/1127373753/internal_flash.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/os.p1: os.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/os.p1.d 
	@${RM} ${OBJECTDIR}/os.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  -D__DEBUG=1 --debugger=pickit3  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,+asmfile,-speed,+space,-debug --addrqual=ignore --mode=free -P -N255 -I"." -I"usb" -I"fileio" -I"C:/microchip/mla/v2016_11_07/framework/usb/inc" -I"C:/microchip/mla/v2016_11_07/framework/fileio/inc" -I"C:/microchip/mla/v2016_11_07/framework/fileio/drivers/internal_flash" -I"C:/microchip/mla/v2016_11_07/bsp/pic18f46j50_pim" --warn=-3 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,+plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -o${OBJECTDIR}/os.p1  os.c 
	@-${MV} ${OBJECTDIR}/os.d ${OBJECTDIR}/os.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/os.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/i2c.p1: i2c.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/i2c.p1.d 
	@${RM} ${OBJECTDIR}/i2c.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  -D__DEBUG=1 --debugger=pickit3  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,+asmfile,-speed,+space,-debug --addrqual=ignore --mode=free -P -N255 -I"." -I"usb" -I"fileio" -I"C:/microchip/mla/v2016_11_07/framework/usb/inc" -I"C:/microchip/mla/v2016_11_07/framework/fileio/inc" -I"C:/microchip/mla/v2016_11_07/framework/fileio/drivers/internal_flash" -I"C:/microchip/mla/v2016_11_07/bsp/pic18f46j50_pim" --warn=-3 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,+plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -o${OBJECTDIR}/i2c.p1  i2c.c 
	@-${MV} ${OBJECTDIR}/i2c.d ${OBJECTDIR}/i2c.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/i2c.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/ui.p1: ui.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/ui.p1.d 
	@${RM} ${OBJECTDIR}/ui.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  -D__DEBUG=1 --debugger=pickit3  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,+asmfile,-speed,+space,-debug --addrqual=ignore --mode=free -P -N255 -I"." -I"usb" -I"fileio" -I"C:/microchip/mla/v2016_11_07/framework/usb/inc" -I"C:/microchip/mla/v2016_11_07/framework/fileio/inc" -I"C:/microchip/mla/v2016_11_07/framework/fileio/drivers/internal_flash" -I"C:/microchip/mla/v2016_11_07/bsp/pic18f46j50_pim" --warn=-3 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,+plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -o${OBJECTDIR}/ui.p1  ui.c 
	@-${MV} ${OBJECTDIR}/ui.d ${OBJECTDIR}/ui.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/ui.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/display.p1: display.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/display.p1.d 
	@${RM} ${OBJECTDIR}/display.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  -D__DEBUG=1 --debugger=pickit3  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,+asmfile,-speed,+space,-debug --addrqual=ignore --mode=free -P -N255 -I"." -I"usb" -I"fileio" -I"C:/microchip/mla/v2016_11_07/framework/usb/inc" -I"C:/microchip/mla/v2016_11_07/framework/fileio/inc" -I"C:/microchip/mla/v2016_11_07/framework/fileio/drivers/internal_flash" -I"C:/microchip/mla/v2016_11_07/bsp/pic18f46j50_pim" --warn=-3 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,+plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -o${OBJECTDIR}/display.p1  display.c 
	@-${MV} ${OBJECTDIR}/display.d ${OBJECTDIR}/display.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/display.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/rtcc.p1: rtcc.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/rtcc.p1.d 
	@${RM} ${OBJECTDIR}/rtcc.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  -D__DEBUG=1 --debugger=pickit3  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,+asmfile,-speed,+space,-debug --addrqual=ignore --mode=free -P -N255 -I"." -I"usb" -I"fileio" -I"C:/microchip/mla/v2016_11_07/framework/usb/inc" -I"C:/microchip/mla/v2016_11_07/framework/fileio/inc" -I"C:/microchip/mla/v2016_11_07/framework/fileio/drivers/internal_flash" -I"C:/microchip/mla/v2016_11_07/bsp/pic18f46j50_pim" --warn=-3 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,+plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -o${OBJECTDIR}/rtcc.p1  rtcc.c 
	@-${MV} ${OBJECTDIR}/rtcc.d ${OBJECTDIR}/rtcc.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/rtcc.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/buck.p1: buck.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/buck.p1.d 
	@${RM} ${OBJECTDIR}/buck.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  -D__DEBUG=1 --debugger=pickit3  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,+asmfile,-speed,+space,-debug --addrqual=ignore --mode=free -P -N255 -I"." -I"usb" -I"fileio" -I"C:/microchip/mla/v2016_11_07/framework/usb/inc" -I"C:/microchip/mla/v2016_11_07/framework/fileio/inc" -I"C:/microchip/mla/v2016_11_07/framework/fileio/drivers/internal_flash" -I"C:/microchip/mla/v2016_11_07/bsp/pic18f46j50_pim" --warn=-3 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,+plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -o${OBJECTDIR}/buck.p1  buck.c 
	@-${MV} ${OBJECTDIR}/buck.d ${OBJECTDIR}/buck.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/buck.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/adc.p1: adc.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/adc.p1.d 
	@${RM} ${OBJECTDIR}/adc.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  -D__DEBUG=1 --debugger=pickit3  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,+asmfile,-speed,+space,-debug --addrqual=ignore --mode=free -P -N255 -I"." -I"usb" -I"fileio" -I"C:/microchip/mla/v2016_11_07/framework/usb/inc" -I"C:/microchip/mla/v2016_11_07/framework/fileio/inc" -I"C:/microchip/mla/v2016_11_07/framework/fileio/drivers/internal_flash" -I"C:/microchip/mla/v2016_11_07/bsp/pic18f46j50_pim" --warn=-3 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,+plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -o${OBJECTDIR}/adc.p1  adc.c 
	@-${MV} ${OBJECTDIR}/adc.d ${OBJECTDIR}/adc.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/adc.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/flash.p1: flash.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/flash.p1.d 
	@${RM} ${OBJECTDIR}/flash.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  -D__DEBUG=1 --debugger=pickit3  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,+asmfile,-speed,+space,-debug --addrqual=ignore --mode=free -P -N255 -I"." -I"usb" -I"fileio" -I"C:/microchip/mla/v2016_11_07/framework/usb/inc" -I"C:/microchip/mla/v2016_11_07/framework/fileio/inc" -I"C:/microchip/mla/v2016_11_07/framework/fileio/drivers/internal_flash" -I"C:/microchip/mla/v2016_11_07/bsp/pic18f46j50_pim" --warn=-3 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,+plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -o${OBJECTDIR}/flash.p1  flash.c 
	@-${MV} ${OBJECTDIR}/flash.d ${OBJECTDIR}/flash.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/flash.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
else
${OBJECTDIR}/_ext/2104742252/buttons.p1: C:/microchip/mla/v2016_11_07/bsp/pic18f46j50_pim/buttons.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/2104742252" 
	@${RM} ${OBJECTDIR}/_ext/2104742252/buttons.p1.d 
	@${RM} ${OBJECTDIR}/_ext/2104742252/buttons.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,+asmfile,-speed,+space,-debug --addrqual=ignore --mode=free -P -N255 -I"." -I"usb" -I"fileio" -I"C:/microchip/mla/v2016_11_07/framework/usb/inc" -I"C:/microchip/mla/v2016_11_07/framework/fileio/inc" -I"C:/microchip/mla/v2016_11_07/framework/fileio/drivers/internal_flash" -I"C:/microchip/mla/v2016_11_07/bsp/pic18f46j50_pim" --warn=-3 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,+plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -o${OBJECTDIR}/_ext/2104742252/buttons.p1  C:/microchip/mla/v2016_11_07/bsp/pic18f46j50_pim/buttons.c 
	@-${MV} ${OBJECTDIR}/_ext/2104742252/buttons.d ${OBJECTDIR}/_ext/2104742252/buttons.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/2104742252/buttons.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/2104742252/leds.p1: C:/microchip/mla/v2016_11_07/bsp/pic18f46j50_pim/leds.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/2104742252" 
	@${RM} ${OBJECTDIR}/_ext/2104742252/leds.p1.d 
	@${RM} ${OBJECTDIR}/_ext/2104742252/leds.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,+asmfile,-speed,+space,-debug --addrqual=ignore --mode=free -P -N255 -I"." -I"usb" -I"fileio" -I"C:/microchip/mla/v2016_11_07/framework/usb/inc" -I"C:/microchip/mla/v2016_11_07/framework/fileio/inc" -I"C:/microchip/mla/v2016_11_07/framework/fileio/drivers/internal_flash" -I"C:/microchip/mla/v2016_11_07/bsp/pic18f46j50_pim" --warn=-3 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,+plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -o${OBJECTDIR}/_ext/2104742252/leds.p1  C:/microchip/mla/v2016_11_07/bsp/pic18f46j50_pim/leds.c 
	@-${MV} ${OBJECTDIR}/_ext/2104742252/leds.d ${OBJECTDIR}/_ext/2104742252/leds.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/2104742252/leds.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1821838623/usb_device.p1: C:/microchip/mla/v2016_11_07/framework/usb/src/usb_device.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1821838623" 
	@${RM} ${OBJECTDIR}/_ext/1821838623/usb_device.p1.d 
	@${RM} ${OBJECTDIR}/_ext/1821838623/usb_device.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,+asmfile,-speed,+space,-debug --addrqual=ignore --mode=free -P -N255 -I"." -I"usb" -I"fileio" -I"C:/microchip/mla/v2016_11_07/framework/usb/inc" -I"C:/microchip/mla/v2016_11_07/framework/fileio/inc" -I"C:/microchip/mla/v2016_11_07/framework/fileio/drivers/internal_flash" -I"C:/microchip/mla/v2016_11_07/bsp/pic18f46j50_pim" --warn=-3 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,+plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -o${OBJECTDIR}/_ext/1821838623/usb_device.p1  C:/microchip/mla/v2016_11_07/framework/usb/src/usb_device.c 
	@-${MV} ${OBJECTDIR}/_ext/1821838623/usb_device.d ${OBJECTDIR}/_ext/1821838623/usb_device.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/1821838623/usb_device.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1821838623/usb_device_hid.p1: C:/microchip/mla/v2016_11_07/framework/usb/src/usb_device_hid.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1821838623" 
	@${RM} ${OBJECTDIR}/_ext/1821838623/usb_device_hid.p1.d 
	@${RM} ${OBJECTDIR}/_ext/1821838623/usb_device_hid.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,+asmfile,-speed,+space,-debug --addrqual=ignore --mode=free -P -N255 -I"." -I"usb" -I"fileio" -I"C:/microchip/mla/v2016_11_07/framework/usb/inc" -I"C:/microchip/mla/v2016_11_07/framework/fileio/inc" -I"C:/microchip/mla/v2016_11_07/framework/fileio/drivers/internal_flash" -I"C:/microchip/mla/v2016_11_07/bsp/pic18f46j50_pim" --warn=-3 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,+plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -o${OBJECTDIR}/_ext/1821838623/usb_device_hid.p1  C:/microchip/mla/v2016_11_07/framework/usb/src/usb_device_hid.c 
	@-${MV} ${OBJECTDIR}/_ext/1821838623/usb_device_hid.d ${OBJECTDIR}/_ext/1821838623/usb_device_hid.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/1821838623/usb_device_hid.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1821838623/usb_device_msd.p1: C:/microchip/mla/v2016_11_07/framework/usb/src/usb_device_msd.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1821838623" 
	@${RM} ${OBJECTDIR}/_ext/1821838623/usb_device_msd.p1.d 
	@${RM} ${OBJECTDIR}/_ext/1821838623/usb_device_msd.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,+asmfile,-speed,+space,-debug --addrqual=ignore --mode=free -P -N255 -I"." -I"usb" -I"fileio" -I"C:/microchip/mla/v2016_11_07/framework/usb/inc" -I"C:/microchip/mla/v2016_11_07/framework/fileio/inc" -I"C:/microchip/mla/v2016_11_07/framework/fileio/drivers/internal_flash" -I"C:/microchip/mla/v2016_11_07/bsp/pic18f46j50_pim" --warn=-3 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,+plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -o${OBJECTDIR}/_ext/1821838623/usb_device_msd.p1  C:/microchip/mla/v2016_11_07/framework/usb/src/usb_device_msd.c 
	@-${MV} ${OBJECTDIR}/_ext/1821838623/usb_device_msd.d ${OBJECTDIR}/_ext/1821838623/usb_device_msd.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/1821838623/usb_device_msd.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1746714840/usb_descriptors.p1: Z:/My\ Documents/EmbeddedSystems/MPLAB/PIC18_USBTest/usb_descriptors.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1746714840" 
	@${RM} ${OBJECTDIR}/_ext/1746714840/usb_descriptors.p1.d 
	@${RM} ${OBJECTDIR}/_ext/1746714840/usb_descriptors.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,+asmfile,-speed,+space,-debug --addrqual=ignore --mode=free -P -N255 -I"." -I"usb" -I"fileio" -I"C:/microchip/mla/v2016_11_07/framework/usb/inc" -I"C:/microchip/mla/v2016_11_07/framework/fileio/inc" -I"C:/microchip/mla/v2016_11_07/framework/fileio/drivers/internal_flash" -I"C:/microchip/mla/v2016_11_07/bsp/pic18f46j50_pim" --warn=-3 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,+plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -o${OBJECTDIR}/_ext/1746714840/usb_descriptors.p1  "Z:/My Documents/EmbeddedSystems/MPLAB/PIC18_USBTest/usb_descriptors.c" 
	@-${MV} ${OBJECTDIR}/_ext/1746714840/usb_descriptors.d ${OBJECTDIR}/_ext/1746714840/usb_descriptors.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/1746714840/usb_descriptors.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1746714840/usb_events.p1: Z:/My\ Documents/EmbeddedSystems/MPLAB/PIC18_USBTest/usb_events.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1746714840" 
	@${RM} ${OBJECTDIR}/_ext/1746714840/usb_events.p1.d 
	@${RM} ${OBJECTDIR}/_ext/1746714840/usb_events.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,+asmfile,-speed,+space,-debug --addrqual=ignore --mode=free -P -N255 -I"." -I"usb" -I"fileio" -I"C:/microchip/mla/v2016_11_07/framework/usb/inc" -I"C:/microchip/mla/v2016_11_07/framework/fileio/inc" -I"C:/microchip/mla/v2016_11_07/framework/fileio/drivers/internal_flash" -I"C:/microchip/mla/v2016_11_07/bsp/pic18f46j50_pim" --warn=-3 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,+plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -o${OBJECTDIR}/_ext/1746714840/usb_events.p1  "Z:/My Documents/EmbeddedSystems/MPLAB/PIC18_USBTest/usb_events.c" 
	@-${MV} ${OBJECTDIR}/_ext/1746714840/usb_events.d ${OBJECTDIR}/_ext/1746714840/usb_events.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/1746714840/usb_events.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/main.p1: main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/main.p1.d 
	@${RM} ${OBJECTDIR}/main.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,+asmfile,-speed,+space,-debug --addrqual=ignore --mode=free -P -N255 -I"." -I"usb" -I"fileio" -I"C:/microchip/mla/v2016_11_07/framework/usb/inc" -I"C:/microchip/mla/v2016_11_07/framework/fileio/inc" -I"C:/microchip/mla/v2016_11_07/framework/fileio/drivers/internal_flash" -I"C:/microchip/mla/v2016_11_07/bsp/pic18f46j50_pim" --warn=-3 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,+plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -o${OBJECTDIR}/main.p1  main.c 
	@-${MV} ${OBJECTDIR}/main.d ${OBJECTDIR}/main.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/main.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/system.p1: system.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/system.p1.d 
	@${RM} ${OBJECTDIR}/system.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,+asmfile,-speed,+space,-debug --addrqual=ignore --mode=free -P -N255 -I"." -I"usb" -I"fileio" -I"C:/microchip/mla/v2016_11_07/framework/usb/inc" -I"C:/microchip/mla/v2016_11_07/framework/fileio/inc" -I"C:/microchip/mla/v2016_11_07/framework/fileio/drivers/internal_flash" -I"C:/microchip/mla/v2016_11_07/bsp/pic18f46j50_pim" --warn=-3 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,+plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -o${OBJECTDIR}/system.p1  system.c 
	@-${MV} ${OBJECTDIR}/system.d ${OBJECTDIR}/system.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/system.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/app_device_custom_hid.p1: app_device_custom_hid.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/app_device_custom_hid.p1.d 
	@${RM} ${OBJECTDIR}/app_device_custom_hid.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,+asmfile,-speed,+space,-debug --addrqual=ignore --mode=free -P -N255 -I"." -I"usb" -I"fileio" -I"C:/microchip/mla/v2016_11_07/framework/usb/inc" -I"C:/microchip/mla/v2016_11_07/framework/fileio/inc" -I"C:/microchip/mla/v2016_11_07/framework/fileio/drivers/internal_flash" -I"C:/microchip/mla/v2016_11_07/bsp/pic18f46j50_pim" --warn=-3 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,+plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -o${OBJECTDIR}/app_device_custom_hid.p1  app_device_custom_hid.c 
	@-${MV} ${OBJECTDIR}/app_device_custom_hid.d ${OBJECTDIR}/app_device_custom_hid.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/app_device_custom_hid.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/app_device_msd.p1: app_device_msd.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/app_device_msd.p1.d 
	@${RM} ${OBJECTDIR}/app_device_msd.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,+asmfile,-speed,+space,-debug --addrqual=ignore --mode=free -P -N255 -I"." -I"usb" -I"fileio" -I"C:/microchip/mla/v2016_11_07/framework/usb/inc" -I"C:/microchip/mla/v2016_11_07/framework/fileio/inc" -I"C:/microchip/mla/v2016_11_07/framework/fileio/drivers/internal_flash" -I"C:/microchip/mla/v2016_11_07/bsp/pic18f46j50_pim" --warn=-3 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,+plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -o${OBJECTDIR}/app_device_msd.p1  app_device_msd.c 
	@-${MV} ${OBJECTDIR}/app_device_msd.d ${OBJECTDIR}/app_device_msd.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/app_device_msd.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/app_led_usb_status.p1: app_led_usb_status.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/app_led_usb_status.p1.d 
	@${RM} ${OBJECTDIR}/app_led_usb_status.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,+asmfile,-speed,+space,-debug --addrqual=ignore --mode=free -P -N255 -I"." -I"usb" -I"fileio" -I"C:/microchip/mla/v2016_11_07/framework/usb/inc" -I"C:/microchip/mla/v2016_11_07/framework/fileio/inc" -I"C:/microchip/mla/v2016_11_07/framework/fileio/drivers/internal_flash" -I"C:/microchip/mla/v2016_11_07/bsp/pic18f46j50_pim" --warn=-3 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,+plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -o${OBJECTDIR}/app_led_usb_status.p1  app_led_usb_status.c 
	@-${MV} ${OBJECTDIR}/app_led_usb_status.d ${OBJECTDIR}/app_led_usb_status.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/app_led_usb_status.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/files.p1: files.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/files.p1.d 
	@${RM} ${OBJECTDIR}/files.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,+asmfile,-speed,+space,-debug --addrqual=ignore --mode=free -P -N255 -I"." -I"usb" -I"fileio" -I"C:/microchip/mla/v2016_11_07/framework/usb/inc" -I"C:/microchip/mla/v2016_11_07/framework/fileio/inc" -I"C:/microchip/mla/v2016_11_07/framework/fileio/drivers/internal_flash" -I"C:/microchip/mla/v2016_11_07/bsp/pic18f46j50_pim" --warn=-3 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,+plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -o${OBJECTDIR}/files.p1  files.c 
	@-${MV} ${OBJECTDIR}/files.d ${OBJECTDIR}/files.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/files.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1127373753/internal_flash.p1: C:/microchip/mla/v2016_11_07/framework/fileio/drivers/internal_flash/internal_flash.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1127373753" 
	@${RM} ${OBJECTDIR}/_ext/1127373753/internal_flash.p1.d 
	@${RM} ${OBJECTDIR}/_ext/1127373753/internal_flash.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,+asmfile,-speed,+space,-debug --addrqual=ignore --mode=free -P -N255 -I"." -I"usb" -I"fileio" -I"C:/microchip/mla/v2016_11_07/framework/usb/inc" -I"C:/microchip/mla/v2016_11_07/framework/fileio/inc" -I"C:/microchip/mla/v2016_11_07/framework/fileio/drivers/internal_flash" -I"C:/microchip/mla/v2016_11_07/bsp/pic18f46j50_pim" --warn=-3 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,+plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -o${OBJECTDIR}/_ext/1127373753/internal_flash.p1  C:/microchip/mla/v2016_11_07/framework/fileio/drivers/internal_flash/internal_flash.c 
	@-${MV} ${OBJECTDIR}/_ext/1127373753/internal_flash.d ${OBJECTDIR}/_ext/1127373753/internal_flash.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/1127373753/internal_flash.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/os.p1: os.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/os.p1.d 
	@${RM} ${OBJECTDIR}/os.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,+asmfile,-speed,+space,-debug --addrqual=ignore --mode=free -P -N255 -I"." -I"usb" -I"fileio" -I"C:/microchip/mla/v2016_11_07/framework/usb/inc" -I"C:/microchip/mla/v2016_11_07/framework/fileio/inc" -I"C:/microchip/mla/v2016_11_07/framework/fileio/drivers/internal_flash" -I"C:/microchip/mla/v2016_11_07/bsp/pic18f46j50_pim" --warn=-3 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,+plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -o${OBJECTDIR}/os.p1  os.c 
	@-${MV} ${OBJECTDIR}/os.d ${OBJECTDIR}/os.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/os.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/i2c.p1: i2c.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/i2c.p1.d 
	@${RM} ${OBJECTDIR}/i2c.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,+asmfile,-speed,+space,-debug --addrqual=ignore --mode=free -P -N255 -I"." -I"usb" -I"fileio" -I"C:/microchip/mla/v2016_11_07/framework/usb/inc" -I"C:/microchip/mla/v2016_11_07/framework/fileio/inc" -I"C:/microchip/mla/v2016_11_07/framework/fileio/drivers/internal_flash" -I"C:/microchip/mla/v2016_11_07/bsp/pic18f46j50_pim" --warn=-3 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,+plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -o${OBJECTDIR}/i2c.p1  i2c.c 
	@-${MV} ${OBJECTDIR}/i2c.d ${OBJECTDIR}/i2c.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/i2c.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/ui.p1: ui.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/ui.p1.d 
	@${RM} ${OBJECTDIR}/ui.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,+asmfile,-speed,+space,-debug --addrqual=ignore --mode=free -P -N255 -I"." -I"usb" -I"fileio" -I"C:/microchip/mla/v2016_11_07/framework/usb/inc" -I"C:/microchip/mla/v2016_11_07/framework/fileio/inc" -I"C:/microchip/mla/v2016_11_07/framework/fileio/drivers/internal_flash" -I"C:/microchip/mla/v2016_11_07/bsp/pic18f46j50_pim" --warn=-3 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,+plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -o${OBJECTDIR}/ui.p1  ui.c 
	@-${MV} ${OBJECTDIR}/ui.d ${OBJECTDIR}/ui.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/ui.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/display.p1: display.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/display.p1.d 
	@${RM} ${OBJECTDIR}/display.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,+asmfile,-speed,+space,-debug --addrqual=ignore --mode=free -P -N255 -I"." -I"usb" -I"fileio" -I"C:/microchip/mla/v2016_11_07/framework/usb/inc" -I"C:/microchip/mla/v2016_11_07/framework/fileio/inc" -I"C:/microchip/mla/v2016_11_07/framework/fileio/drivers/internal_flash" -I"C:/microchip/mla/v2016_11_07/bsp/pic18f46j50_pim" --warn=-3 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,+plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -o${OBJECTDIR}/display.p1  display.c 
	@-${MV} ${OBJECTDIR}/display.d ${OBJECTDIR}/display.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/display.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/rtcc.p1: rtcc.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/rtcc.p1.d 
	@${RM} ${OBJECTDIR}/rtcc.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,+asmfile,-speed,+space,-debug --addrqual=ignore --mode=free -P -N255 -I"." -I"usb" -I"fileio" -I"C:/microchip/mla/v2016_11_07/framework/usb/inc" -I"C:/microchip/mla/v2016_11_07/framework/fileio/inc" -I"C:/microchip/mla/v2016_11_07/framework/fileio/drivers/internal_flash" -I"C:/microchip/mla/v2016_11_07/bsp/pic18f46j50_pim" --warn=-3 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,+plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -o${OBJECTDIR}/rtcc.p1  rtcc.c 
	@-${MV} ${OBJECTDIR}/rtcc.d ${OBJECTDIR}/rtcc.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/rtcc.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/buck.p1: buck.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/buck.p1.d 
	@${RM} ${OBJECTDIR}/buck.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,+asmfile,-speed,+space,-debug --addrqual=ignore --mode=free -P -N255 -I"." -I"usb" -I"fileio" -I"C:/microchip/mla/v2016_11_07/framework/usb/inc" -I"C:/microchip/mla/v2016_11_07/framework/fileio/inc" -I"C:/microchip/mla/v2016_11_07/framework/fileio/drivers/internal_flash" -I"C:/microchip/mla/v2016_11_07/bsp/pic18f46j50_pim" --warn=-3 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,+plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -o${OBJECTDIR}/buck.p1  buck.c 
	@-${MV} ${OBJECTDIR}/buck.d ${OBJECTDIR}/buck.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/buck.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/adc.p1: adc.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/adc.p1.d 
	@${RM} ${OBJECTDIR}/adc.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,+asmfile,-speed,+space,-debug --addrqual=ignore --mode=free -P -N255 -I"." -I"usb" -I"fileio" -I"C:/microchip/mla/v2016_11_07/framework/usb/inc" -I"C:/microchip/mla/v2016_11_07/framework/fileio/inc" -I"C:/microchip/mla/v2016_11_07/framework/fileio/drivers/internal_flash" -I"C:/microchip/mla/v2016_11_07/bsp/pic18f46j50_pim" --warn=-3 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,+plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -o${OBJECTDIR}/adc.p1  adc.c 
	@-${MV} ${OBJECTDIR}/adc.d ${OBJECTDIR}/adc.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/adc.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/flash.p1: flash.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/flash.p1.d 
	@${RM} ${OBJECTDIR}/flash.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,+asmfile,-speed,+space,-debug --addrqual=ignore --mode=free -P -N255 -I"." -I"usb" -I"fileio" -I"C:/microchip/mla/v2016_11_07/framework/usb/inc" -I"C:/microchip/mla/v2016_11_07/framework/fileio/inc" -I"C:/microchip/mla/v2016_11_07/framework/fileio/drivers/internal_flash" -I"C:/microchip/mla/v2016_11_07/bsp/pic18f46j50_pim" --warn=-3 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,+plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -o${OBJECTDIR}/flash.p1  flash.c 
	@-${MV} ${OBJECTDIR}/flash.d ${OBJECTDIR}/flash.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/flash.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assemble
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
dist/${CND_CONF}/${IMAGE_TYPE}/PIC18_USBTest.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk    
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE) --chip=$(MP_PROCESSOR_OPTION) -G -mdist/${CND_CONF}/${IMAGE_TYPE}/PIC18_USBTest.${IMAGE_TYPE}.map  -D__DEBUG=1 --debugger=pickit3  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,+asmfile,-speed,+space,-debug --addrqual=ignore --mode=free -P -N255 -I"." -I"usb" -I"fileio" -I"C:/microchip/mla/v2016_11_07/framework/usb/inc" -I"C:/microchip/mla/v2016_11_07/framework/fileio/inc" -I"C:/microchip/mla/v2016_11_07/framework/fileio/drivers/internal_flash" -I"C:/microchip/mla/v2016_11_07/bsp/pic18f46j50_pim" --warn=-3 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,+plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"       --memorysummary dist/${CND_CONF}/${IMAGE_TYPE}/memoryfile.xml -odist/${CND_CONF}/${IMAGE_TYPE}/PIC18_USBTest.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}     
	@${RM} dist/${CND_CONF}/${IMAGE_TYPE}/PIC18_USBTest.${IMAGE_TYPE}.hex 
	
else
dist/${CND_CONF}/${IMAGE_TYPE}/PIC18_USBTest.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk   
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE) --chip=$(MP_PROCESSOR_OPTION) -G -mdist/${CND_CONF}/${IMAGE_TYPE}/PIC18_USBTest.${IMAGE_TYPE}.map  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,+asmfile,-speed,+space,-debug --addrqual=ignore --mode=free -P -N255 -I"." -I"usb" -I"fileio" -I"C:/microchip/mla/v2016_11_07/framework/usb/inc" -I"C:/microchip/mla/v2016_11_07/framework/fileio/inc" -I"C:/microchip/mla/v2016_11_07/framework/fileio/drivers/internal_flash" -I"C:/microchip/mla/v2016_11_07/bsp/pic18f46j50_pim" --warn=-3 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,+plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    --memorysummary dist/${CND_CONF}/${IMAGE_TYPE}/memoryfile.xml -odist/${CND_CONF}/${IMAGE_TYPE}/PIC18_USBTest.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}     
	
endif


# Subprojects
.build-subprojects:


# Subprojects
.clean-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/default
	${RM} -r dist/default

# Enable dependency checking
.dep.inc: .depcheck-impl

DEPFILES=$(shell mplabwildcard ${POSSIBLE_DEPFILES})
ifneq (${DEPFILES},)
include ${DEPFILES}
endif
