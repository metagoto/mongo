// @file mongommf.cpp

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

/* this module adds some of our layers atop memory mapped files - specifically our handling of private views & such 
   if you don't care about journaling/durability (temp sort files & such) use MemoryMappedFile class, not this.
*/

#include "pch.h"
#include "mongommf.h"

namespace mongo {

#if !defined(_DEBUG)
    ///*static*/ void* MongoMMF::switchToPrivateView(void *p) { return p; }
#else
    // see dur.h.
    static map<void *, MongoMMF*> our_read_views;
    static mutex our_views_mutex("");

    /*static*/ void* MongoMMF::switchToPrivateView(void *readonly_ptr) { 
        void *p = readonly_ptr;
        assert( durable );
        assert( debug );
        mutex::scoped_lock lk(our_views_mutex);
        std::map< void*, MongoMMF* >::iterator i = 
            our_read_views.upper_bound(((char *)p)+1);
        i--;

        bool ok = i != our_read_views.end();
        if( ok ) {
            MongoMMF *mmf = i->second;
            assert( mmf );

            size_t ofs = ((char *)p) - ((char*)mmf->_view_readonly);

            if( ofs < mmf->length() ) { 
                return ((char *)mmf->_view_private) + ofs;
            }
        }

        if( 1 ) { 
            static int once;
            /* temp : not using MongoMMF yet for datafiles, just .ns.  more to do... */
            if( once++ == 0 )
                log() << "TEMP TODO _DURABLE : use mongommf for datafiles" << endl;
            return p;
        }

        for( std::map<void*,MongoMMF*>::iterator i = our_read_views.begin(); i != our_read_views.end(); i++ ) { 
            char *wl = (char *) i->second->_view_private;
            char *wh = wl + i->second->length();
            if( p >= wl && p < wh ) { 
                log() << "dur: perf warning p=" << p << " is already in the writable view of " << i->second->filename() << endl;
                return p;
            }
        }
        log() << "switchToPrivateView error " << p << endl;
        assert( false ); // did you call writing() with a pointer that isn't into a datafile?
        return 0;
    }
#endif

    /* switch to _view_write.  normally, this is a bad idea since your changes will not 
       show up in _view_private if there have been changes there; thus the leading underscore
       as a tad of a "warning".  but useful when done with some care, such as during 
       initialization.
    */
    /*static*/ void* MongoMMF::_switchToWritableView(void *p) { 
        RARELY log() << "todo dur not done switchtowritable" << endl;
#if defined(_DEBUG)
        return switchToPrivateView(p);
#else
        return p;
#endif
    }

    bool MongoMMF::open(string fname, bool sequentialHint) {
        _view_write = mapWithOptions(fname.c_str(), sequentialHint ? SEQUENTIAL : 0);
        // temp : _view_private pending more work!
        _view_private = _view_write;
        if( _view_write ) { 
             if( durable ) {
#if defined(_DEBUG)
                 _view_readonly = MemoryMappedFile::createReadOnlyMap();
                 mutex::scoped_lock lk(our_views_mutex);
                 our_read_views[_view_readonly] = this; 
#endif
             }
            return true;
        }
        return false;
    }

    bool MongoMMF::create(string fname, unsigned long long& len, bool sequentialHint) { 
        _view_write = map(fname.c_str(), len, sequentialHint ? SEQUENTIAL : 0);
        // temp : _view_private pending more work!
        _view_private = _view_write;
        if( _view_write ) {
            if( durable ) {
#if defined(_DEBUG)
                _view_readonly = MemoryMappedFile::createReadOnlyMap();
                mutex::scoped_lock lk(our_views_mutex);
                our_read_views[_view_readonly] = this;
#endif
            }
            return true;
        }
        return false;
    }
    
    /* we will re-map the private few frequently, thus the use of MoveableBuffer */
    MoveableBuffer MongoMMF::getView() { 
        if( durable && debug )
            return _view_readonly;
        return _view_private;
    }

    MongoMMF::MongoMMF() {
        _view_write = _view_private = _view_readonly = 0; 
    }

    MongoMMF::~MongoMMF() { 
        close();
    }

    /*virtual*/ void MongoMMF::close() {
#if defined(_DEBUG) && defined(_DURABLE)
        {
            mutex::scoped_lock lk(our_views_mutex);
            our_read_views.erase(_view_readonly);
        }
#endif
        _view_write = _view_private = _view_readonly = 0;
        MemoryMappedFile::close();
    }

}
