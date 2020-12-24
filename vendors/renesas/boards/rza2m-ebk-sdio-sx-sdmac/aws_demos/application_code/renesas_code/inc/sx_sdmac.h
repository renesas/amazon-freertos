/****************************************************************************
 *   wifi_cli_demo.c                                                        *
 *   Sample API Usage and CLI demo                                          *
 *                                                                          *
 *   (C) Copyright silex technology                                         *
 ***************************************************************************/
#define WIFI_INTERFACE_NAME "wlan0"
extern int wifi_cli_setup(void);
extern int cmd_wifi_init(int argc, char **argv, char *rsp);
extern int cmd_wifi_connect(int argc, char **argv, char *rsp);
extern int cmd_disconnect(int argc, char **argv, char *rsp);
extern int sx_netdev_stack_init(void);
extern int sx_cli_setup(void);
