add_vpp_plugin(packet_count
  SOURCES
  status.c

  LINK_LIBRARIES
  vppinfra
  vlib
  vnet
)