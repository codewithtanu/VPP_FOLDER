#include <vlib/vlib.h>
#include <vnet/vnet.h>
#include <vnet/plugin/plugin.h>

typedef struct {
    u64 packets;
} counter_main_t;

counter_main_t counter_main;

static uword
packet_counter_node_fn (vlib_main_t * vm,
                        vlib_node_runtime_t * node,
                        vlib_frame_t * frame)
{
    u32 n_left_from, *from;
    u32 next_index;
    u32 *to_next;
    u32 n_left_to_next;

    from = vlib_frame_vector_args(frame);
    n_left_from = frame->n_vectors;

    /* Count packets */
    counter_main.packets += n_left_from;

    next_index = 0; /* ip4-input */

    vlib_get_next_frame(vm, node, next_index,
                        to_next, n_left_to_next);

    while (n_left_from > 0 && n_left_to_next > 0)
    {
        u32 bi0 = from[0];

        to_next[0] = bi0;

        from++;
        to_next++;
        n_left_from--;
        n_left_to_next--;
    }

    vlib_put_next_frame(vm, node, next_index,
                        n_left_to_next);

    return frame->n_vectors;
}

VLIB_REGISTER_NODE(packet_counter_node) = {
    .function = packet_counter_node_fn,
    .name = "packet-counter-node",
    .vector_size = sizeof(u32),
    .type = VLIB_NODE_TYPE_INTERNAL,
    .n_next_nodes = 1,
    .next_nodes = {
        [0] = "ip4-input",
    },
};

static clib_error_t *
show_counter_fn (vlib_main_t * vm,
                 unformat_input_t * input,
                 vlib_cli_command_t * cmd)
{
    vlib_cli_output(vm, "Packets counted: %llu",
                    counter_main.packets);
    return 0;
}

VLIB_CLI_COMMAND(show_counter_command, static) = {
    .path = "my-counter",
    .short_help = "show my-counter",
    .function = show_counter_fn,
};

VLIB_PLUGIN_REGISTER () = {
    .description = "Packet Counter Plugin",
};