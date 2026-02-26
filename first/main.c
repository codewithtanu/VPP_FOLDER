#include <vlib/vlib.h>
#include <vnet/plugin/plugin.h>
#include <vppinfra/error.h>

static clib_error_t *
hello_command_fn (vlib_main_t * vm,
                  unformat_input_t * input,
                  vlib_cli_command_t * cmd)
{
    char *name = 0;

    /* Parse CLI arguments */
    while (unformat_check_input(input) != UNFORMAT_END_OF_INPUT)
    {
        if (unformat(input, "name %s", &name))
            ;
        else
            return clib_error_return(0, "Unknown input: `%U'",
                                     format_unformat_error, input);
    }

    if (!name)
        name = "Tarun Kumar";

    vlib_cli_output(vm, "Hello %s 👋", name);
    vlib_cli_output(vm, "Welcome to advanced VPP plugin development!");

    return 0;
}

/* CLI command registration */
VLIB_CLI_COMMAND (hello_command, static) = {
    .path = "main",
    .short_help = "myplugin hello [name <your_name>]",
    .function = hello_command_fn,
};

/* Plugin registration */
VLIB_PLUGIN_REGISTER () = {
    .version = "1.0",
    .description = "My Advanced VPP Plugin",
};