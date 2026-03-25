#include <vlib/vlib.h>
#include <vnet/vnet.h>
#include <vnet/ip/ip.h>
#include <vnet/ip/icmp46_packet.h>
#include <vnet/ethernet/ethernet.h>
#include <vnet/plugin/plugin.h>

/* Trace structure */
typedef struct
{
    u32 next_index;
} my_ping_trace_t;

typedef enum
{
    PING_REPLY_NEXT_INTERFACE_OUTPUT,
    PING_REPLY_N_NEXT,
} ping_reply_next_t;

/* Trace formatter */
static u8 *
format_my_ping_trace (u8 * s, va_list * args)
{
    CLIB_UNUSED(vlib_main_t * vm) = va_arg (*args, vlib_main_t *);
    CLIB_UNUSED(vlib_node_t * node) = va_arg (*args, vlib_node_t *);
    my_ping_trace_t * t = va_arg (*args, my_ping_trace_t *);

    s = format (s, "ping-reply-node: next index %d", t->next_index);
    return s;
}


/* Node function */
VLIB_NODE_FN(my_ping_node_fn)
(vlib_main_t *vm,
 vlib_node_runtime_t *node,
 vlib_frame_t *frame)
{
    u32 *from = vlib_frame_vector_args(frame);
    u32 n_left = frame->n_vectors;
    u32 nexts[VLIB_FRAME_SIZE];

   u32 i = 0;

while (n_left > 0)
{
    u32 bi0 = from[0];
    vlib_buffer_t *b0 = vlib_get_buffer(vm, bi0);

    ethernet_header_t *eth = vlib_buffer_get_current(b0);
    ip4_header_t *ip = (ip4_header_t *)(eth + 1);
    icmp46_header_t *icmp = (icmp46_header_t *)(ip + 1);

    if (icmp->type == ICMP4_echo_request)
    {
        icmp->type = ICMP4_echo_reply;

        ip4_address_t tmp_ip = ip->src_address;
        ip->src_address = ip->dst_address;
        ip->dst_address = tmp_ip;

        u8 tmp_mac[6];
        clib_memcpy(tmp_mac, eth->src_address, 6);
        clib_memcpy(eth->src_address, eth->dst_address, 6);
        clib_memcpy(eth->dst_address, tmp_mac, 6);

        ip->checksum = 0;
        ip->checksum = ip4_header_checksum(ip);

        icmp->checksum = 0;
    }

    nexts[i] = 0;   // correct indexing

    if (PREDICT_FALSE(b0->flags & VLIB_BUFFER_IS_TRACED))
    {
        my_ping_trace_t *t =
            vlib_add_trace(vm, node, b0, sizeof(*t));
        t->next_index = 0;
    }

    from++;
    n_left--;
    i++;
}

    vlib_buffer_enqueue_to_next(vm, node,
                                vlib_frame_vector_args(frame),
                                nexts,
                                frame->n_vectors);

    return frame->n_vectors;
}


/* Register node */
VLIB_REGISTER_NODE(my_ping_node_fn) =
{
    .name = "ping-reply-node",
    .vector_size = sizeof(u32),
    .type = VLIB_NODE_TYPE_INTERNAL,
    .format_trace = format_my_ping_trace,

    .n_next_nodes = PING_REPLY_N_NEXT,
    .next_nodes =
    {
        [PING_REPLY_NEXT_INTERFACE_OUTPUT] = "interface-output",
    },
};


/* Attach node to feature arc */
VNET_FEATURE_INIT(ping_reply_feature, static) =
{
    .arc_name = "ip4-local",
    .node_name = "ping-reply-node",
    .runs_before = VNET_FEATURES("ip4-lookup"),
};


/* CLI command */
static clib_error_t *
show_my_ping_fn(vlib_main_t *vm,
                unformat_input_t *input,
                vlib_cli_command_t *cmd)
{
    vlib_cli_output(vm, "Ping reply plugin running");
    return 0;
}

VLIB_CLI_COMMAND(show_packet_count_cmd, static) =
{
    .path = "show my_ping",
    .short_help = "show my_ping",
    .function = show_my_ping_fn,
};


/* Plugin registration */
VLIB_PLUGIN_REGISTER () =
{
    .version = "1.0",
    .description = "Ping reply plugin",
};