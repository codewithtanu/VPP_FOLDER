add_vpp_plugin(multiple_nodes
  SOURCES
  main.c

  LINK_LIBRARIES
  vppinfra
  vlib
  vnet
)