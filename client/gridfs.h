/** @file gridfs.h */

#pragma once

#include "dbclient.h"

namespace mongo {

    typedef unsigned long long gridfs_offset;

    class GridFS;
    class GridFile;

    class Chunk {
    public:
        Chunk( BSONObj data );
        Chunk( BSONObj fileId , int chunkNumber , const char * data , int len );

        int len(){
            int len;
            const char * data = _data["data"].binData( len );
            int * foo = (int*)data;
            assert( len - 4 == foo[0] );
            return len - 4;
        }

        const char * data( int & len ){
            const char * data = _data["data"].binData( len );
            int * foo = (int*)data;
            assert( len - 4 == foo[0] );

            len = len - 4;
            return data + 4;
        }

    private:
        BSONObj _data;
        friend class GridFS;
    };


    /**
       this is the main entry point into the mongo grid fs
     */
    class GridFS{
    public:
        /**
         * @param client - db connection
         * @param dbName - root database name
         * @param prefix - if you want your data somewhere besides <dbname>.fs
         */
        GridFS( DBClientBase& client , const std::string& dbName , const std::string& prefix="fs" );
        ~GridFS();

        /**
         * puts the file reference by fileName into the db
         * @param fileName local filename relative to process
         * @param remoteName optional filename to use for file stored in GridFS
         *                   (default is to use fileName parameter)
         * @param contentType optional MIME type for this object.
         *                    (default is to omit)
         * @return the file object
         */
        BSONObj storeFile( const std::string& fileName , const std::string& remoteName="" , const std::string& contentType="");

        /**
         * puts the file represented by data into the db
         * @param data pointer to buffer to store in GridFS
         * @param length length of buffer
         * @param remoteName optional filename to use for file stored in GridFS
         *                   (default is to use fileName parameter)
         * @param contentType optional MIME type for this object.
         *                    (default is to omit)
         * @return the file object
         */
        BSONObj storeFile( const char* data , size_t length , const std::string& remoteName , const std::string& contentType);
        /**
         * removes file referenced by fileName from the db
         * @param fileName filename (in GridFS) of the file to remove
         * @return the file object
         */
        void removeFile( const std::string& fileName );

        /**
         * returns a file object matching the query
         */
        GridFile findFile( BSONObj query );

        /**
         * equiv to findFile( { filename : filename } )
         */
        GridFile findFile( const std::string& fileName );

        /**
         * convenience method to get all the files
         */
        std::auto_ptr<DBClientCursor> list();

        /**
         * convenience method to get all the files with a filter
         */
        std::auto_ptr<DBClientCursor> list( BSONObj query );

    private:
        DBClientBase& _client;
        std::string _dbName;
        std::string _prefix;
        std::string _filesNS;
        std::string _chunksNS;

        // insert fileobject. All chunks must be in DB.
        BSONObj insertFile(const std::string& name, const OID& id, unsigned length, const std::string& contentType);

        friend class GridFile;
    };

    /**
       wrapper for a file stored in the Mongo database
     */
    class GridFile {
    public:
        /**
         * @return whether or not this file exists
         * findFile will always return a GriFile, so need to check this
         */
        bool exists(){
            return ! _obj.isEmpty();
        }

        std::string getFilename(){
            return _obj["filename"].str();
        }

        int getChunkSize(){
            return (int)(_obj["chunkSize"].number());
        }

        gridfs_offset getContentLength(){
            return (gridfs_offset)(_obj["length"].number());
        }

        unsigned long long getUploadDate(){
            return _obj["uploadDate"].date();
        }

        std::string getMD5(){
            return _obj["md5"].str();
        }

        BSONObj getMetadata();

        int getNumChunks(){
            return (int) ceil( (double)getContentLength() / (double)getChunkSize() );
        }

        Chunk getChunk( int n );

        /**
           write the file to the output stream
         */
        gridfs_offset write( std::ostream & out );

        /**
           write the file to this filename
         */
        gridfs_offset write( const std::string& where );

    private:
        GridFile( GridFS * grid , BSONObj obj );

        void _exists();

        GridFS * _grid;
        BSONObj _obj;

        friend class GridFS;
    };
}


