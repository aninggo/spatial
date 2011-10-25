// -*- C++ -*-
//
// Copyright Sylvain Bougerel 2009 - 2012.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file COPYING or copy at
// http://www.boost.org/LICENSE_1_0.txt)

/**
 *  @file   frozen_boxset.hpp
 *  @brief  Contains the definition of the @ref frozen_boxset and @ref
 *  runtime_frozen_boxset containers. These containers are not mapped containers
 *  and store values in space that can be represented as boxes.
 *
 *  Iterating these containers always yield a constant value iterator. That is
 *  because modifying the value stored in the container may compromise the
 *  ordering in the container. One way around this issue is to use a @ref
 *  frozen_boxmap container or to @c const_cast the value dereferenced from the
 *  iterator.
 *
 *  @see frozen_boxmap
 *  @see runtime_frozen_boxmap
 *  @see frozen_boxset
 *  @see runtime_frozen_boxset
 */

#ifndef SPATIAL_FROZEN_BOXSET_HPP
#define SPATIAL_FROZEN_BOXSET_HPP

#include "bits/spatial.hpp"
#include "bits/spatial_kdtree.hpp"

namespace spatial
{

  template<dimension_type Rank, typename Key,
           typename Compare = bracket_less<Key>,
           typename Alloc = std::allocator<Key> >
  struct frozen_boxset
    : details::Kdtree<details::Static_rank<Rank << 1>, Key, Key,
                      Compare, Alloc>
  {
  private:
    typedef details::Kdtree<details::Static_rank<Rank << 1>, Key, Key,
                            Compare, Alloc>          base_type;
    typedef frozen_boxset<Rank, Key, Compare, Alloc> Self;

  public:
    frozen_boxset() { }

    explicit frozen_boxset(const Compare& compare)
      : base_type(details::Static_rank<Rank << 1>())
    { }

    frozen_boxset(const Compare& compare, const Alloc& alloc)
      : base_type(details::Static_rank<Rank << 1>(), compare, alloc)
    { }

    frozen_boxset(const frozen_boxset& other, bool balancing = false)
      : base_type(other, balancing)
    { }

    frozen_boxset&
    operator=(const frozen_boxset& other)
    {
      return static_cast<Self&>(base_type::operator=(other));
    }
  };

  /**
   *  Specialization for @ref frozen_boxset with runtime rank support. The
   *  rank of the @ref frozen_boxset can be determined at run time and does not
   *  need to be fixed at compile time. Using:
   *  @code
   *    struct box { ... };
   *    frozen_boxset<0, box> my_set;
   *  @endcode
   *  ...is therefore completely equivalent to:
   *  @code
   *    struct box { ... };
   *    runtime_frozen_boxset<box> my_set;
   *  @endcode
   *
   *  @see runtime_frozen_boxset for more information about how to use this
   *  container.
   */
  template<typename Key, typename Compare, typename Alloc>
  struct frozen_boxset<0, Key, Compare, Alloc>
    : details::Kdtree<details::Dynamic_rank, Key, Key, Compare, Alloc>
  {
  private:
    typedef details::Kdtree<details::Dynamic_rank, Key, Key,
                            Compare, Alloc>          base_type;
    typedef frozen_boxset<0, Key, Compare, Alloc>    Self;

  public:
    frozen_boxset() : base_type(details::Dynamic_rank(2)) { }

    explicit frozen_boxset(dimension_type dim)
      : base_type(details::Dynamic_rank(dim << 1))
    { except::check_rank_argument(dim); }

    frozen_boxset(dimension_type dim, const Compare& compare)
      : base_type(details::Dynamic_rank(dim << 1), compare)
    { except::check_rank_argument(dim); }

    explicit frozen_boxset(const Compare& compare)
      : base_type(details::Dynamic_rank(2), compare)
    { }

    frozen_boxset(dimension_type dim, const Compare& compare,
                  const Alloc& alloc)
      : base_type(details::Dynamic_rank(dim << 1), compare, alloc)
    { except::check_rank_argument(dim); }

    frozen_boxset(const Compare& compare, const Alloc& alloc)
      : base_type(details::Dynamic_rank(2), compare, alloc)
    { }

    frozen_boxset(const frozen_boxset& other, bool balancing = false)
      : base_type(other, balancing)
    { }

    frozen_boxset&
    operator=(const frozen_boxset& other)
    {
      return static_cast<Self&>(base_type::operator=(other));
    }
  };

  /**
   *  @brief  boxset with dynamic rank support. The rank of the boxset can
   *  be determined at run time and does not need to be fixed at compile time.
   */
  template<typename Key,
           typename Compare = bracket_less<Key>,
           typename Alloc = std::allocator<Key> >
  struct runtime_frozen_boxset
    : details::Kdtree<details::Dynamic_rank, Key, Key, Compare, Alloc>
  {
  private:
    typedef details::Kdtree<details::Dynamic_rank, Key, Key,
                            Compare, Alloc>              base_type;
    typedef runtime_frozen_boxset<Key, Compare, Alloc>   Self;

  public:
    runtime_frozen_boxset() : base_type(details::Dynamic_rank(2)) { }

    explicit runtime_frozen_boxset(dimension_type dim)
      : base_type(details::Dynamic_rank(dim << 1))
    { except::check_rank_argument(dim); }

    runtime_frozen_boxset(dimension_type dim, const Compare& compare)
      : base_type(details::Dynamic_rank(dim << 1), compare)
    { except::check_rank_argument(dim); }

    explicit runtime_frozen_boxset(const Compare& compare)
      : base_type(details::Dynamic_rank(2), compare)
    { }

    runtime_frozen_boxset(dimension_type dim, const Compare& compare,
                          const Alloc& alloc)
      : base_type(details::Dynamic_rank(dim << 1), compare, alloc)
    { except::check_rank_argument(dim); }

    runtime_frozen_boxset(const Compare& compare, const Alloc& alloc)
      : base_type(details::Dynamic_rank(2), compare, alloc)
    { }

    runtime_frozen_boxset(const runtime_frozen_boxset& other,
                          bool balancing = false)
      : base_type(other, balancing)
    { }

    runtime_frozen_boxset&
    operator=(const runtime_frozen_boxset& other)
    {
      return static_cast<Self&>(base_type::operator=(other));
    }
  };

} // namespace spatial

#endif // SPATIAL_FROZEN_BOXSET_HPP
