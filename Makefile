MCU_TARGET := nRF52840_xxAA
PROG_PORT := 4444

.PHONY: debug
debug:
	JLinkExe -device $(MCU_TARGET) -speed 4000 -if SWD -autoconnect 1 -RTTTelnetPort $(PROG_PORT)

.PHONY: rtt
rtt:
	jlinkrttclient -RTTTelnetPort $(PROG_PORT)
