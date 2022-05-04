#include <xgt/chain/block_log.hpp>
#include <fstream>
#include <fc/io/raw.hpp>

#include <fc/thread/mutex.hpp>
#include <fc/thread/scoped_lock.hpp>


#define LOG_WRITE (std::ios::in | std::ios::out | std::ios::binary | std::ios::app | std::ios::ate)

namespace xgt { namespace chain {

   typedef fc::scoped_lock<fc::mutex> scoped_lock;

   namespace detail {
      class block_log_impl {
         public:
            optional< signed_block > head;
            block_id_type            head_id;
            std::fstream             block_stream;
            std::fstream             index_stream;
            fc::path                 block_file;
            fc::path                 index_file;

            fc::mutex             mtx;
      };
   }

   block_log::block_log()
   :my( new detail::block_log_impl() )
   {

   }

   block_log::~block_log()
   {
      flush();
   }

   void block_log::open( const fc::path& file )
   {
      if( my->block_stream.is_open() )
         my->block_stream.close();
      if( my->index_stream.is_open() )
         my->index_stream.close();

      my->block_file = file;
      my->index_file = fc::path( file.generic_string() + ".index" );

      my->block_stream.open( my->block_file.generic_string().c_str(), LOG_WRITE );
      my->index_stream.open( my->index_file.generic_string().c_str(), LOG_WRITE );

      my->block_stream.exceptions( std::fstream::failbit | std::fstream::badbit );
      my->index_stream.exceptions( std::fstream::failbit | std::fstream::badbit );

      /* On startup of the block log, there are several states the log file and the index file can be
       * in relation to eachother.
       *
       *                          Block Log
       *                     Exists       Is New
       *                 +------------+------------+
       *          Exists |    Check   |   Delete   |
       *   Index         |    Head    |    Index   |
       *    File         +------------+------------+
       *          Is New |   Replay   |     Do     |
       *                 |    Log     |   Nothing  |
       *                 +------------+------------+
       *
       * Checking the heads of the files has several conditions as well.
       *  - If they are the same, do nothing.
       *  - If the index file head is not in the log file, delete the index and replay.
       *  - If the index file head is in the log, but not up to date, replay from index head.
       */
      auto log_size = fc::file_size( my->block_file );
      auto index_size = fc::file_size( my->index_file );

      if( log_size )
      {
         ilog( "Log is nonempty" );
         my->head = read_head();
         my->head_id = my->head->id();

         if( index_size )
         {
            ilog( "Index is nonempty" );
            uint64_t block_pos;
            my->block_stream.seekg( -sizeof( uint64_t), std::ios::end );
            my->block_stream.read( (char*)&block_pos, sizeof( block_pos ) );

            uint64_t index_pos;
            my->index_stream.seekg( -sizeof( uint64_t), std::ios::end );
            my->index_stream.read( (char*)&index_pos, sizeof( index_pos ) );

            if( block_pos < index_pos )
            {
               ilog( "block_pos < index_pos, close and reopen index_stream" );
               construct_index();
            }
            else if( block_pos > index_pos )
            {
               ilog( "Index is incomplete" );
               construct_index();
            }
         }
         else
         {
            ilog( "Index is empty" );
            construct_index();
         }
      }
      else if( index_size )
      {
         ilog( "Index is nonempty, remove and recreate it" );
         my->index_stream.close();
         fc::remove_all( my->index_file );
         my->index_stream.open( my->index_file.generic_string().c_str(), LOG_WRITE );
      }
   }

   void block_log::close()
   {
      my.reset( new detail::block_log_impl() );
   }

   bool block_log::is_open()const
   {
      return my->block_stream.is_open();
   }

   uint64_t block_log::append( const signed_block& b )
   {
      try
      {
         scoped_lock lock(my->mtx);

         my->block_stream.seekg(0, std::ios::end);
         my->index_stream.seekg(0, std::ios::end);

         uint64_t pos = my->block_stream.tellp();
         FC_ASSERT( static_cast<uint64_t>(my->index_stream.tellp()) == sizeof( uint64_t ) * ( b.block_num() - 1 ),
            "Append to index file occuring at wrong position.",
            ( "position", (uint64_t) my->index_stream.tellp() )( "expected",( b.block_num() - 1 ) * sizeof( uint64_t ) ) );
         auto data = fc::raw::pack_to_vector( b );
         my->block_stream.write( data.data(), data.size() );
         my->block_stream.write( (char*)&pos, sizeof( pos ) );
         my->index_stream.write( (char*)&pos, sizeof( pos ) );
         my->head = b;
         my->head_id = b.id();

         return pos;
      }
      FC_LOG_AND_RETHROW()
   }

   void block_log::flush()
   {
      scoped_lock lock(my->mtx);

      my->block_stream.flush();
      my->index_stream.flush();
   }

   std::pair< signed_block, uint64_t > block_log::read_block( uint64_t pos )const
   {
      scoped_lock lock(my->mtx);

      return read_block_helper( pos );
   }

   std::pair< signed_block, uint64_t > block_log::read_block_helper( uint64_t pos )const
   {
      try
      {
         my->block_stream.seekg( pos );
         std::pair<signed_block,uint64_t> result;
         fc::raw::unpack( my->block_stream, result.first );
         result.second = uint64_t(my->block_stream.tellg()) + 8;
         return result;
      }
      FC_LOG_AND_RETHROW()
   }

   optional< signed_block > block_log::read_block_by_num( uint32_t block_num )const
   {
      try
      {
         scoped_lock lock(my->mtx);

         optional< signed_block > b;
         uint64_t pos = get_block_pos_helper( block_num );
         if( pos != npos )
         {
            b = read_block_helper( pos ).first;
            FC_ASSERT( b->block_num() == block_num , "Wrong block was read from block log.", ( "returned", b->block_num() )( "expected", block_num ));
         }
         return b;
      }
      FC_LOG_AND_RETHROW()
   }

   uint64_t block_log::get_block_pos( uint32_t block_num ) const
   {
      scoped_lock lock(my->mtx);

      return get_block_pos_helper( block_num );
   }

   uint64_t block_log::get_block_pos_helper( uint32_t block_num ) const
   {
      try
      {
         if( !( my->head.valid() && block_num <= protocol::block_header::num_from_id( my->head_id ) && block_num > 0 ) )
            return npos;
         my->index_stream.seekg( sizeof( uint64_t ) * ( block_num - 1 ) );
         uint64_t pos;
         my->index_stream.read( (char*)&pos, sizeof( pos ) );
         return pos;
      }
      FC_LOG_AND_RETHROW()
   }

   signed_block block_log::read_head()const
   {
      try
      {
         scoped_lock lock(my->mtx);

         uint64_t pos;
         my->block_stream.seekg( -sizeof(pos), std::ios::end );
         my->block_stream.read( (char*)&pos, sizeof(pos) );
         return read_block_helper( pos ).first;
      }
      FC_LOG_AND_RETHROW()
   }

   const optional< signed_block >& block_log::head()const
   {
      scoped_lock lock(my->mtx);

      return my->head;
   }

   void block_log::construct_index()
   {
      try
      {
         ilog( "Reconstructing Block Log Index..." );
         my->index_stream.close();
         fc::remove_all( my->index_file );
         my->index_stream.open( my->index_file.generic_string().c_str(), LOG_WRITE );

         uint64_t pos = 0;
         uint64_t end_pos = 0;

         my->block_stream.clear();

         my->block_stream.seekg( -(sizeof(uint64_t)), std::ios::end );
         my->block_stream.read( (char*)(&end_pos), sizeof(uint64_t) );

         FC_ASSERT(my->block_stream.gcount() == sizeof(uint64_t), "failed to read end pos");

         signed_block tmp;

         my->block_stream.seekg( pos );

         FC_ASSERT(fc::file_size(my->block_file.generic_string().c_str()) >= end_pos, "invalid block log end post, is block_log corrupt?");

         while( pos < end_pos )
         {
            fc::raw::unpack( my->block_stream, tmp );
            my->block_stream.read( (char*)&pos, sizeof( pos ) );
            my->index_stream.write( (char*)&pos, sizeof( pos ) );
         }
      }
      FC_LOG_AND_RETHROW()
   }
} } // xgt::chain
