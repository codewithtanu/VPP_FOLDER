#include <vlib/vlib.h>
#include <vlib/unix/plugin.h>
#include <vppinfra/error.h>

static clib_error_t *
hello_command_fn (vlib_main_t * vm,
                  unformat_input_t * input,
                  vlib_cli_command_t * cmd)
{
  
    vlib_cli_output(vm, "my Name is Tarun Kumar!");
    return 0;
}

/* CLI command definition */
VLIB_CLI_COMMAND (hello_command, static) = {
    .path = "second",
    .short_help = "hello - print Author Name",
    .function = hello_command_fn,
};

/* Plugin registration */
VLIB_PLUGIN_REGISTER () = {
    //  .version = VPP_BUILD_VER,
    .description = "My Second VPP Plugin",
};