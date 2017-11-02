# presentation-files
source code and slides used in presentations

## Constant Fun

This presentation discusses why it is useful to move some of the
processing to compile time and shows some applications of doing so.
In particular it shows how to create associated containers constructed
at compile time and what is needed from the types involved to make
it possible. The presentation also does some analysis to estmimate
the cost in terms of compile-time and object file size.  Specifically,
the presentation discusses

* implication of static and dynamic initialization - the C++ language
  rules for implementing `constexpr` functions and classes supporting
  `constexpr` objectss.
* differences in error handling with constant expressions
* sorting sequences at compile and the needed infrastructure -
  creating constant associative containers with compile-time and
  run-time look-up

## Parallel Partition

The partition algorithm is an important building block for
other algorithms, especially for sorting. It seems like
partition is inherently sequential but there are ways to
parallelize the algorithm. This presentation shows a way
to do so.
