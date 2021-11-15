#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <inttypes.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <uiohook/uiohook.h>
#include <wchar.h>
#include <traypp/tray.hpp>

bool logger_proc(unsigned int level, const char* format, ...) {
    bool status = false;

    va_list args;
    switch (level) {
    case LOG_LEVEL_INFO:
        va_start(args, format);
        status = vfprintf(stdout, format, args) >= 0;
        va_end(args);
        break;

    case LOG_LEVEL_WARN:
    case LOG_LEVEL_ERROR:
        va_start(args, format);
        status = vfprintf(stderr, format, args) >= 0;
        va_end(args);
        break;
    }

    return status;
}

// NOTE: The following callback executes on the same thread that hook_run() is called 
// from.  This is important because hook_run() attaches to the operating systems
// event dispatcher and may delay event delivery to the target application.
// Furthermore, some operating systems may choose to disable your hook if it 
// takes too long to process.  If you need to do any extended processing, please 
// do so by copying the event to your own queued dispatch thread.

void dispatch_proc(uiohook_event* const event) {
    char buffer[256] = { 0 };
    size_t length = snprintf(buffer, sizeof(buffer),
        "id=%i,when=%" PRIu64 ",mask=0x%X",
        event->type, event->time, event->mask);

    switch (event->type) {
    case EVENT_KEY_PRESSED:
        // If the escape key is pressed, naturally terminate the program.
        if (event->data.keyboard.keycode == VC_F7) {

            break;

        }
    case EVENT_KEY_RELEASED:
        snprintf(buffer + length, sizeof(buffer) - length,
            ",keycode=%u,rawcode=0x%X",
            event->data.keyboard.keycode, event->data.keyboard.rawcode);
        break;

    case EVENT_KEY_TYPED:
        snprintf(buffer + length, sizeof(buffer) - length,
            ",keychar=%lc,rawcode=%u",
            (wint_t)event->data.keyboard.keychar,
            event->data.keyboard.rawcode);
        break;
    default:
        return;
    }
    fprintf(stdout, "%s\n", buffer);
}

int hookstatus(bool stateOn) {
    if (stateOn == true) {
        hook_run();
    }
    else {
        hook_stop();
    }
    return stateOn = !stateOn;
}

int main() {

    Tray::Tray tray("test", "icon.ico");
    tray.addEntry(Tray::Button("Exit", [&] { tray.exit(); exit(1); }));
    tray.addEntry(Tray::Button("Test"))->setDisabled(true);
    tray.addEntry(Tray::Separator());
    tray.addEntry(Tray::Toggle("Sound", true, [](bool stateOn) { hookstatus(stateOn); printf("Deine Mum: %i\n", stateOn); }));
    tray.addEntry(Tray::Separator());
    tray.addEntry(Tray::Submenu("Test Submenu"))->addEntry(Tray::Button("Submenu button!"))->setDisabled(true);

    hook_set_logger_proc(&logger_proc);
    hook_set_dispatch_proc(&dispatch_proc);

    hook_run(); // Starting State On
    tray.run();
}