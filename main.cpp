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

#include "include/curses.h"

std::ifstream fin;

bool correctly_term = false;
void SignalHandler() {
    fin.close();
    remove("IP_arp.txt.temp_3234834");
    endwin();
    if (correctly_term) {
        exit(0);
    }
    exit(1);
}

int main() {
    initscr();
    cbreak();
    noecho();
    curs_set(0);

    atexit(SignalHandler); //signal handler, for the correct termination of the program in any case of termination

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
                    RegCloseKey(hKey);
                    correctly_term = true;
                    SignalHandler();
                default:
                    value = 1;
                    RegSetValueEx(hKey, "LicenseAgreementAccepted", 0, REG_DWORD, reinterpret_cast<const BYTE *>(&value), data_size);
                    RegCloseKey(hKey);
                    break;
            }
        }
    }

    char line;
    bool auto_is_selected = false;
    int i = 0, delay = -1;
    do {
        timeout(delay);
        ++i;
        clear();
        system("arp -a > IP_arp.txt.temp_3234834");

        printw("(r)efresh (q)uit (a)uto  Count:%d%s",i,"\n-----------------------");
        if (auto_is_selected) {
            mvchgat(0,17,6,A_REVERSE,0,nullptr);
        }

        move(2, 0);
        fin.open("IP_arp.txt.temp_3234834");
        while (fin.get(line)) {
            printw("%c", line);
        }
        refresh();

        fin.close();
        switch (getch()) {
            case 'a':
            case 'A':
            case 65508:
            case 65428:
                if (delay == -1) {
                    mvprintw(0, 24, "Enter delay:");
                    std::string buffer;
                    int num_ch;
                    mvchgat(0,17,6,A_REVERSE,0,nullptr);
                    while ((num_ch = getch()) != '\n') {
                        Buffer_Is_Empty_And_Endl: // to avoid an error when the buffer is empty
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
                    if (buffer.empty()) {goto Buffer_Is_Empty_And_Endl;} //to avoid an error when the buffer is empty
                    delay = (stoi(buffer) * 1000);
                    auto_is_selected = true;
                } else {
                    delay = -1;
                    auto_is_selected = false;
                }
                break;
            case 'q':
            case 'Q':
            case 65449:
            case 65417:
                correctly_term = true;
                SignalHandler();
                return 0;
            // case 'r':
            default:
                break;
        }
    } while (true);
}
