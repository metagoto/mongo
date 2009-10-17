// config.h

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

/* This file is things related to the "grid configuration":
   - what machines make up the db component of our cloud
   - where various ranges of things live
*/

#pragma once

#include "../db/namespace.h"
#include "../client/dbclient.h"
#include "../client/model.h"
#include "shardkey.h"

namespace mongo {
    
    class Grid;
    class ConfigServer;

    extern ConfigServer configServer;
    extern Grid grid;

    class ChunkManager;

    class CollectionInfo {
    public:
        CollectionInfo( ShardKeyPattern _key = BSONObj() , bool _unique = false ) : 
            key( _key ) , unique( _unique ){}

        ShardKeyPattern key;
        bool unique;
    };
    
    /**
       top level grid configuration for an entire database
    */
    class DBConfig : public Model {
    public:
        DBConfig( std::string name = "" ) : _name( name ) , _primary("") , _shardingEnabled(false){ }
        
        std::string getName(){ return _name; };

        /**
         * @return if anything in this db is partitioned or not
         */
        bool isShardingEnabled(){
            return _shardingEnabled;
        }
        
        void enableSharding();
        ChunkManager* shardCollection( const std::string& ns , ShardKeyPattern fieldsAndOrder , bool unique );
        
        /**
         * @return whether or not this partition is partitioned
         */
        bool isSharded( const std::string& ns );
        
        ChunkManager* getChunkManager( const std::string& ns , bool reload = false );
        
        /**
         * @return the correct for shard for the ns
         * if the namespace is sharded, will return an empty string
         */
        std::string getShard( const std::string& ns );
        
        std::string getPrimary(){
            if ( _primary.size() == 0 )
                throw UserException( (std::string)"no primary shard configured for db: " + _name );
            return _primary;
        }
        
        void setPrimary( std::string s ){
            _primary = s;
        }
        
        virtual void save( bool check=true);

        virtual std::string modelServer();
        
        // model stuff

        virtual const char * getNS(){ return "config.databases"; }
        virtual void serialize(BSONObjBuilder& to);
        virtual void unserialize(const BSONObj& from);
        bool loadByName(const char *nm);
        
    protected:
        std::string _name; // e.g. "alleyinsider"
        std::string _primary; // e.g. localhost , mongo.foo.com:9999
        bool _shardingEnabled;
        
        std::map<std::string,CollectionInfo> _sharded; // { "alleyinsider.blog.posts" : { ts : 1 }  , ... ] - all ns that are sharded
        std::map<std::string,ChunkManager*> _shards; // this will only have entries for things that have been looked at

        friend class Grid;
    };

    class Grid {
    public:
        /**
           gets the config the db.
           will return an empty DBConfig if not in db already
         */
        DBConfig * getDBConfig( std::string ns , bool create=true);
        
        std::string pickShardForNewDB();
        
        bool knowAboutShard( std::string name ) const;

        unsigned long long getNextOpTime() const;
    private:
        std::map<std::string,DBConfig*> _databases;
    };

    class ConfigServer : public DBConfig {
    public:

        enum { Port = 27016 }; /* standard port # for a grid db */
        
        ConfigServer();
        ~ConfigServer();

        bool ok(){
            // TODO: check can connect
            return _primary.size() > 0;
        }
        
        virtual std::string modelServer(){
            uassert( "ConfigServer not setup" , _primary.size() );
            return _primary;
        }

        /**
           call at startup, this will initiate connection to the grid db 
        */
        bool init( std::vector<std::string> configHosts , bool infer );
        
        int dbConfigVersion();
        int dbConfigVersion( DBClientBase& conn );

        /**
         * @return 0 = ok, otherwise error #
         */
        int checkConfigVersion();
        
        static int VERSION;

    private:
        std::string getHost( std::string name , bool withPort );
    };
    
} // namespace mongo
