// engine.h

#pragma once

#include "../stdafx.h"
#include "../db/jsobj.h"

extern const char * jsconcatcode; // TODO: change name to mongoJSCode

namespace mongo {

    typedef unsigned long long ScriptingFunction;
    typedef BSONObj (*NativeFunction) ( const BSONObj &args );
    
    class Scope : boost::noncopyable {
    public:
        Scope();
        virtual ~Scope();
        
        virtual void reset() = 0;
        virtual void init( BSONObj * data ) = 0;
        void init( const char * data ){
            BSONObj o( data , 0 );
            init( &o );
        }
        
        virtual void localConnect( const char * dbName ) = 0;
        virtual void externalSetup() = 0;
        
        virtual double getNumber( const char *field ) = 0;
        virtual std::string getString( const char *field ) = 0;
        virtual bool getBoolean( const char *field ) = 0;
        virtual BSONObj getObject( const char *field ) = 0;

        virtual int type( const char *field ) = 0;

        void append( BSONObjBuilder & builder , const char * fieldName , const char * scopeName );

        virtual void setElement( const char *field , const BSONElement& e ) = 0;
        virtual void setNumber( const char *field , double val ) = 0;
        virtual void setString( const char *field , const char * val ) = 0;
        virtual void setObject( const char *field , const BSONObj& obj , bool readOnly=true ) = 0;
        virtual void setBoolean( const char *field , bool val ) = 0;
        virtual void setThis( const BSONObj * obj ) = 0;
                    
        virtual ScriptingFunction createFunction( const char * code );
        
        /**
         * @return 0 on success
         */
        virtual int invoke( ScriptingFunction func , const BSONObj& args, int timeoutMs = 0 , bool ignoreReturn = false ) = 0;
        void invokeSafe( ScriptingFunction func , const BSONObj& args, int timeoutMs = 0 ){
            assert( invoke( func , args , timeoutMs ) == 0 );
        }
        virtual std::string getError() = 0;
        
        int invoke( const char* code , const BSONObj& args, int timeoutMs = 0 );
        void invokeSafe( const char* code , const BSONObj& args, int timeoutMs = 0 ){
            if ( invoke( code , args , timeoutMs ) == 0 )
                return;
            throw UserException( (std::string)"invoke failed: " + getError() );
        }

        virtual bool exec( const std::string& code , const std::string& name , bool printResult , bool reportError , bool assertOnError, int timeoutMs = 0 ) = 0;
        virtual void execSetup( const std::string& code , const std::string& name = "setup" ){
            exec( code , name , false , true , true , 0 );
        }
        virtual bool execFile( const std::string& filename , bool printResult , bool reportError , bool assertOnError, int timeoutMs = 0 );
        
        virtual void injectNative( const char *field, NativeFunction func ) = 0;

        virtual void gc() = 0;

        void loadStored( bool ignoreNotConnected = false );
        
        /**
         if any changes are made to .system.js, call this
         right now its just global - slightly inefficient, but a lot simpler
        */
        static void storedFuncMod();
        
        static int getNumScopes(){
            return _numScopes;
        }
        
    protected:

        virtual ScriptingFunction _createFunction( const char * code ) = 0;

        std::string _localDBName;
        long long _loadedVersion;
        static long long _lastVersion;
        std::map<std::string,ScriptingFunction> _cachedFunctions;

        static int _numScopes;
    };
    
    class ScriptEngine : boost::noncopyable {
    public:
        ScriptEngine();
        virtual ~ScriptEngine();
        
        virtual Scope * createScope() = 0;
        
        virtual void runTest() = 0;
        
        virtual bool utf8Ok() const = 0;

        static void setup();

        std::auto_ptr<Scope> getPooledScope( const std::string& pool );
        void threadDone();
    };

    extern ScriptEngine * globalScriptEngine;
}
