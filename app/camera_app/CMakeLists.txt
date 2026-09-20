if(CONFIG_LVX_USE_CONTEST2026_026_CAM_APP)
  nuttx_add_application(
    NAME cm_app
    SRCS cm_app.c wifi_client.c log.c gui/gui.c gui/cm_terminal.c socket_tools/socket_tools.c
    STACKSIZE 32768
    CFLAGS -Dmain=main_ctrl
    DEPENDS lvgl)
endif()
