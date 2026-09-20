include $(APPDIR)/Make.defs

PROGNAME  = cm_app
PRIORITY  = SCHED_PRIORITY_DEFAULT
STACKSIZE = 32768
MODULE    = $(CONFIG_LVX_USE_CONTEST2026_026_CAM_APP)
CSRCS      = wifi_client.c log.c gui/gui.c gui/cm_terminal.c socket_tools/socket_tools.c
MAINSRC   = cm_app.c

include $(APPDIR)/Application.mk
