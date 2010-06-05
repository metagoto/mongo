// s_only.cpp

/*    Copyright 2009 10gen Inc.
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

#include "pch.h"
#include "../client/dbclient.h"
#include "../db/dbhelpers.h"
#include "../db/matcher.h"

/*
  most a pile of hacks to make linking nicer

 */
namespace mongo {

    auto_ptr<CursorIterator> Helpers::find( const char *ns , BSONObj query , bool requireIndex ){
        uassert( 10196 ,  "Helpers::find can't be used in mongos" , 0 );
        auto_ptr<CursorIterator> i;
        return i;
    }

    boost::thread_specific_ptr<Client> currentClient;

    Client::Client( const char * s ){ assert(!"this shouldn't be called"); }
    Client::~Client(){ log() << "Client::~Client : this shouldn't be called" << endl; }
    bool Client::shutdown(){ assert(!"this shouldn't be called"); return true; }

}
