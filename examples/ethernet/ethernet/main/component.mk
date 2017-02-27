#
# "main" pseudo-component makefile.
#
COMPONENT_SRCDIRS := .

COMPONENT_OBJS = ethernet_main.o
ifdef CONFIG_PHY_TLK110
COMPONENT_OBJS += tlk110_phy.o
endif
ifdef CONFIG_PHY_LAN8720
COMPONENT_OBJS += lan8720_phy.o
endif

COMPONENT_EXTRA_CLEAN := tlk110_phy.o lan8720_phy.o
