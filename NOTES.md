# Be Aware of
* switch/case
    * May be optimized to a lookup in other segment
    * As a result, fails on second+ gen, but not first gen
    * Solution: Disable optimizations
* Functions returning constant to be modified
    * May be optimized out
    * Solution: Disable optimizations
