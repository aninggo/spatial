// -*- C++ -*-
//
// Copyright Sylvain Bougerel 2009 - 2012.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file COPYING or copy at
// http://www.boost.org/LICENSE_1_0.txt)

/**
 *  @file   spatial_test_node.hpp
 *  @brief  All tests for the elements defined in spatial_node.hpp are located
 *  in this file.
 *  @see spatial_node.hpp
 */

#ifndef SPATIAL_TEST_NODE_HPP
#define SPATIAL_TEST_NODE_HPP

struct header_fixture
{
  Node<Kdtree_link<int, int> > header;
  header_fixture()
  {
    header.parent = &header;
    header.left = &header;
    header.right = &header;
  }
};

struct int_node_fixture
{
  /*          H
              |
             Root
             /   \
             L     R
            /  \
          LL    LR       */
  typedef Node<Kdtree_link<int, int> > node_type;
  node_type header;
  node_type node_root;
  node_type node_left;
  node_type node_left_left;
  node_type node_left_right;
  node_type node_right;

  int_node_fixture()
  {
    header.parent = &node_root;
    header.left = &header;
    header.right = &node_right;
    node_root.parent = &header;
    node_root.left = &node_left;
    node_root.right = &node_right;
    node_left.parent = &node_root;
    node_left.left = &node_left_left;
    node_left.right = &node_left_right;
    node_right.parent = &node_root;
    node_right.left = 0;
    node_right.right = 0;
    node_left_right.parent = &node_left;
    node_left_right.left = 0;
    node_left_right.right = 0;
    node_left_left.parent = &node_left;
    node_left_left.left = 0;
    node_left_left.right = 0;
  }
};

BOOST_AUTO_TEST_CASE( test_header )
{
  header_fixture fix1;
  BOOST_CHECK(header(&fix1.header));
  int_node_fixture fix2;
  BOOST_CHECK(header(&fix2.header));
  BOOST_CHECK(!header(&fix2.node_root));
  BOOST_CHECK(!header(&fix2.node_left));
  BOOST_CHECK(!header(&fix2.node_left_left));
  BOOST_CHECK(!header(&fix2.node_left_right));
  BOOST_CHECK(!header(&fix2.node_right));
  BOOST_CHECK(!header(&fix2.node_right));
}

BOOST_AUTO_TEST_CASE( test_5_node )
{
  int_node_fixture fix;
  BOOST_CHECK(header(&fix.header));
  BOOST_CHECK(!header(&fix.node_root));
  BOOST_CHECK(!header(&fix.node_left));
  BOOST_CHECK(!header(&fix.node_right));
  BOOST_CHECK(!header(&fix.node_left_right));
  BOOST_CHECK(!header(&fix.node_left_left));
  BOOST_CHECK(minimum(&fix.node_root) == &fix.node_left_left);
  BOOST_CHECK(maximum(&fix.node_root) == &fix.node_right);
  Node<Kdtree_link<int, int> >* node = &fix.node_left_left;
  BOOST_REQUIRE((node = increment(node)) == &fix.node_left);
  BOOST_REQUIRE((node = increment(node)) == &fix.node_left_right);
  BOOST_REQUIRE((node = increment(node)) == &fix.node_root);
  BOOST_REQUIRE((node = increment(node)) == &fix.node_right);
  BOOST_REQUIRE((node = increment(node)) == &fix.header);
  BOOST_REQUIRE((node = decrement(node)) == &fix.node_right);
  BOOST_REQUIRE((node = decrement(node)) == &fix.node_root);
  BOOST_REQUIRE((node = decrement(node)) == &fix.node_left_right);
  BOOST_REQUIRE((node = decrement(node)) == &fix.node_left);
  BOOST_REQUIRE((node = decrement(node)) == &fix.node_left_left);
  const Node<Kdtree_link<int, int> >* cnode = &fix.node_root;
  BOOST_REQUIRE((cnode = preorder_increment(cnode))
                == &fix.node_left);
  BOOST_REQUIRE((cnode = preorder_increment(cnode))
                == &fix.node_left_left);
  BOOST_REQUIRE((cnode = preorder_increment(cnode))
                == &fix.node_left_right);
  BOOST_REQUIRE((cnode = preorder_increment(cnode))
                == &fix.node_right);
  BOOST_REQUIRE((cnode = preorder_increment(cnode))
                == &fix.header);
}

BOOST_AUTO_TEST_CASE( test_swap_node )
{
  {
    int_node_fixture fix;
    Base_ptr left_left = &fix.node_left_left;
    Base_ptr right = &fix.node_right;
    swap(*left_left, *right);
    BOOST_CHECK(left_left == &fix.node_left_left);
    BOOST_CHECK(right == &fix.node_right);
    BOOST_CHECK(fix.node_left_left.left == 0);
    BOOST_CHECK(fix.node_left_left.right == 0);
    BOOST_CHECK(fix.node_left_left.parent == &fix.node_root);
    BOOST_CHECK(fix.node_root.right == &fix.node_left_left);
    BOOST_CHECK(fix.node_root.left == &fix.node_left);
    BOOST_CHECK(fix.node_root.parent == &fix.header);
    BOOST_CHECK(fix.node_right.left == 0);
    BOOST_CHECK(fix.node_right.right == 0);
    BOOST_CHECK(fix.node_right.parent == &fix.node_left);
    BOOST_CHECK(fix.node_left.right == &fix.node_left_right);
    BOOST_CHECK(fix.node_left.left == &fix.node_right);
    BOOST_CHECK(fix.node_left.parent == &fix.node_root);
  }
  {
    Five_Node_basic_fixture fix; // swap with non-root, invert args
    Base_ptr left_left = &fix.node_left_left;
    Base_ptr right = &fix.node_right;
    swap(*right, *left_left);
    BOOST_CHECK(left_left == &fix.node_left_left);
    BOOST_CHECK(right == &fix.node_right);
    BOOST_CHECK(fix.node_left_left.left == 0);
    BOOST_CHECK(fix.node_left_left.right == 0);
    BOOST_CHECK(fix.node_left_left.parent == &fix.node_root);
    BOOST_CHECK(fix.node_root.right == &fix.node_left_left);
    BOOST_CHECK(fix.node_root.left == &fix.node_left);
    BOOST_CHECK(fix.node_root.parent == &fix.header);
    BOOST_CHECK(fix.node_right.left == 0);
    BOOST_CHECK(fix.node_right.right == 0);
    BOOST_CHECK(fix.node_right.parent == &fix.node_left);
    BOOST_CHECK(fix.node_left.right == &fix.node_left_right);
    BOOST_CHECK(fix.node_left.left == &fix.node_right);
    BOOST_CHECK(fix.node_left.parent == &fix.node_root);
  }
  {
    Five_Node_basic_fixture fix; // swap with root
    Base_ptr left_left = &fix.node_left_left;
    Base_ptr root = &fix.node_root;
    swap(*left_left, *root);
    BOOST_CHECK(left_left == &fix.node_left_left);
    BOOST_CHECK(root == &fix.node_root);
    BOOST_CHECK(fix.node_left_left.left == &fix.node_left);
    BOOST_CHECK(fix.node_left_left.right == &fix.node_right);
    BOOST_CHECK(fix.node_left_left.parent == &fix.header);
    BOOST_CHECK(fix.node_root.right == 0);
    BOOST_CHECK(fix.node_root.left == 0);
    BOOST_CHECK(fix.node_root.parent == &fix.node_left);
    BOOST_CHECK(fix.node_right.left == 0);
    BOOST_CHECK(fix.node_right.right == 0);
    BOOST_CHECK(fix.node_right.parent == &fix.node_left_left);
    BOOST_CHECK(fix.node_left.right == &fix.node_left_right);
    BOOST_CHECK(fix.node_left.left == &fix.node_root);
    BOOST_CHECK(fix.node_left.parent == &fix.node_left_left);
    BOOST_CHECK(fix.header.parent == &fix.node_left_left);
    BOOST_CHECK(fix.header.left == &fix.header);
    BOOST_CHECK(fix.header.right == &fix.node_right);
  }
  {
    Five_Node_basic_fixture fix; // swap with root, invert args
    Base_ptr left_left = &fix.node_left_left;
    Base_ptr root = &fix.node_root;
    swap(*root, *left_left);
    BOOST_CHECK(left_left == &fix.node_left_left);
    BOOST_CHECK(root == &fix.node_root);
    BOOST_CHECK(fix.node_left_left.left == &fix.node_left);
    BOOST_CHECK(fix.node_left_left.right == &fix.node_right);
    BOOST_CHECK(fix.node_left_left.parent == &fix.header);
    BOOST_CHECK(fix.node_root.right == 0);
    BOOST_CHECK(fix.node_root.left == 0);
    BOOST_CHECK(fix.node_root.parent == &fix.node_left);
    BOOST_CHECK(fix.node_right.left == 0);
    BOOST_CHECK(fix.node_right.right == 0);
    BOOST_CHECK(fix.node_right.parent == &fix.node_left_left);
    BOOST_CHECK(fix.node_left.right == &fix.node_left_right);
    BOOST_CHECK(fix.node_left.left == &fix.node_root);
    BOOST_CHECK(fix.node_left.parent == &fix.node_left_left);
    BOOST_CHECK(fix.header.parent == &fix.node_left_left);
    BOOST_CHECK(fix.header.left == &fix.header);
    BOOST_CHECK(fix.header.right == &fix.node_right);
  }
  {
    Five_Node_basic_fixture fix; // swap with left child
    Base_ptr left_left = &fix.node_left_left;
    Base_ptr left = &fix.node_left;
    swap(*left_left, *left);
    BOOST_CHECK(left_left == &fix.node_left_left);
    BOOST_CHECK(left == &fix.node_left);
    BOOST_CHECK(fix.node_left.left == 0);
    BOOST_CHECK(fix.node_left.right == 0);
    BOOST_CHECK(fix.node_left.parent == &fix.node_left_left);
    BOOST_CHECK(fix.node_left_left.left == &fix.node_left);
    BOOST_CHECK(fix.node_left_left.right == &fix.node_left_right);
    BOOST_CHECK(fix.node_left_left.parent == &fix.node_root);
    BOOST_CHECK(&fix.node_left_left == fix.node_root.left);
    BOOST_CHECK(&fix.node_left_left == fix.node_left_right.parent);
  }
  {
    Five_Node_basic_fixture fix; // swap with left child, invert args
    Base_ptr left_left = &fix.node_left_left;
    Base_ptr left = &fix.node_left;
    swap(*left, *left_left);
    BOOST_CHECK(left_left == &fix.node_left_left);
    BOOST_CHECK(left == &fix.node_left);
    BOOST_CHECK(fix.node_left.left == 0);
    BOOST_CHECK(fix.node_left.right == 0);
    BOOST_CHECK(fix.node_left.parent == &fix.node_left_left);
    BOOST_CHECK(fix.node_left_left.left == &fix.node_left);
    BOOST_CHECK(fix.node_left_left.right == &fix.node_left_right);
    BOOST_CHECK(fix.node_left_left.parent == &fix.node_root);
    BOOST_CHECK(&fix.node_left_left == fix.node_root.left);
    BOOST_CHECK(&fix.node_left_left == fix.node_left_right.parent);
  }
  {
    Five_Node_basic_fixture fix; // swap with right child
    Base_ptr left_right = &fix.node_left_right;
    Base_ptr left = &fix.node_left;
    swap(*left_right, *left);
    BOOST_CHECK(left_right == &fix.node_left_right);
    BOOST_CHECK(left == &fix.node_left);
    BOOST_CHECK(fix.node_left.left == 0);
    BOOST_CHECK(fix.node_left.right == 0);
    BOOST_CHECK(fix.node_left.parent == &fix.node_left_right);
    BOOST_CHECK(fix.node_left_right.left == &fix.node_left_left);
    BOOST_CHECK(fix.node_left_right.right == &fix.node_left);
    BOOST_CHECK(fix.node_left_right.parent == &fix.node_root);
    BOOST_CHECK(&fix.node_left_right == fix.node_root.left);
    BOOST_CHECK(&fix.node_left_right == fix.node_left_left.parent);
  }
  {
    Five_Node_basic_fixture fix; // swap with right child, invert args
    Base_ptr left_right = &fix.node_left_right;
    Base_ptr left = &fix.node_left;
    swap(*left, *left_right);
    BOOST_CHECK(left_right == &fix.node_left_right);
    BOOST_CHECK(left == &fix.node_left);
    BOOST_CHECK(fix.node_left.left == 0);
    BOOST_CHECK(fix.node_left.right == 0);
    BOOST_CHECK(fix.node_left.parent == &fix.node_left_right);
    BOOST_CHECK(fix.node_left_right.left == &fix.node_left_left);
    BOOST_CHECK(fix.node_left_right.right == &fix.node_left);
    BOOST_CHECK(fix.node_left_right.parent == &fix.node_root);
    BOOST_CHECK(&fix.node_left_right == fix.node_root.left);
    BOOST_CHECK(&fix.node_left_right == fix.node_left_left.parent);
  }
  {
    Five_Node_basic_fixture fix; // swap root with left child
    Base_ptr root = &fix.node_root;
    Base_ptr left = &fix.node_left;
    swap(*left, *root);
    BOOST_CHECK(root == &fix.node_root);
    BOOST_CHECK(left == &fix.node_left);
    BOOST_CHECK(fix.node_left.left == &fix.node_root);
    BOOST_CHECK(fix.node_left.right == &fix.node_right);
    BOOST_CHECK(fix.node_left.parent == &fix.header);
    BOOST_CHECK(fix.node_root.left == &fix.node_left_left);
    BOOST_CHECK(fix.node_root.right == &fix.node_left_right);
    BOOST_CHECK(fix.node_root.parent == &fix.node_left);
    BOOST_CHECK(fix.header.parent == &fix.node_left);
    BOOST_CHECK(&fix.node_root == fix.node_left_left.parent);
    BOOST_CHECK(&fix.node_root == fix.node_left_right.parent);
  }
  {
    Five_Node_basic_fixture fix; // swap root with left child, invert args
    Base_ptr root = &fix.node_root;
    Base_ptr left = &fix.node_left;
    swap(*root, *left);
    BOOST_CHECK(root == &fix.node_root);
    BOOST_CHECK(left == &fix.node_left);
    BOOST_CHECK(fix.node_left.left == &fix.node_root);
    BOOST_CHECK(fix.node_left.right == &fix.node_right);
    BOOST_CHECK(fix.node_left.parent == &fix.header);
    BOOST_CHECK(fix.node_root.left == &fix.node_left_left);
    BOOST_CHECK(fix.node_root.right == &fix.node_left_right);
    BOOST_CHECK(fix.node_root.parent == &fix.node_left);
    BOOST_CHECK(fix.header.parent == &fix.node_left);
    BOOST_CHECK(&fix.node_root == fix.node_left_left.parent);
    BOOST_CHECK(&fix.node_root == fix.node_left_right.parent);
  }
  {
    Five_Node_basic_fixture fix; // swap root with right child
    Base_ptr root = &fix.node_root;
    Base_ptr right = &fix.node_right;
    swap(*right, *root);
    BOOST_CHECK(root == &fix.node_root);
    BOOST_CHECK(right == &fix.node_right);
    BOOST_CHECK(fix.node_right.left == &fix.node_left);
    BOOST_CHECK(fix.node_right.right == &fix.node_root);
    BOOST_CHECK(fix.node_right.parent == &fix.header);
    BOOST_CHECK(fix.node_root.left == 0);
    BOOST_CHECK(fix.node_root.right == 0);
    BOOST_CHECK(fix.node_root.parent == &fix.node_right);
    BOOST_CHECK(fix.header.parent == &fix.node_right);
    BOOST_CHECK(&fix.node_right == fix.node_left.parent);
  }
  {
    Five_Node_basic_fixture fix; // swap root with right child, invert args
    Base_ptr root = &fix.node_root;
    Base_ptr right = &fix.node_right;
    swap(*root, *right);
    BOOST_CHECK(root == &fix.node_root);
    BOOST_CHECK(right == &fix.node_right);
    BOOST_CHECK(fix.node_right.left == &fix.node_left);
    BOOST_CHECK(fix.node_right.right == &fix.node_root);
    BOOST_CHECK(fix.node_right.parent == &fix.header);
    BOOST_CHECK(fix.node_root.left == 0);
    BOOST_CHECK(fix.node_root.right == 0);
    BOOST_CHECK(fix.node_root.parent == &fix.node_right);
    BOOST_CHECK(fix.header.parent == &fix.node_right);
    BOOST_CHECK(&fix.node_right == fix.node_left.parent);
  }
}

BOOST_AUTO_TEST_CASE( test_swap_kdtree_node )
{ // swap only position (tested before). Do not swap values!
  Five_Kdtree_node_fixture fix;
  Kdtree_node<int2>* left_left = &fix.node_left_left;
  Kdtree_node<int2>* right = &fix.node_right;
  swap(*left_left, *right);
  BOOST_CHECK(fix.node_left_left.value == zeros);
  BOOST_CHECK(fix.node_right.value == threes);
}

BOOST_AUTO_TEST_CASE( test_swap_relaxed_kdtree_node )
{ // swap position and weight, but not values!
  Five_Relaxed_kdtree_node_fixture fix;
  Relaxed_kdtree_node<int2>* left = &fix.node_left;
  Relaxed_kdtree_node<int2>* right = &fix.node_right;
  swap(*left, *right);
  BOOST_CHECK_EQUAL(fix.node_left.weight, 1);
  BOOST_CHECK_EQUAL(fix.node_right.weight, 3);
  BOOST_CHECK(fix.node_left.value == ones);
  BOOST_CHECK(fix.node_right.value == threes);
}

BOOST_AUTO_TEST_CASE( test_Node_iterator )
{
  // increment and decrement tested earlier, check pre/post correctness and
  // derefencing
  {
    typedef details::Node_iterator<pair_type,
      Kdtree_node<pair_type> > iterator;
    pair_type test_object(1, 2);
    Kdtree_node<pair_type> test_node;
    test_node.parent = &test_node;
    test_node.left = 0;
    test_node.right = 0;
    test_node.value = test_object;
    iterator iter(&test_node);
    BOOST_CHECK(*iter == test_object);
    BOOST_CHECK_EQUAL(iter->first, 1);
    BOOST_CHECK_EQUAL(iter->second, 2);
    // should compile: non-const iterator!
    BOOST_CHECK((*iter = test_object) == test_object);
    BOOST_CHECK_EQUAL((iter->first = 3), 3);
  }
  {
    typedef details::Node_iterator<pair_type,
      Relaxed_kdtree_node<pair_type> > iterator;
    pair_type test_object(1, 2);
    Relaxed_kdtree_node<pair_type> test_node;
    test_node.parent = &test_node;
    test_node.left = 0;
    test_node.right = 0;
    test_node.value = test_object;
    iterator iter(&test_node);
    BOOST_CHECK(*iter == test_object);
    BOOST_CHECK_EQUAL(iter->first, 1);
    BOOST_CHECK_EQUAL(iter->second, 2);
    // should compile: non-const iterator!
    BOOST_CHECK((*iter = test_object) == test_object);
    BOOST_CHECK_EQUAL((iter->first = 3), 3);
  }
  {
    // Make sure that constant iterator does not invalidate anything
    Five_Kdtree_node_fixture fix;
    typedef details::Node_iterator<int2, Kdtree_node<int2> > iterator;
    iterator iter(&fix.node_root);
    BOOST_CHECK(*iter == twos);
  }
  {
    Five_Kdtree_node_fixture fix;
    typedef details::Node_iterator<int2, Kdtree_node<int2> > iterator;
    iterator iter(&fix.node_root);
    BOOST_REQUIRE((++iter).node == &fix.node_right);
    BOOST_REQUIRE((--iter).node == &fix.node_root);
    BOOST_REQUIRE((iter++).node == &fix.node_root);
    BOOST_REQUIRE(iter.node == &fix.node_right);
    BOOST_REQUIRE((iter--).node == &fix.node_right);
    BOOST_REQUIRE(iter.node == &fix.node_root);
  }
}

BOOST_AUTO_TEST_CASE( test_Const_Node_iterator )
{
  // increment and decrement tested earlier, check pre/post correctness and
  // derefencing
  {
    typedef details::Const_Node_iterator<pair_type,
      Kdtree_node<pair_type> > iterator;
    pair_type test_object(1, 2);
    Kdtree_node<pair_type> test_node;
    test_node.parent = &test_node;
    test_node.left = 0;
    test_node.right = 0;
    test_node.value = test_object;
    iterator iter(&test_node);
    BOOST_CHECK(*iter == test_object);
    BOOST_CHECK_EQUAL(iter->first, 1);
    BOOST_CHECK_EQUAL(iter->second, 2);
  }
  {
    typedef details::Const_Node_iterator<pair_type,
      Relaxed_kdtree_node<pair_type> > iterator;
    pair_type test_object(1, 2);
    Relaxed_kdtree_node<pair_type> test_node;
    test_node.parent = &test_node;
    test_node.left = 0;
    test_node.right = 0;
    test_node.value = test_object;
    iterator iter(&test_node);
    BOOST_CHECK(*iter == test_object);
    BOOST_CHECK_EQUAL(iter->first, 1);
    BOOST_CHECK_EQUAL(iter->second, 2);
  }
  {
    // Make sure that constant iterator does not invalidate anything
    Five_Kdtree_node_fixture fix;
    typedef details::Const_Node_iterator<int2, Kdtree_node<int2> > iterator;
    iterator iter(&fix.node_root);
    BOOST_CHECK(*iter == twos);
  }
  {
    Five_Kdtree_node_fixture fix;
    typedef details::Const_Node_iterator<int2, Kdtree_node<int2> > iterator;
    iterator iter(&fix.node_root);
    BOOST_REQUIRE((++iter).node == &fix.node_right);
    BOOST_REQUIRE((--iter).node == &fix.node_root);
    BOOST_REQUIRE((iter++).node == &fix.node_root);
    BOOST_REQUIRE(iter.node == &fix.node_right);
    BOOST_REQUIRE((iter--).node == &fix.node_right);
    BOOST_REQUIRE(iter.node == &fix.node_root);
  }
}

BOOST_AUTO_TEST_CASE( test_Preorder_node_iterator )
{
  // increment and decrement tested earlier, check pre/post correctness and
  // derefencing
  {
    typedef details::Preorder_node_iterator<pair_type,
      Kdtree_node<pair_type> > iterator;
    pair_type test_object(1, 2);
    Kdtree_node<pair_type> test_node;
    test_node.parent = &test_node;
    test_node.left = 0;
    test_node.right = 0;
    test_node.value = test_object;
    iterator iter(&test_node);
    BOOST_CHECK(*iter == test_object);
    BOOST_CHECK_EQUAL(iter->first, 1);
    BOOST_CHECK_EQUAL(iter->second, 2);
  }
  {
    Five_Kdtree_node_fixture fix;
    typedef details::Preorder_node_iterator<int2, Kdtree_node<int2> > iterator;
    iterator iter(&fix.node_root);
    BOOST_REQUIRE((++iter).node == &fix.node_left);
    BOOST_REQUIRE((iter++).node == &fix.node_left);
    BOOST_REQUIRE(iter.node == &fix.node_left_left);
  }
  {
    Five_Kdtree_node_fixture fix;
    typedef details::Preorder_node_iterator<int2, Kdtree_node<int2> > iterator;
    iterator a(&fix.node_root);
    iterator b(&fix.node_root);
    BOOST_CHECK(a == b);
    iterator c(&fix.node_left);
    BOOST_CHECK(a != c);
  }
  {
    Five_Kdtree_node_fixture fix;
    typedef details::Preorder_node_iterator<int2, Kdtree_node<int2> > iterator;
    iterator a(&fix.node_root);
    iterator b(a);
    BOOST_CHECK(a == b);
  }
}

#endif // SPATIAL_TEST_NODE_HPP
