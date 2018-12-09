# bareopt
A simple subset of std::optional for c++11 and above. It should be mostly compatible with std::optional, except for the additional support to optional references.

This optional does not have:
* constexpr support
* conditionally explicit constructors
* triviality propagation (except for destructor triviality)
* relational operators
* probably something else

Some of the tests have been taken from [this optional implementation](https://github.com/TartanLlama/optional), which you should probably be using anyways.
