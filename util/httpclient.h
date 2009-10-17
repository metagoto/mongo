// httpclient.h

#pragma once

#include "../stdafx.h"

namespace mongo {
    
    class HttpClient {
    public:
        int get( std::string url , std::map<std::string,std::string>& headers, std::stringstream& data );
    };
}

