/* arp-pdcurses - Simple pdcurses TUI for "arp -a" command (Windows os).
Copyright (C) 2023  gH2843

This file is part of arp-pdcurses.

arp-pdcurses is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>. */

#include <windows.h>

#include <fstream>
#include <iostream>

#include "include/curses.h"

int main() {

    initscr();
    cbreak();
    noecho();
    curs_set(0);

    HKEY hKey;
    DWORD value, data_size = sizeof(DWORD);
    if  (RegGetValue(HKEY_CURRENT_USER, "SOFTWARE\\arp-pdcurses", "LicenseAgreementAccepted", RRF_RT_REG_DWORD, nullptr, &value, &data_size) == ERROR_SUCCESS){
        //run program without disclaimer
    }
    else {
        if ((RegCreateKeyEx(HKEY_CURRENT_USER, "SOFTWARE\\arp-pdcurses", 0, nullptr, REG_OPTION_NON_VOLATILE,KEY_WRITE, nullptr, &hKey, nullptr) == ERROR_SUCCESS)) {
            printw("arp-pdcurses Copyright (C) 2023  gH2843\nThis program comes with ABSOLUTELY NO WARRANTY;\nThis is free software, and you are welcome to redistribute it\nunder certain conditions;\nsee <https://www.gnu.org/licenses/gpl-3.0.html> for details.\n\n         Press 'q' to quit, any other key to continue");
            switch (getch()) {
                case 'q':
                case 'Q':
                case 65449:
                case 65417:
                    endwin();
                    RegCloseKey(hKey);
                    return 0;
                default:
                    value = 1;
                    RegSetValueEx(hKey, "LicenseAgreementAccepted", 0, REG_DWORD, reinterpret_cast<const BYTE *>(&value), data_size);
                    RegCloseKey(hKey);
                    break;
            }
        }
    }

    std::ifstream fin;
    char line;
    bool select_auto = false;
    int i = 0, delay = -1;
    do {
        timeout(delay);
        ++i;
        clear();
        system("arp -a > IP_arp.txt.temp_3234834");

        printw("(r)efresh (q)uit (a)uto  Count:%d%s",i,"\n-----------------------");
        if (select_auto) {
            mvchgat(0,17,6,A_REVERSE,0,NULL);
        }

        move(2, 0);
        fin.open("IP_arp.txt.temp_3234834");
        while (fin.get(line)) {
            printw("%c", line);
        }
        refresh();

        switch (getch()) {
            case 'a':
            case 'A':
            case 65508:
            case 65428:
                if (delay == -1) {
                    mvprintw(0, 24, "Enter delay:");
                    std::string buffer;
                    int num_ch;
                    mvchgat(0,17,6,A_REVERSE,0,NULL);
                    while ((num_ch = getch()) != '\n') {
                        if (isdigit(num_ch)) {
                            // add a digit to the buffer and display it on the screen
                            buffer += num_ch;
                            mvprintw( 0, 37, buffer.c_str());
                            refresh();
                        } else if (num_ch == KEY_BACKSPACE || num_ch == 8 && !buffer.empty()) {
                            // delete the last digit from the buffer and on the screen
                            buffer.pop_back();
                            move(0, 37);
                            clrtoeol();
                            mvprintw(0, 37, buffer.c_str());
                            refresh();
                        }
                    }
                    delay = (stoi(buffer) * 1000);
                    select_auto = true;
                } else {
                    delay = -1;
                    select_auto = false;
                }
                fin.close();
                break;
            case 'q':
            case 'Q':
            case 65449:
            case 65417:
                fin.close();
                remove("IP_arp.txt.temp_3234834");
                endwin();
                return 0;
            // case 'r':
            default:
                fin.close();
                break;
        }
    } while (true);
}
