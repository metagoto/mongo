// @file bufreader.h parse a memory region into usable pieces

/**
*    Copyright (C) 2009 10gen Inc.
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

#pragma once

namespace mongo { 

    /** helper to read and parse a block of memory 
        methods throw the eof exception if the operation would pass the end of the
        buffer with which we are working.
    */
    class BufReader : boost::noncopyable { 
    public:
        class eof : public std::exception { 
        public:
            virtual const char * what() { return "BufReader eof"; }
        };

        BufReader(void *p, unsigned len) : _start(p), _pos(p), _end(((char *)_pos)+len) { }

        bool atEof() const { return _pos == _end; }
        
        /** read in the object specified, and advance buffer pointer */
        template <typename T>
        void read(T &t) { 
            T* cur = (T*) _pos;
            T *next = cur + 1;
            if( _end < next ) throw eof();
            t = *cur;
            _pos = next;
        }

        /** verify we can look at t, but do not advance */
        template <typename T>
        void peek(T &t) { 
            T* cur = (T*) _pos;
            T *next = cur + 1;
            if( _end < next ) throw eof();
            t = *cur;
        }

        /** return current offset into buffer */
        unsigned offset() const { return (char*)_pos - (char*)_start; }

        /** return current position pointer, and advance by len */
        const void* skip(unsigned len) { 
            char *nxt = ((char *) _pos) + len;
            if( _end < nxt ) throw eof();
            void *p = _pos;
            _pos = nxt;
            return p;
        }

        void readStr(string& s) {
            StringBuilder b;
            while( 1 ) { 
                char ch;
                read(ch);
                if( ch == 0 )
                    break;
                b << ch;
            }
            s = b.str();
        }

        /** skip ahead to our alignment boundary */
        void align(unsigned alignment) {
            size_t ofs = ((char*)_pos) - ((char*)_start);
            ofs = (ofs + (alignment-1)) & (~(alignment-1));
            _pos = ((char*)_start) + ofs;
        }

        void* pos() { return _pos; }

    private:
        void *_start;
        void *_pos;
        void *_end;
    };

}
