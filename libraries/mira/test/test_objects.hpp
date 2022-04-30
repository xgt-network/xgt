#pragma once

#include <chainbase/chainbase.hpp>

#include <fc/reflect/reflect.hpp>
#include <fc/reflect/variant.hpp>

namespace fc
{

template<typename T>
void to_variant( const chainbase::oid<T>& var,  variant& vo )
{
   vo = var._id;
}

template<typename T>
void from_variant( const variant& vo, chainbase::oid<T>& var )
{
   var._id = vo.as_int64();
}

template< typename T >
struct get_typename< chainbase::oid< T > >
{
   static const char* name()
   {
      static std::string n = std::string( "chainbase::oid<" ) + get_typename< T >::name() + ">";
      return n.c_str();
   }
};

namespace raw
{

template<typename Stream, typename T>
void pack( Stream& s, const chainbase::oid<T>& id )
{
   s.write( (const char*)&id._id, sizeof(id._id) );
}

template<typename Stream, typename T>
void unpack( Stream& s, chainbase::oid<T>& id )
{
   s.read( (char*)&id._id, sizeof(id._id));
}

} }

#include <xgt/protocol/fixed_string.hpp>

#include <mira/ordered_index.hpp>
#include <mira/tag.hpp>
#include <mira/member.hpp>
#include <mira/indexed_by.hpp>
#include <mira/composite_key.hpp>
#include <mira/mem_fun.hpp>

enum test_object_type
{
   book_object_type,
   single_index_object_type,
   test_object_type,
   test_object2_type,
   test_object3_type,
   wallet_object_type
};

struct book : public chainbase::object< book_object_type, book > {
   book() = default;

   id_type id;
   int a = 0;
   int b = 1;

   int sum()const { return a + b; }
};

struct by_id;
struct by_a;
struct by_b;
struct by_sum;

typedef mira::multi_index_adapter<
   book,
   mira::multi_index::indexed_by<
      mira::multi_index::ordered_unique< mira::multi_index::tag< by_id >, mira::multi_index::member< book, book::id_type, &book::id > >,
      mira::multi_index::ordered_unique< mira::multi_index::tag< by_a >,  mira::multi_index::member< book, int,           &book::a  > >,
      mira::multi_index::ordered_unique< mira::multi_index::tag< by_b >,
         mira::multi_index::composite_key< book,
            mira::multi_index::member< book, int, &book::b >,
            mira::multi_index::member< book, int, &book::a >
         >,
         mira::multi_index::composite_key_compare< std::greater< int >, std::less< int > >
      >,
      mira::multi_index::ordered_unique< mira::multi_index::tag< by_sum >, mira::multi_index::const_mem_fun< book, int, &book::sum > >
  >
> book_index;

struct single_index_object : public chainbase::object< single_index_object_type, single_index_object >
{
   single_index_object() = default;

   id_type id;
};

typedef mira::multi_index_adapter<
   single_index_object,
   mira::multi_index::indexed_by<
      mira::multi_index::ordered_unique< mira::multi_index::tag< by_id >, mira::multi_index::member< single_index_object, single_index_object::id_type, &single_index_object::id > >
   >
> single_index_index;

struct test_object : public chainbase::object< test_object_type, test_object >
{
   id_type id;
   uint32_t val;
   std::string name;
};

struct ordered_idx;
struct composited_ordered_idx;

typedef mira::multi_index_adapter<
   test_object,
   mira::multi_index::indexed_by<
      mira::multi_index::ordered_unique< mira::multi_index::tag< ordered_idx >, mira::multi_index::member< test_object, chainbase::oid< test_object >, &test_object::id > >,
      mira::multi_index::ordered_unique< mira::multi_index::tag< composited_ordered_idx >,
         mira::multi_index::composite_key< test_object,
            mira::multi_index::member< test_object, std::string, &test_object::name >,
            mira::multi_index::member< test_object, uint32_t, &test_object::val >
         >
      >
   >
> test_object_index;

struct test_object2 : public chainbase::object< test_object2_type, test_object2 >
{
   test_object2() = default;

   id_type id;
   uint32_t val;
};

struct ordered_idx2;
struct composite_ordered_idx2;

typedef mira::multi_index_adapter<
   test_object2,
   mira::multi_index::indexed_by<
      mira::multi_index::ordered_unique< mira::multi_index::tag< ordered_idx2 >, mira::multi_index::member< test_object2, chainbase::oid< test_object2 >, &test_object2::id > >,
      mira::multi_index::ordered_unique< mira::multi_index::tag< composite_ordered_idx2 >,
         mira::multi_index::composite_key< test_object2,
            mira::multi_index::member< test_object2, uint32_t, &test_object2::val >,
            mira::multi_index::member< test_object2, chainbase::oid<test_object2>, &test_object2::id >
         >
      >
   >
> test_object2_index;

struct test_object3 : public chainbase::object< test_object3_type, test_object3 >
{
   test_object3() = default;

   id_type id;
   uint32_t val;
   uint32_t val2;
   uint32_t val3;
};

struct ordered_idx3;
struct composite_ordered_idx3a;
struct composite_ordered_idx3b;

typedef mira::multi_index_adapter<
   test_object3,
   mira::multi_index::indexed_by<
      mira::multi_index::ordered_unique< mira::multi_index::tag< ordered_idx3 >, mira::multi_index::member< test_object3, chainbase::oid< test_object3 >, &test_object3::id > >,
      mira::multi_index::ordered_unique< mira::multi_index::tag< composite_ordered_idx3a >,
         mira::multi_index::composite_key< test_object3,
            mira::multi_index::member< test_object3, uint32_t, &test_object3::val >,
            mira::multi_index::member< test_object3, uint32_t, &test_object3::val2 >
         >
      >,
      mira::multi_index::ordered_unique< mira::multi_index::tag< composite_ordered_idx3b >,
         mira::multi_index::composite_key< test_object3,
            mira::multi_index::member< test_object3, uint32_t, &test_object3::val >,
            mira::multi_index::member< test_object3, uint32_t, &test_object3::val3 >
         >
      >
   >
> test_object3_index;


typedef xgt::protocol::fixed_string<16> wallet_name_type;

struct wallet_object : public chainbase::object< wallet_object_type, wallet_object >
{
   wallet_object() = default;

   id_type id;
   wallet_name_type name;
};

struct by_name;

typedef mira::multi_index_adapter<
   wallet_object,
   mira::multi_index::indexed_by<
      mira::multi_index::ordered_unique< mira::multi_index::tag< by_id >, mira::multi_index::member< wallet_object, wallet_object::id_type, &wallet_object::id > >,
      mira::multi_index::ordered_unique< mira::multi_index::tag< by_name >, mira::multi_index::member< wallet_object, wallet_name_type, &wallet_object::name > >
   >
> wallet_index;

FC_REFLECT( book::id_type, (_id) )
FC_REFLECT( book, (id)(a)(b) )
CHAINBASE_SET_INDEX_TYPE( book, book_index )

FC_REFLECT( single_index_object::id_type, (_id) )
FC_REFLECT( single_index_object, (id) )
CHAINBASE_SET_INDEX_TYPE( single_index_object, single_index_index )

FC_REFLECT( test_object::id_type, (_id) )
FC_REFLECT( test_object, (id)(val)(name) )
CHAINBASE_SET_INDEX_TYPE( test_object, test_object_index )

FC_REFLECT( test_object2::id_type, (_id) )
FC_REFLECT( test_object2, (id)(val) )
CHAINBASE_SET_INDEX_TYPE( test_object2, test_object2_index )

FC_REFLECT( test_object3::id_type, (_id) )
FC_REFLECT( test_object3, (id)(val)(val2)(val3) )
CHAINBASE_SET_INDEX_TYPE( test_object3, test_object3_index )

FC_REFLECT( wallet_object::id_type, (_id) )
FC_REFLECT( wallet_object, (id)(name) )
CHAINBASE_SET_INDEX_TYPE( wallet_object, wallet_index )
