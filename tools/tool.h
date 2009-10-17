// Tool.h

#pragma once

#include <string>

#include <boost/program_options.hpp>

#if defined(_WIN32)
#include <io.h>
#endif

#include "client/dbclient.h"
#include "db/instance.h"

using std::string;

namespace mongo {

    class Tool {
    public:
        Tool( std::string name , std::string defaultDB="test" , std::string defaultCollection="");
        virtual ~Tool();

        int main( int argc , char ** argv );

        boost::program_options::options_description_easy_init add_options(){
            return _options->add_options();
        }
        boost::program_options::options_description_easy_init add_hidden_options(){
            return _hidden_options->add_options();
        }
        void addPositionArg( const char * name , int pos ){
            _positonalOptions.add( name , pos );
        }

        std::string getParam( std::string name , std::string def="" ){
            if ( _params.count( name ) )
                return _params[name.c_str()].as<std::string>();
            return def;
        }
        bool hasParam( std::string name ){
            return _params.count( name );
        }

        std::string getNS(){
            if ( _coll.size() == 0 ){
                std::cerr << "no collection specified!" << std::endl;
                throw -1;
            }
            return _db + "." + _coll;
        }

        virtual int run() = 0;

        virtual void printHelp(std::ostream &out);

        virtual void printExtraHelp( std::ostream & out );

    protected:

        mongo::DBClientBase &conn( bool slaveIfPaired = false );
        void auth( std::string db = "" );
        
        std::string _name;

        std::string _db;
        std::string _coll;

        std::string _username;
        std::string _password;

        void addFieldOptions();
        void needFields();
        
        std::vector<std::string> _fields;
        BSONObj _fieldsObj;

        
    private:
        std::string _host;
        mongo::DBClientBase * _conn;
        bool _paired;

        boost::program_options::options_description * _options;
        boost::program_options::options_description * _hidden_options;
        boost::program_options::positional_options_description _positonalOptions;

        boost::program_options::variables_map _params;

    };

}
