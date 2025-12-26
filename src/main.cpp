#include "SignalMonitor.h"

int main(const int argc, char *argv[]) {
    ConfigManager c {};
    if (argc > 1) {
        c.setCommand(argv[1]);
    }
    SignalMonitor sm { &c };
    sm.run();
    return 0;
}