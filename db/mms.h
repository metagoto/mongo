// mms.h

#pragma once

#include "../stdafx.h"
#include "../util/background.h"

namespace mongo {


    /** Mongo Monitoring Service
        if enabled, this runs in the background ands pings mss
     */
    class MMS : public BackgroundJob {
    public:

        MMS();
        ~MMS();

        /**
           e.g. http://mms.10gen.com/ping/
         */
        void setBaseUrl( const std::string& host );
        
        void setToken( const std::string& s ){ token = s; }
        void setName( const std::string& s ){ name = s; }

        void setPingInterval( int seconds ){ secsToSleep = seconds; }

        void run();

    private:
        std::string baseurl;
        int secsToSleep;
        
        std::string token;
        std::string name;

    };

    extern MMS mms;
}
