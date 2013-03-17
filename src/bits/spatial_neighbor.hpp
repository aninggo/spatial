// -*- C++ -*-
//
// Copyright Sylvain Bougerel 2009 - 2013.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file COPYING or copy at
// http://www.boost.org/LICENSE_1_0.txt)

/**
 *  @file   spatial_neighbor.hpp
 *  @brief  Contains the definition of the neighbor iterators. These iterators
 *  walk through all items in the container in order from the closest to the
 *  furthest away from a given key.
 *
 *  \see neighbor
 */

#ifndef SPATIAL_NEIGHBOR_HPP
#define SPATIAL_NEIGHBOR_HPP

#ifndef SPATIAL_HPP
#  error "Do not include this file directly in your project."
#endif

#include "spatial_metric.hpp"

namespace spatial
{
  namespace details
  {
    /**
     *  Extra information needed by the iterator to perform its work. This
     *  information is copied to each iterator from a given container.
     *
     *  Although it may be possible to modify this information directly from
     *  it's members, it may be unwise to do so, as it could invalidate the
     *  iterator and cause the program to behave unexpectedly. If any of this
     *  information needs to be modified, it is probably recommended to create a
     *  new iterator altogether.
     *
     *  \tparam Ct The container to which these iterator relate to.
     *  \tparam Metric The type of metric applied to the iterator.
     *  \tparam DistanceType The type used to store the distance value.
     */
    template<typename Ct, typename Metric>
    struct Neighbor_data : container_traits<Ct>::key_compare
    {
      //! Build an unintialized neighbor data object.
      Neighbor_data() { }

      /**
       *  Builds required neighbor data from the given container, metric and
       *  dimension.
       *
       *  \param c The container being iterated.
       *  \param g The metric to apply during iteration
       *  \param n The node dimension for the node pointed to by the iterator.
       */
      Neighbor_data
      (const typename container_traits<Ct>::key_compare& c, const Metric& g,
       const typename container_traits<Ct>::key_type& k)
        : container_traits<Ct>::key_compare(c), _target(g, k)
      { }

      /**
       *  The target of the iteration; element of the container are iterate
       *  from the closest to the element furthest away from the target.
       */
      Compress<Metric, typename container_traits<Ct>::key_type> _target;

      /**
       *  The last valid computed value of the distance. The value stored is
       *  only valid if the iterator is not past-the-end.
       */
      typename Metric::distance_type _distance;
    };
  } // namespace details

  /**
   *  A spatial iterator for a container \c Ct that goes through the nearest
   *  to the furthest element from a target key, with distances applied
   *  according to a user-defined geometric space of type \ref Metric.
   *
   *  \tparam Ct The container type bound to the iterator.
   *  \tparam DistanceType The type used to represent distances.
   *  \tparam Metric An type that follow the \ref Metric concept.
   *
   *  The Metric type is a complex type that must be a model of \ref
   *  Metric:
   *
   *  \code
   *  struct Metric
   *  {
   *    typedef DistanceType distance_type;
   *
   *    DistanceType
   *    distance_to_key(dimension_type rank,
   *                    const Key& origin, const Key& key) const;
   *
   *    DistanceType
   *    distance_to_plane(dimension_type rank, dimension_type dim,
   *                      const Key& origin, const Key& key) const;
   *  };
   *  \endcode
   *
   *  The details of the \c Metric type are explained in \ref Metric.
   *  The library provides ready-made models of \ref Metric such as \ref
   *  euclidian and \ref manhattan that are designed to work only with C++'s
   *  built-in arithmetic types. If more metrics needs to be defined, see the
   *  explanation in \ref Metric.
   */
  template <typename Ct, typename Metric =
            euclidian<typename details::mutate<Ct>::type,
                      double,
                      typename details::with_builtin_difference
                      <Ct, double>::type> >
  class neighbor_iterator
    : public details::Bidirectional_iterator
  <typename container_traits<Ct>::mode_type,
   typename container_traits<Ct>::rank_type>
  {
  private:
    typedef typename details::Bidirectional_iterator
    <typename container_traits<Ct>::mode_type,
     typename container_traits<Ct>::rank_type> Base;

  public:
    //! Key comparator type transferred from the container
    typedef typename container_traits<Ct>::key_compare key_compare;

    //! The metric type used by the iterator
    typedef Metric metric_type;

    //! The distance type that is read from metric_type
    typedef typename Metric::distance_type distance_type;

    //! The key type that is used as a target for the nearest neighbor search
    typedef typename container_traits<Ct>::key_type key_type;

    //! Uninitialized iterator.
    neighbor_iterator() { }

    /**
     *  The standard way to build this iterator: specify a metric to apply, an
     *  iterator on a container, and that container.
     *
     *  \param container The container to iterate.
     *  \param metric The metric applied during the iteration.
     *  \param target The target of the neighbor iteration.
     *  \param it An iterator on container.
     */
    neighbor_iterator(Ct& container_, const Metric& metric_,
                      const typename container_traits<Ct>::key_type& target_,
                      const typename container_traits<Ct>::iterator& iter_)
      : Base(container_.rank(), iter_.node,
             modulo(iter_.node, container_.rank())),
        _data(container_.key_comp(), metric_, target_) { }

    /**
     *  When the information of the dimension for the current node being
     *  pointed to by the iterator is known, this constructor saves some CPU
     *  cycle, by comparison to the other constructor.
     *
     *  In order to iterate through nodes in the \kdtree built in the
     *  container, the algorithm must know at each node which dimension is
     *  used to partition the space. Some algorithms will provide this
     *  dimension, such as the function \ref modulo().
     *
     *  \attention Specifying the incorrect dimension value for the node will
     *  result in unknown behavior. It is recommended that you do not use this
     *  constructor if you are not sure about this dimension, and use the
     *  other constructors instead.
     *
     *  \param container The container to iterate.
     *  \param metric The metric applied during the iteration.
     *  \param target The target of the neighbor iteration.
     *  \param node_dim The dimension of the node pointed to by iterator.
     *  \param node Use the value of node as the start point for the
     *  iteration.
     */
    neighbor_iterator(Ct& container_, const Metric& metric_,
                      const typename container_traits<Ct>::key_type& target_,
                      dimension_type node_dim_,
                      typename container_traits<Ct>::mode_type::node_ptr node_)
      : Base(container_.rank(), node_, node_dim_),
        _data(container_.key_comp(), metric_, target_) { }

    //! Increments the iterator and returns the incremented value. Prefer to
    //! use this form in \c for loops.
    neighbor_iterator<Ct, Metric>& operator++()
    { return increment_neighbor(*this); }

    //! Increments the iterator but returns the value of the iterator before
    //! the increment. Prefer to use the other form in \c for loops.
    neighbor_iterator<Ct, Metric> operator++(int)
    {
      neighbor_iterator<Ct, Metric> x(*this);
      increment_neighbor(*this);
      return x;
    }

    //! Decrements the iterator and returns the decremented value. Prefer to
    //! use this form in \c for loops.
    neighbor_iterator<Ct, Metric>& operator--()
    { return decrement_neighbor(*this); }

    //! Decrements the iterator but returns the value of the iterator before
    //! the decrement. Prefer to use the other form in \c for loops.
    neighbor_iterator<Ct, Metric> operator--(int)
    {
      neighbor_iterator<Ct, Metric> x(*this);
      decrement_neighbor(*this);
      return x;
    }

    //! Return the key_comparator used by the iterator
    key_compare
    key_comp() const { return static_cast<const key_compare&>(_data); }

    //! Return the metric used by the iterator
    metric_type
    metric() const { return _data._target.base(); }

    //! Read-only accessor to the last valid distance of the iterator
    const distance_type&
    distance() const { return _data._distance; }

    //! Read/write accessor to the last valid distance of the iterator
    distance_type&
    distance() { return _data._distance; }

    //! Read-only accessor to the target of the iterator
    const key_type&
    target_key() const { return _data._target(); }

    //! Read/write accessor to the target of the iterator
    key_type&
    target_key() { return _data._target(); }

  private:
    //! The related data for the iterator.
    details::Neighbor_data<Ct, Metric> _data;
  };

  /**
   *  A spatial iterator for a container \c Ct that goes through the nearest
   *  to the furthest element from a target key, with distances applied
   *  according to a user-defined geometric space of type \c Metric.
   *
   *  The Metric type is a complex type that must be a model of \ref
   *  Metric:
   *
   *  \code
   *  struct Metric
   *  {
   *    typedef my_distance_type distance_type;
   *
   *    distance_type
   *    distance_to_key(dimension_type rank,
   *                    const Key& origin, const Key& key) const;
   *
   *    distance_type
   *    distance_to_plane(dimension_type rank, dimension_type dim,
   *                      const Key& origin, const Key& key) const;
   *  };
   *  \endcode
   *
   *  The details of the \c Metric type are explained in \ref Metric.
   *  Metrics are generally not defined by the user of the library, given
   *  their complexity. Rather, the user of the library uses ready-made
   *  models of \ref Metric such as \ref euclid and \ref manhattan. If
   *  more metrics needs to be defined, see the explanation in \ref
   *  Metric.
   *
   *  This iterator only returns constant objects.
   *
   *  \tparam Ct The container type bound to the iterator.
   *  \tparam DistanceType The type used to represent distances.
   *  \tparam Metric An type that follow the \ref Metric concept.
   */
  template<typename Ct, typename Metric>
  class neighbor_iterator<const Ct, Metric>
    : public details::Const_bidirectional_iterator
  <typename container_traits<Ct>::mode_type,
   typename container_traits<Ct>::rank_type>
  {
  private:
    typedef typename details::Const_bidirectional_iterator
    <typename container_traits<Ct>::mode_type,
     typename container_traits<Ct>::rank_type> Base;

  public:
    //! Key comparator type transferred from the container
    typedef typename container_traits<Ct>::key_compare key_compare;

    //! The metric type used by the iterator
    typedef Metric metric_type;

    //! The distance type that is read from metric_type
    typedef typename Metric::distance_type distance_type;

    //! The key type that is used as a target for the nearest neighbor search
    typedef typename container_traits<Ct>::key_type key_type;

    //! Uninitialized iterator.
    neighbor_iterator() { }

    /**
     *  The standard way to build this iterator: specify a metric to apply,
     *  an iterator on a container, and that container.
     *
     *  \param container The container to iterate.
     *  \param metric The metric applied during the iteration.
     *  \param target The target of the neighbor iteration.
     *  \param iter An iterator on \c container.
     */
    neighbor_iterator(const Ct& container_, const Metric& metric_,
                      const typename container_traits<Ct>::key_type& target_,
                      typename container_traits<Ct>::iterator iter_)
      : Base(container_.rank(), iter_.node,
             modulo(iter_.node, container_.rank())),
        _data(container_.key_comp(), metric_, target_) { }

    /**
     *  When the information of the dimension for the current node being
     *  pointed to by the iterator is known, this constructor saves some CPU
     *  cycle, by comparison to the other constructor.
     *
     *  In order to iterate through nodes in the \kdtree built in the
     *  container, the algorithm must know at each node which dimension is
     *  used to partition the space. Some algorithms will provide this
     *  dimension, such as the function \ref modulo().
     *
     *  \attention Specifying the incorrect dimension value for the node will
     *  result in unknown behavior. It is recommended that you do not use this
     *  constructor if you are not sure about this dimension, and use the
     *  other constructors instead.
     *
     *  \param container_ The container to iterate.
     *  \param metric_ The metric applied during the iteration.
     *  \param target_ The target of the neighbor iteration.
     *  \param node_dim_ The dimension of the node pointed to by iterator.
     *  \param node_ Use the value of node as the start point for the
     *  iteration.
     */
    neighbor_iterator(const Ct& container_, const Metric& metric_,
                      const typename container_traits<Ct>::key_type& target_,
                      dimension_type node_dim_,
                      typename container_traits<Ct>::mode_type::node_ptr node_)
      : Base(container_.rank(), node_, node_dim_),
        _data(container_.key_comp(), metric_, target_) { }

    //! Covertion of mutable iterator into a constant iterator is permitted.
    neighbor_iterator(const neighbor_iterator<Ct, Metric>& iter)
      : Base(iter.rank(), iter.node, iter.node_dim),
        _data(iter.key_comp(), iter.metric(), iter.target_key()) { }

    //! Increments the iterator and returns the incremented value. Prefer to
    //! use this form in \c for loops.
    neighbor_iterator<Ct, Metric>& operator++()
    { return increment_neighbor(*this); }

    //! Increments the iterator but returns the value of the iterator before
    //! the increment. Prefer to use the other form in \c for loops.
    neighbor_iterator<Ct, Metric> operator++(int)
    {
      neighbor_iterator<Ct, Metric> x(*this);
      increment_neighbor(*this);
      return x;
    }

    //! Decrements the iterator and returns the decremented value. Prefer to
    //! use this form in \c for loops.
    neighbor_iterator<Ct, Metric>& operator--()
    { return decrement_neighbor(*this); }

    //! Decrements the iterator but returns the value of the iterator before
    //! the decrement. Prefer to use the other form in \c for loops.
    neighbor_iterator<Ct, Metric> operator--(int)
    {
      neighbor_iterator<Ct, Metric> x(*this);
      decrement_neighbor(*this);
      return x;
    }

    //! Return the key_comparator used by the iterator
    key_compare
    key_comp() const { return static_cast<const key_compare&>(_data); }

    //! Return the metric used by the iterator
    metric_type
    metric() const { return _data._target.base(); }

    //! Read-only accessor to the last valid distance of the iterator
    distance_type
    distance() const { return _data._distance; }

    //! Read/write accessor to the last valid distance of the iterator
    distance_type&
    distance() { return _data._distance; }

    //! Read-only accessor to the target of the iterator
    const key_type&
    target_key() const { return _data._target(); }

    //! Read/write accessor to the target of the iterator
    key_type&
    target_key() { return _data._target(); }

  private:
    //! The related data for the iterator.
    details::Neighbor_data<Ct, Metric> _data;
  };

  /**
   *  Read/write accessor for neighbor iterators that retrieve the valid
   *  calculated distance from the target. The distance read is only relevant if
   *  the iterator does not point past-the-end.
   */
  //@{
  template <typename Ct, typename Metric>
  inline typename Metric::distance_type
  distance(const neighbor_iterator<Ct, Metric>& iter)
  { return iter.distance(); }

  template <typename Ct, typename Metric>
  inline void
  distance(neighbor_iterator<Ct, Metric>& iter,
           const typename Metric::distance_type& dist)
  { iter.distance() = dist; }
  //@}

  /**
   *  A quick accessor for neighbor iterators that retrive the key that is the
   *  target for the nearest neighbor iteration.
   */
  template <typename Ct, typename Metric>
  inline const typename container_traits<Ct>::key_type&
  target_key(const neighbor_iterator<Ct, Metric>& iter)
  { return iter.target_key(); }

  /**
   *  This structure defines a pair of neighbor iterator.
   *
   *  \tparam Ct The container to which these iterator relate to.
   *  \tparam Metric The metric used by the iterator.
   *  \see mapping_iterator
   */
  template <typename Ct, typename Metric
            = euclidian<typename details::mutate<Ct>::type,
                        double, typename details::with_builtin_difference
                        <Ct, double>::type> >
  struct neighbor_iterator_pair
    : std::pair<neighbor_iterator<Ct, Metric>,
                neighbor_iterator<Ct, Metric> >
  {
    /**
     *  A pair of iterators that represents a range (that is: a range of
     *  elements to iterate, and not an orthogonal range).
     */
    typedef std::pair<neighbor_iterator<Ct, Metric>,
                      neighbor_iterator<Ct, Metric> > Base;

    //! Empty constructor.
    neighbor_iterator_pair() { }

    //! Regular constructor that builds a neighbor_iterator_pair out of 2
    //! neighbor_iterators.
    neighbor_iterator_pair(const neighbor_iterator<Ct, Metric>& a,
                           const neighbor_iterator<Ct, Metric>& b)
      : Base(a, b) { }
  };

  /**
   *  This structure defines a pair of constant neighbor iterator.
   *
   *  \tparam Ct The container to which these iterator relate to.
   *  \tparam Metric The metric used by the iterator.
   *  \see neighbor_iterator
   */
  template <typename Ct, typename Metric>
  struct neighbor_iterator_pair<const Ct, Metric>
    : std::pair<neighbor_iterator<const Ct, Metric>,
                neighbor_iterator<const Ct, Metric> >
  {
    /**
     *  A pair of iterators that represents a range (that is: a range of
     *  elements to iterate, and not an orthogonal range).
     */
    typedef std::pair<neighbor_iterator<const Ct, Metric>,
                      neighbor_iterator<const Ct, Metric> > Base;

    //! Empty constructor.
    neighbor_iterator_pair() { }

    //! Regular constructor that builds a neighbor_iterator_pair out of 2
    //! neighbor_iterators.
    neighbor_iterator_pair(const neighbor_iterator<const Ct, Metric>& a,
                           const neighbor_iterator<const Ct, Metric>& b)
      : Base(a, b)
    { }

    //! Convert a mutable neighbor iterator pair into a const neighbor iterator
    //!pair.
    neighbor_iterator_pair(const neighbor_iterator_pair<Ct, Metric>& p)
      : Base(p.first, p.second) { }
  };

  namespace details
  {
    template <typename Container, typename Metric>
    neighbor_iterator<Container, Metric>&
    increment_neighbor(neighbor_iterator<Container, Metric>& iter);

    template <typename Container, typename Metric>
    neighbor_iterator<Container, Metric>&
    decrement_neighbor(neighbor_iterator<Container, Metric>& iter);

    template <typename Container, typename Metric>
    neighbor_iterator<Container, Metric>&
    minimum_neighbor(neighbor_iterator<Container, Metric>& iter);

    template <typename Container, typename Metric>
    neighbor_iterator<Container, Metric>&
    maximum_neighbor(neighbor_iterator<Container, Metric>& iter);

    template <typename Container, typename Metric>
    neighbor_iterator<Container, Metric>&
    lower_bound_neighbor(neighbor_iterator<Container, Metric>& iter,
                         const typename Metric::distance_type bound);

    template <typename Container, typename Metric>
    neighbor_iterator<Container, Metric>&
    upper_bound_neighbor(neighbor_iterator<Container, Metric>& iter,
                         const typename Metric::distance_type& bound);

  } // namespace details

  /**
   *  Build a past-the-end neighbor iterator with a user-defined \ref Metric.
   *  \param container The container in which a neighbor must be found.
   *  \param metric The metric to use in search of the neighbor.
   *  \param target The target key used in the neighbor search.
   */
  //@{
  template <typename Ct, typename Metric>
  inline neighbor_iterator<Ct, Metric>
  neighbor_end(Ct& container, const Metric& metric,
               const typename container_traits<Ct>::key_type& target)
  {
    return neighbor_iterator<Ct, Metric>
      (container, metric, target, container.dimension() - 1,
       container.end().node);
  }

  template <typename Ct, typename Metric>
  inline neighbor_iterator<const Ct, Metric>
  neighbor_end(const Ct& container, const Metric& metric,
               const typename container_traits<Ct>::key_type& target)
  {
    return neighbor_iterator<const Ct, Metric>
      (container, metric, target, container.dimension() - 1,
       container.end().node);
  }

  template <typename Ct, typename Metric>
  inline neighbor_iterator<const Ct, Metric>
  neighbor_cend(const Ct& container, const Metric& metric,
                const typename container_traits<Ct>::key_type& target)
  { return neighbor_end(container, metric, target); }
  //@}

  /**
   *  Build a past-the-end neighbor iterator, assuming an euclidian metric with
   *  distances expressed in double. It requires that the container used was
   *  defined with a built-in key compare functor.
   *  \param container The container in which a neighbor must be found.
   *  \param target The target key used in the neighbor search.
   */
  //@{
  template <typename Ct>
  inline typename enable_if<details::is_compare_builtin<Ct>,
                            neighbor_iterator<Ct> >::type
  neighbor_end(Ct& container,
               const typename container_traits<Ct>::key_type& target)
  {
    return neighbor_end
      (container,
       euclidian<Ct, double,
                 typename details::with_builtin_difference<Ct, double>::type>
         (details::with_builtin_difference<Ct, double>()(container)),
       target);
  }

  template <typename Ct>
  inline typename enable_if<details::is_compare_builtin<Ct>,
                            neighbor_iterator<const Ct> >::type
  neighbor_end(const Ct& container,
               const typename container_traits<Ct>::key_type& target)
  {
    return neighbor_end
      (container,
       euclidian<Ct, double,
                 typename details::with_builtin_difference<Ct, double>::type>
         (details::with_builtin_difference<Ct, double>()(container)),
       target);
  }
  //@}

  /**
   *  Build a \ref neighbor_iterator<> pointing to the nearest neighbor of \c
   *  target using a user-defined \ref Metric.
   *  \param container The container in which a neighbor must be found.
   *  \param metric The metric to use in search of the neighbor.
   *  \param target The target key used in the neighbor search.
   */
  //@{
  template <typename Ct, typename Metric>
  inline neighbor_iterator<Ct, Metric>
  neighbor_begin(Ct& container, const Metric& metric,
                 const typename container_traits<Ct>::key_type& target)
  {
    if (container.empty()) return neighbor_end(container, metric, target);
    neighbor_iterator<Ct, Metric>
      it(container, metric, target, 0, container.end().node->parent);
    return details::minimum_neighbor(it);
  }

  template <typename Ct, typename Metric>
  inline neighbor_iterator<const Ct, Metric>
  neighbor_begin(const Ct& container, const Metric& metric,
                 const typename container_traits<Ct>::key_type& target)
  {
    if (container.empty()) return mapping_end(container, metric, target);
    neighbor_iterator<const Ct, Metric>
      it(container, metric, target, 0, container.end().node->parent);
    return details::minimum_neighbor(it);
  }

  template <typename Ct, typename Metric>
  inline neighbor_iterator<const Ct, Metric>
  neighbor_cbegin(const Ct& container, const Metric& metric,
                  const typename container_traits<Ct>::key_type& target)
  { return neighbor_begin(container, metric, target); }
  //@}

  /**
   *  Build a \ref neighbor_iterator<> pointing to the nearest neighbor of \c
   *  target assuming an euclidian metric with distances expressed in double. It
   *  requires that the container used was defined with a built-in key compare
   *  functor.
   *  \param container The container in which a neighbor must be found.
   *  \param target The target key used in the neighbor search.
   */
  //@{
  template <typename Ct>
  inline typename enable_if<details::is_compare_builtin<Ct>,
                            neighbor_iterator<Ct> >::type
  neighbor_begin(Ct& container,
                 const typename container_traits<Ct>::key_type& target)
  {
    return neighbor_begin
      (container,
       euclidian<Ct, double,
                 typename details::with_builtin_difference<Ct, double>::type>
         (details::with_builtin_difference<Ct, double>()(container)),
       target);
  }

  template <typename Ct>
  inline typename enable_if<details::is_compare_builtin<Ct>,
                            neighbor_iterator<const Ct> >::type
  neighbor_begin(const Ct& container,
                 const typename container_traits<Ct>::key_type& target)
  {
    return neighbor_begin
      (container,
       euclidian<Ct, double,
                 typename details::with_builtin_difference<Ct, double>::type>
         (details::with_builtin_difference<Ct, double>()(container)),
       target);
  }
  //@}

  /**
   *  Build a \ref neighbor_iterator<> pointing to the neighbor closest to
   *  target but for which distance to target is greater or equal to the value
   *  given in \c bound. Uses a user-defined \ref Metric.
   *  \param container The container in which a neighbor must be found.
   *  \param metric The metric to use in search of the neighbor.
   *  \param target The target key used in the neighbor search.
   */
  //@{
  template <typename Ct, typename Metric>
  inline neighbor_iterator<Ct, Metric>
  neighbor_lower_bound(Ct& container, const Metric& metric,
                       const typename container_traits<Ct>::key_type& target,
                       const typename Metric::distance_type& bound)
  {
    if (container.empty()) return mapping_end(container, metric, target);
    except::check_positive_distance(bound);
    neighbor_iterator<Ct, Metric>
      it(container, metric, target, 0, container.top()); // At root dim = 0
    return details::lower_bound_neighbor(it, bound);
  }

  template <typename Ct, typename Metric>
  inline neighbor_iterator<const Ct, Metric>
  neighbor_lower_bound(const Ct& container, const Metric& metric,
                       const typename container_traits<Ct>::key_type& target,
                       const typename Metric::distance_type& bound)
  {
    if (container.empty()) return mapping_end(container, metric, target);
    except::check_positive_distance(bound);
    neighbor_iterator<const Ct, Metric>
      it(container, metric, target, 0, container.top()); // At root dim = 0
    return details::lower_bound_neighbor(it, bound);
  }

  template <typename Ct, typename Metric>
  inline neighbor_iterator<const Ct, Metric>
  neighbor_clower_bound(const Ct& container, const Metric& metric,
                        const typename container_traits<Ct>::key_type& target,
                        const typename Metric::distance_type& bound)
  { return neighbor_lower_bound(container, metric, target, bound); }
  //@}

  /**
   *  Build a \ref neighbor_iterator<> pointing to the neighbor closest to
   *  target but for which distance to target is greater or equal to the value
   *  given in \c bound. It assumes an euclidian metric with distances expressed
   *  in double. It also requires that the container used was defined with one
   *  of the built-in key compare functor.
   *  \param container The container in which a neighbor must be found.
   *  \param target The target key used in the neighbor search.
   */
  //@{
  template <typename Ct>
  inline typename enable_if<details::is_compare_builtin<Ct>,
                            neighbor_iterator<Ct> >::type
  neighbor_lower_bound(Ct& container,
                       const typename container_traits<Ct>::key_type& target,
                       double bound)
  {
    return neighbor_lower_bound
      (container,
       euclidian<Ct, double,
                 typename details::with_builtin_difference<Ct, double>::type>
         (details::with_builtin_difference<Ct, double>()(container)),
       target, bound);
  }

  template <typename Ct>
  inline typename enable_if<details::is_compare_builtin<Ct>,
                            neighbor_iterator<const Ct> >::type
  neighbor_lower_bound(const Ct& container,
                       const typename container_traits<Ct>::key_type& target,
                       double bound)
  {
    return neighbor_lower_bound
      (container,
       euclidian<Ct, double,
                 typename details::with_builtin_difference<Ct, double>::type>
         (details::with_builtin_difference<Ct, double>()(container)),
       target, bound);
  }
  //@}

  /**
   *  Build a \ref neighbor_iterator<> pointing to the neighbor closest to
   *  target but for which distance to target is strictly greater than the value
   *  given in \c bound. Uses a user-defined \ref Metric.
   *  \param container The container in which a neighbor must be found.
   *  \param metric The metric to use in search of the neighbor.
   *  \param target The target key used in the neighbor search.
   */
  //@{
  template <typename Ct, typename Metric>
  inline neighbor_iterator<Ct, Metric>
  neighbor_upper_bound(Ct& container, const Metric& metric,
                       const typename container_traits<Ct>::key_type& target,
                       const typename Metric::distance_type& bound)
  {
    if (container.empty()) return mapping_end(container, metric, target);
    except::check_positive_distance(bound);
    neighbor_iterator<Ct, Metric>
      it(container, metric, target, 0, container.top()); // At root dim = 0
    return details::upper_bound_neighbor(it, bound);
  }

  template <typename Ct, typename Metric>
  inline neighbor_iterator<const Ct, Metric>
  neighbor_upper_bound(const Ct& container, const Metric& metric,
                       const typename container_traits<Ct>::key_type& target,
                       const typename Metric::distance_type& bound)
  {
    if (container.empty()) return mapping_end(container, metric, target);
    except::check_positive_distance(bound);
    neighbor_iterator<const Ct, Metric>
      it(container, metric, target, 0, container.top()); // At root dim = 0
    return details::upper_bound_neighbor(it, bound);
  }

  template <typename Ct, typename Metric>
  inline neighbor_iterator<const Ct, Metric>
  neighbor_cupper_bound(const Ct& container, const Metric& metric,
                        const typename container_traits<Ct>::key_type& target,
                        const typename Metric::distance_type& bound)
  { return neighbor_upper_bound(container, metric, target); }
  //@}

  /**
   *  Build a \ref neighbor_iterator<> pointing to the neighbor closest to
   *  target but for which distance to target is greater than the value given in
   *  \c bound. It assumes an euclidian metric with distances expressed in
   *  double. It also requires that the container used was defined with one of
   *  the built-in key compare functor.
   *  \param container The container in which a neighbor must be found.
   *  \param target The target key used in the neighbor search.
   */
  //@{
  template <typename Ct>
  inline typename enable_if<details::is_compare_builtin<Ct>,
                            neighbor_iterator<Ct> >::type
  neighbor_upper_bound(Ct& container,
                       const typename container_traits<Ct>::key_type& target,
                       double bound)
  {
    return neighbor_upper_bound
      (container,
       euclidian<Ct, double,
                 typename details::with_builtin_difference<Ct, double>::type>
         (details::with_builtin_difference<Ct, double>()(container)),
       target, bound);
  }

  template <typename Ct>
  inline typename enable_if<details::is_compare_builtin<Ct>,
                            neighbor_iterator<const Ct> >::type
  neighbor_upper_bound(const Ct& container,
                       const typename container_traits<Ct>::key_type& target,
                       double bound)
  {
    return neighbor_upper_bound
      (container,
       euclidian<Ct, double,
                 typename details::with_builtin_difference<Ct, double>::type>
         (details::with_builtin_difference<Ct, double>()(container)),
       target, bound);
  }
  //@}

} // namespace spatial

#include "spatial_euclidian_neighbor.hpp"
#include "spatial_neighbor.tpp"

#endif // SPATIAL_NEIGHBOR_HPP
