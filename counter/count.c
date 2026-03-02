#include <vlib/vlib.h>
#include <vnet/plugin/plugin.h>
#include <vppinfra/error.h>


/*
   COMPILE AND COPY COMMAND
    
   gcc -fPIC -shared my_fast_plugin.c -o my_fast_plugin_plugin.so -I~/vpp/src -I~/vpp/build-root/install-vpp-native/vpp/include

cp my_fast_plugin_plugin.so \
~/vpp/build-root/install-vpp-native/vpp/lib/x86_64-linux-gnu/vpp_plugins/
*/


/* Global counter */
static u64 call_count = 0;




/* Run command */
static clib_error_t *
call_counter_run_fn (vlib_main_t * vm,
                     unformat_input_t * input,
                     vlib_cli_command_t * cmd)
{
    call_count++;

    vlib_cli_output(vm,
        "Plugin called %llu time(s)",
        call_count);

    return 0;
}

/* Show command */
static clib_error_t *
call_counter_show_fn (vlib_main_t * vm,
                      unformat_input_t * input,
                      vlib_cli_command_t * cmd)
{
    vlib_cli_output(vm,
        "Current call count: %llu",
        call_count);

    return 0;
}

/* Reset command */
static clib_error_t *
call_counter_reset_fn (vlib_main_t * vm,
                       unformat_input_t * input,
                       vlib_cli_command_t * cmd)
{
    call_count = 0;

    vlib_cli_output(vm,
        "Counter reset to 0");

    return 0;
}

/* CLI registrations */

VLIB_CLI_COMMAND (call_counter_run_cmd, static) = {
    .path = "counter run",
    .short_help = "call-counter run",
    .function = call_counter_run_fn,
};

VLIB_CLI_COMMAND (call_counter_show_cmd, static) = {
    .path = "counter show",
    .short_help = "call-counter show",
    .function = call_counter_show_fn,
};

VLIB_CLI_COMMAND (call_counter_reset_cmd, static) = {
    .path = "counter reset",
    .short_help = "call-counter reset",
    .function = call_counter_reset_fn,
};

/* Plugin registration */

VLIB_PLUGIN_REGISTER () = {
    .version = "1.0",
    .description = "Call Counter Plugin",
};