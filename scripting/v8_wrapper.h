// v8_wrapper.h

#pragma once

#include <v8.h>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include "../db/jsobj.h"

namespace mongo {

    v8::Local<v8::Object> mongoToV8( const mongo::BSONObj & m , bool array = 0 );
    mongo::BSONObj v8ToMongo( v8::Handle<v8::Object> o );

    void v8ToMongoElement( BSONObjBuilder & b , v8::Handle<v8::String> name , 
                           const std::string sname , v8::Handle<v8::Value> value );
    v8::Handle<v8::Value> mongoToV8Element( const BSONElement &f );
    
    v8::Function * getNamedCons( const char * name );
    v8::Function * getObjectIdCons();

}
