#include "stdafx.h"
//--------------------------------------------------------------------------
char comment[] = "This is a universal plugin for working with packed executable files. For dumping, filling with data and for nopping areas.";

char help[] =
        "This is a universal plugin for working with packed executable files. For dumping, filling with data and for nopping areas.\n"
        "\n";


//--------------------------------------------------------------------------
// This is the preferred name of the plugin module in the menu system
// The preferred name may be overriden in plugins.cfg file

char wanted_name[] = "Memory dumper plugin";


// This is the preferred hotkey for the plugin module
// The preferred hotkey may be overriden in plugins.cfg file
// Note: IDA won't tell you if the hotkey is not correct
//       It will just disable the hotkey.

char wanted_hotkey[] = "Ctrl-D";
