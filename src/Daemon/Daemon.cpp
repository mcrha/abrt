/* 
    Copyright (C) 2009  Jiri Moskovcak (jmoskovc@redhat.com) 
    Copyright (C) 2009  RedHat inc. 
 
    This program is free software; you can redistribute it and/or modify 
    it under the terms of the GNU General Public License as published by 
    the Free Software Foundation; either version 2 of the License, or 
    (at your option) any later version. 
 
    This program is distributed in the hope that it will be useful, 
    but WITHOUT ANY WARRANTY; without even the implied warranty of 
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
    GNU General Public License for more details. 
 
    You should have received a copy of the GNU General Public License along 
    with this program; if not, write to the Free Software Foundation, Inc., 
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA. 
    */
    
#include "CrashWatcher.h"
#include <iostream>
#include <cstdio>

CCrashWatcher *ccdaemon;
DBus::Glib::BusDispatcher *dispatcher;

void terminate(int signal)
{
    fprintf(stderr, "Got SIGINT/SIGTERM, cleaning up..\n");
    delete ccdaemon;
    delete dispatcher;
    exit(0);
}

int main(int argc, char** argv){
    int daemonize = 1;
    /*signal handlers */
    signal(SIGTERM, terminate);
    signal(SIGINT, terminate);
    /* connect to dbus */
    //DBus::Glib::BusDispatcher *dispatcher;
    dispatcher = new DBus::Glib::BusDispatcher();
    dispatcher->attach(NULL);
    DBus::default_dispatcher = dispatcher;
	DBus::Connection conn = DBus::Connection::SystemBus();
    
    try{
    ccdaemon = new CCrashWatcher(DEBUG_DUMPS_DIR, conn);
    if (argc > 1){
        if (strcmp(argv[1], "-d") == 0){
            daemonize = 0;
        }
    }
    if(daemonize){
            try{
                ccdaemon->Daemonize();
            }
            catch(std::string err)
            {
                std::cerr << err << std::endl;
            }
            catch(...){
                std::cerr << "daemon.cpp:Daemonize" << std::endl;
            }
        }
        else{
            try{
            #ifdef DEBUG
                std::cerr << "trying to run" << std::endl;
            #endif  /*DEBUG*/
                ccdaemon->Run();
            }
            catch(std::string err)
            {
                std::cerr << err << std::endl;
            }
            catch(...){
                std::cerr << "daemon.cpp:Run" << std::endl;
            }
        }
    }
    catch(std::string err)
    {
        std::cerr << "Cannot create daemon: " << err << std::endl;
    }
}

