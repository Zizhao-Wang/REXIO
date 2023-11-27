
#ifndef EXPERIMENT1_LOGO_H
#define EXPERIMENT1_LOGO_H

#define NEXIO_LOGO "\
\x1b[1;34m==============================================\x1b[0m\n\
\x1b[1;37m\x1b[44m                                          \x1b[0m\n\
\x1b[1;37m\x1b[44m  __   _   _____  __    __  _   _____   \x1b[0m\n\
\x1b[1;37m\x1b[44m |  \\ | | | ____| \\ \\  / / | | /  _  \\  \x1b[0m\n\
\x1b[1;37m\x1b[44m |   \\| | | |__    \\ \\/ /  | | | | | |  \x1b[0m\n\
\x1b[1;37m\x1b[44m | |\\   | |  __|    }  {   | | | | | |  \x1b[0m\n\
\x1b[1;37m\x1b[44m | | \\  | | |___   / /\\ \\  | | | |_| |  \x1b[0m\n\
\x1b[1;37m\x1b[44m |_|  \\_| |_____| /_/  \\_\\ |_| \\_____/  \x1b[0m\n\
\x1b[1;37m\x1b[44m                                          \x1b[0m\n\
"

#define UCAS_SIAT "\
 _   _   _____       ___   _____        _____   _       ___   _____  \n\
| | | | /  ___|     /   | /  ___/      /  ___/ | |     /   | |_   _| \n\
| | | | | |        / /| | | |___       | |___  | |    / /| |   | |   \n\
| | | | | |       / /_| | \\___  \\------\\___  \\ | |   / /_| |   | |   \n\
| |_| | | |___   / /  | |  ___| |       ___| | | |  / /  | |   | |   \n\
\\_____/ \\_____| /_/   |_| /_____/      /_____/ |_| /_/   |_|   |_|   \n\
                 ______  _   ______  _   _       ___   _____        _          __  ___   __   _   _____  \n\
                |___  / | | |___  / | | | |     /   | /  _  \\      | |        / / /   | |  \\ | | /  ___| \n\
                   / /  | |    / /  | |_| |    / /| | | | | |      | |  __   / / / /| | |   \\| | | |     \n\
                  / /   | |   / /   |  _  |   / /_| | | | | |      | | /  | / / / /_| | | |\\   | | |  _  \n\
                 / /__  | |  / /__  | | | |  / /  | | | |_| |      | |/   |/ / / /  | | | | \\  | | |_| | \n\
                /_____| |_| /_____| |_| |_| /_/   |_| \\_____/      |___/|___/ /_/   |_| |_|  \\_| \\_____/\n\
"

#define PROJECT_INTRO "\
\x1b[1;32mWelcome to NEXIO!\x1b[0m\n\
\x1b[1;36mNEXIO is an industry-leading key-value store, setting benchmarks in the realm of high-performance data storage solutions.\n\
Empowered by cutting-edge algorithms and innovative data structures, NEXIO provides unparalleled speed, reliability, and efficiency.\n\
It's not just about storing data, it's about accessing it swiftly and reliably, every single time.\n\
Whether it's real-time analytics, mission-critical databases, or everyday computing needs, NEXIO stands out as the preferred choice for modern storage needs.\x1b[0m\n\
\x1b[1;34m==============================================\x1b[0m\n\
"


void logo_print(){
    printf("%s", NEXIO_LOGO);
    printf("%s", UCAS_SIAT);
    printf("%s", PROJECT_INTRO);
}


#endif //EXPERIMENT1_LOGO_H