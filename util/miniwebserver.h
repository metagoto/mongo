// miniwebserver.h

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

#pragma once

#include "message.h"

namespace mongo {

    class MiniWebServer {
    public:
        MiniWebServer();
        virtual ~MiniWebServer() {}

        bool init(const std::string &ip, int _port);
        void run();

        virtual void doRequest(
            const char *rq, // the full request
            std::string url,
            // set these and return them:
            std::string& responseMsg,
            int& responseCode,
            std::vector<std::string>& headers, // if completely empty, content-type: text/html will be added
            const SockAddr &from
        ) = 0;

        int socket() const { return sock; }
        
    protected:
        std::string parseURL( const char * buf );
        std::string parseMethod( const char * headers );
        std::string getHeader( const char * headers , std::string name );
        void parseParams( std::map<std::string,std::string> & params , std::string query );
        static const char *body( const char *buf );

    private:
        void accepted(int s, const SockAddr &from);
        static bool fullReceive( const char *buf );

        int port;
        int sock;
    };

} // namespace mongo
