// mmap.cpp

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

#include "stdafx.h"
#include "mmap.h"

using namespace std;

namespace mongo {

    set<MemoryMappedFile*> mmfiles;

    MemoryMappedFile::~MemoryMappedFile() {
        close();
        mmfiles.erase(this);
    }

    void MemoryMappedFile::created(){
        mmfiles.insert(this);
    }

    /*static*/
    int closingAllFiles = 0;
    void MemoryMappedFile::closeAllFiles( stringstream &message ) {
        if ( closingAllFiles ) {
            message << "warning closingAllFiles=" << closingAllFiles << endl;
            return;
        }
        ++closingAllFiles;
        ProgressMeter pm( mmfiles.size() , 2 , 1 );
        for ( set<MemoryMappedFile*>::iterator i = mmfiles.begin(); i != mmfiles.end(); i++ ){
            (*i)->close();
            pm.hit();
        }
        message << "    closeAllFiles() finished" << endl;
        --closingAllFiles;
    }

    long long MemoryMappedFile::totalMappedLength(){
        unsigned long long total = 0;
        
        for ( set<MemoryMappedFile*>::iterator i = mmfiles.begin(); i != mmfiles.end(); i++ )
            total += (*i)->length();

        return total;
    }

    void MemoryMappedFile::updateLength( const char *filename, long &length ) {
        if ( !boost::filesystem::exists( filename ) )
            return;
        // make sure we map full length if preexisting file.
        boost::uintmax_t l = boost::filesystem::file_size( filename );
        assert( l <= 0x7fffffff );
        length = (long) l;
    }

    void* MemoryMappedFile::map(const char *filename) {
        boost::uintmax_t l = boost::filesystem::file_size( filename );
        assert( l <= 0x7fffffff );
        long i = (long)l;
        return map( filename , i );
    }

} // namespace mongo
