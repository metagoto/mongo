// @file writeback_listener.h

/**
*    Copyright (C) 2010 10gen Inc.
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

#include "../pch.h"

#include "../client/connpool.h"
#include "../util/background.h"

namespace mongo {

    /*
     * The writeback listener takes back write attempts that were made against a wrong shard.
     * (Wrong here in the sense that the target chunk moved before this mongos had a chance to
     * learn so.) It is responsible for reapplying these writes to the correct shard.
     *
     * Currently, there is one listener per shard.
     */
    class WriteBackListener : public BackgroundJob {
    public:
        static void init( DBClientBase& conn );

        static void waitFor( const OID& oid );

    protected:
        WriteBackListener( const string& addr );

        string name() const { return "WriteBackListener"; }        
        void run();

    private:
        string _addr;

        static mongo::mutex _cacheLock; // protects _cache
        static map<string,WriteBackListener*> _cache;
        
        static mongo::mutex _seenWritebacksLock;  // protects _seenWritbacks
        static set<OID> _seenWritebacks; // TODO: this can grow unbounded
    };
    
    void waitForWriteback( const OID& oid );

}  // namespace mongo
