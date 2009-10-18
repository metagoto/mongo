// client.h

/**
*    Copyright (C) 2008 10gen Inc.
*
*    This program is free software: you can redistribute it and/or  modify
*    it under the terms of the GNU Affero General Public License, version 3,
*    as published by the Free Software Foundation.
*
*    This program is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU Affero General Public License for more details.
*
*    You should have received a copy of the GNU Affero General Public License
*    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/* Client represents a connection to the database (the server-side) and corresponds 
   to an open socket (or logical connection if pooling on sockets) from a client.

   todo: switch to asio...this will fit nicely with that.
*/

#pragma once

#include "lasterror.h"
#include "../util/top.h"

namespace mongo { 

    class AuthenticationInfo;
    class Database;
    struct CurOp;

    class Client : boost::noncopyable { 
    public:
        static boost::mutex clientsMutex;
        static set<Client*> clients; // always be in clientsMutex when manipulating this
    private:
        CurOp *_op;
        Database *_database;
        Namespace _ns;
        //NamespaceString _nsstr;
        bool _shutdown;
        list<string> _tempCollections;
        const char *_desc;
    public:
        AuthenticationInfo *ai;
        Top top;

        CurOp* curop() { return _op; }
        Database* database() { return _database; }
        const char *ns() { return _ns.buf; }

        void setns(const char *ns, Database *db) { 
            _database = db;
            _ns = ns;
            //_nsstr = ns;
        }
        void clearns() { setns("", 0); }

        Client(const char *desc);
        ~Client();

        void addTempCollection( const string& ns ){
            _tempCollections.push_back( ns );
        }

        /* each thread which does db operations has a Client object in TLS.  
           call this when your thread starts. 
        */
        static void initThread(const char *desc);

        /* 
           this has to be called as the client goes away, but before thread termination
           @return true if anything was done
         */
        bool shutdown();
    };
    
    /* defined in security.cpp */
    extern boost::thread_specific_ptr<Client> currentClient;

    inline Client& cc() { 
        return *currentClient.get();
    }

    inline void Client::initThread(const char *desc) {
        assert( currentClient.get() == 0 );
        currentClient.reset( new Client(desc) );
    }

};
