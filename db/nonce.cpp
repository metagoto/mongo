// nonce.cpp

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

#include "stdafx.h"
#include "nonce.h"

extern int do_md5_test(void);

namespace mongo {
    
    Security::Security(){
#if defined(__linux__)
        devrandom = new ifstream("/dev/urandom", ios::binary|ios::in);
        massert( "can't open dev/urandom", devrandom->is_open() );
#elif defined(_WIN32)
        srand(curTimeMicros());
#else
        srandomdev();
#endif
        assert( sizeof(nonce) == 8 );
        
#ifndef NDEBUG
        if ( do_md5_test() )
	    massert("md5 unit test fails", false);
#endif
    }
    
    nonce Security::getNonce(){
        nonce n;
#if defined(__linux__)
        devrandom->read((char*)&n, sizeof(n));
        massert("devrandom failed", !devrandom->fail());
#elif defined(_WIN32)
        n = (((unsigned long long)rand())<<32) | rand();
#else
        n = (((unsigned long long)random())<<32) | random();
#endif
        return n;
    }
    
    Security security;
        
} // namespace mongo
