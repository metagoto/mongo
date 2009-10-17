// strategy.h

#pragma once

#include "../stdafx.h"
#include "chunk.h"
#include "request.h"

namespace mongo {
    
    class Strategy {
    public:
        Strategy(){}
        virtual ~Strategy() {}
        virtual void queryOp( Request& r ) = 0;
        virtual void getMore( Request& r ) = 0;
        virtual void writeOp( int op , Request& r ) = 0;
        
    protected:
        void doWrite( int op , Request& r , std::string server );
        void doQuery( Request& r , std::string server );
        
        void insert( std::string server , const char * ns , const BSONObj& obj );
        
    };

    extern Strategy * SINGLE;
    extern Strategy * SHARDED;

    void checkShardVersion( DBClientBase & conn , const std::string& ns , bool authoritative = false );
    
    bool setShardVersion( DBClientBase & conn , const std::string& ns , ShardChunkVersion version , bool authoritative , BSONObj& result );

    bool lockNamespaceOnServer( const std::string& server , const std::string& ns );
    bool lockNamespaceOnServer( DBClientBase& conn , const std::string& ns );

}

