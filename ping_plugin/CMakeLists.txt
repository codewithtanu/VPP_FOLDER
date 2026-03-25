add_vpp_plugin(my_ping
  SOURCES
  main.c

  LINK_LIBRARIES
  vppinfra
  vlib
  vnet
)