#include <vlib/vlib.h>
#include <vnet/vnet.h>
#include <vnet/plugin/plugin.h>
#include <vnet/feature/feature.h>
#include <vnet/ip/ip.h>

/*
create tap id 0 host-if-name vpp-tap0
set interface state tap0 up
set interface ip address tap0 10.10.1.1/24
set interface feature tap0 packet_count arc ip4-unicast


LINUX : -

sudo ip addr add 10.10.1.2/24 dev vpp-tap0
sudo ip link set vpp-tap0 up
ping 10.10.1.1
trace add virtio-input
*/

// ------------------------------------------------------------
// Global Counters
// ------------------------------------------------------------

u64 total_packets = 0;
u64 icmp_packets  = 0;
u64 tcp_packets   = 0;
u64 udp_packets   = 0;
u64 other_packets = 0;

// Debug frame counters
u64 node_a_frames = 0;
u64 node_b_frames = 0;

// ------------------------------------------------------------
// First Node : packet_count  (Node A)
// ------------------------------------------------------------

VLIB_NODE_FN(packet_count_node)
(vlib_main_t *vm,
 vlib_node_runtime_t *node,
 vlib_frame_t *frame)
{
  u32 *from = vlib_frame_vector_args(frame);
  u32 n_left_from = frame->n_vectors;

  u16 nexts[VLIB_FRAME_SIZE];

  node_a_frames++;

  vlib_cli_output(vm,
      "Node A (packet_count) running, frame=%llu, packets=%u",
      node_a_frames,
      n_left_from);

  for (u32 i = 0; i < n_left_from; i++)
  {
    vlib_buffer_t *b = vlib_get_buffer(vm, from[i]);

    ip4_header_t *ip = vlib_buffer_get_current(b);
    u8 protocol = ip->protocol;

    total_packets++;

    if (protocol == IP_PROTOCOL_ICMP)
      icmp_packets++;
    else if (protocol == IP_PROTOCOL_TCP)
      tcp_packets++;
    else if (protocol == IP_PROTOCOL_UDP)
      udp_packets++;
    else
      other_packets++;

    nexts[i] = 0;   // 0 → packet_count_pass
  }

  vlib_buffer_enqueue_to_next(vm, node, from, nexts, n_left_from);

  return n_left_from;
}

// ------------------------------------------------------------
// Second Node : packet_count_pass  (Node B)
// ------------------------------------------------------------

VLIB_NODE_FN(packet_count_pass_node)
(vlib_main_t *vm,
 vlib_node_runtime_t *node,
 vlib_frame_t *frame)
{
  u32 *from = vlib_frame_vector_args(frame);
  u32 n_left_from = frame->n_vectors;

  u16 nexts[VLIB_FRAME_SIZE];

  node_b_frames++;

  vlib_cli_output(vm,
      "Node B (packet_count_pass) running, frame=%llu, packets=%u",
      node_b_frames,
      n_left_from);

  for (u32 i = 0; i < n_left_from; i++)
  {
    nexts[i] = 0;   // 0 → ip4-lookup
  }

  vlib_buffer_enqueue_to_next(vm, node, from, nexts, n_left_from);

  return n_left_from;
}

// ------------------------------------------------------------
// Register First Node
// ------------------------------------------------------------

VLIB_REGISTER_NODE(packet_count_node) = {
  .name = "packet_count",
  .vector_size = sizeof(u32),
  .type = VLIB_NODE_TYPE_INTERNAL,

  .n_next_nodes = 1,
  .next_nodes = {
    [0] = "packet_count_pass",
  },
};

// ------------------------------------------------------------
// Register Second Node
// ------------------------------------------------------------

VLIB_REGISTER_NODE(packet_count_pass_node) = {
  .name = "packet_count_pass",
  .vector_size = sizeof(u32),
  .type = VLIB_NODE_TYPE_INTERNAL,

  .n_next_nodes = 1,
  .next_nodes = {
    [0] = "ip4-lookup",
  },
};

// ------------------------------------------------------------
// Attach Only First Node To Feature Arc
// ------------------------------------------------------------

VNET_FEATURE_INIT(packet_count_feature, static) = {
  .arc_name = "ip4-unicast",
  .node_name = "packet_count",
  .runs_before = VNET_FEATURES("ip4-lookup"),
};

// ------------------------------------------------------------
// CLI Command
// ------------------------------------------------------------

static clib_error_t *
show_packet_count(vlib_main_t *vm,
                  unformat_input_t *input,
                  vlib_cli_command_t *cmd)
{
  vlib_cli_output(vm, "===== Protocol Counters =====");
  vlib_cli_output(vm, "Total Packets : %llu", total_packets);
  vlib_cli_output(vm, "ICMP Packets  : %llu", icmp_packets);
  vlib_cli_output(vm, "TCP Packets   : %llu", tcp_packets);
  vlib_cli_output(vm, "UDP Packets   : %llu", udp_packets);
  vlib_cli_output(vm, "Other Packets : %llu", other_packets);

  vlib_cli_output(vm, "\n===== Frame Counters =====");
  vlib_cli_output(vm, "Node A Frames : %llu", node_a_frames);
  vlib_cli_output(vm, "Node B Frames : %llu", node_b_frames);

  return 0;
}

VLIB_CLI_COMMAND(show_packet_count_cmd, static) = {
  .path = "show multiple_nodes",
  .short_help = "show multiple_nodes",
  .function = show_packet_count,
};

// ------------------------------------------------------------
// Plugin Registration
// ------------------------------------------------------------

VLIB_PLUGIN_REGISTER() = {
  .version = "1.0",
  .description = "Two chained nodes example with debug prints",
};