# VPP Plugin Development Guide (Without Rebuilding VPP)

This guide explains how to:

-   Create a VPP plugin
-   Compile it as a shared library (.so)
-   Load it into VPP
-   Avoid rebuilding VPP every time
-   Develop plugins cleanly and safely

------------------------------------------------------------------------

## 1️⃣ Create Plugin Source File

Example: `second.c`

``` c
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
    .description = "My Second VPP Plugin",
};
```

------------------------------------------------------------------------

## 2️⃣ Compile as Shared Library (.so)

``` bash
gcc -fPIC -shared -o second.so second.c   -I/usr/include/vpp   -lvlib -lvppinfra
```

Explanation:

-   `-fPIC` → Position Independent Code (required for shared libs)
-   `-shared` → Create .so file
-   `-I/usr/include/vpp` → Include VPP headers
-   `-lvlib -lvppinfra` → Link required VPP libraries

------------------------------------------------------------------------

## 3️⃣ Option 1 --- Copy to System Plugin Directory

``` bash
sudo cp second.so /usr/lib/x86_64-linux-gnu/vpp_plugins/
sudo systemctl restart vpp
```

Check plugin:

``` bash
sudo vppctl
show plugins
```

Run command inside VPP:

``` bash
second
```

------------------------------------------------------------------------

## 4️⃣ ✅ Recommended Method (No Copy Every Time)

Instead of copying to system directory:

Edit:

``` bash
sudo nano /etc/vpp/startup.conf
```

Add:

    plugins {
      path /home/tarun/Desktop/VPP/second
    }

Now:

-   Keep `second.so` in your project folder
-   Just recompile
-   Restart VPP

``` bash
gcc -fPIC -shared -o second.so second.c   -I/usr/include/vpp   -lvlib -lvppinfra

sudo systemctl restart vpp
```

No copying required anymore.

------------------------------------------------------------------------

## 5️⃣ Development Workflow

Every time you change code:

1.  Edit `second.c`
2.  Recompile
3.  Restart VPP

```{=html}
<!-- -->
```
    gcc -fPIC -shared -o second.so second.c   -I/usr/include/vpp   -lvlib -lvppinfra

    sudo systemctl restart vpp

Then:

    sudo vppctl
    second

------------------------------------------------------------------------

## 6️⃣ Important Notes

-   VPP loads plugins only at startup
-   No hot reload support
-   CLI command name is defined by `.path`
-   File name of `.so` does NOT define command name

------------------------------------------------------------------------

## 7️⃣ Clean Development Tip

Avoid copying plugins to:

    /usr/lib/x86_64-linux-gnu/vpp_plugins/

Instead use custom plugin path in `startup.conf`.

------------------------------------------------------------------------

# 🎯 Final Result

You can now:

-   Develop VPP plugins
-   Compile independently
-   Load dynamically at startup
-   Avoid rebuilding VPP

Standalone VPP plugin development workflow 🚀
