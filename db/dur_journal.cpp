// @file dur_journal.cpp

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

#include "pch.h"
#include "dur_journal.h"

#if !defined(_DURABLE)

namespace mongo {
    namespace dur {
    void openJournal() { }
    }
}

#else

#include "../util/logfile.h"

namespace mongo {
    namespace dur {

        /* temp - this will become more elaborate and probably a class. */


        /** call at init.  uasserts on failure.  if fails, you likely want to terminate. */
        void openJournal() { 

        }

    }
}

#endif
