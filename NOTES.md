# Ideas
* Make infection marker more subtle
	* Timestamp based by combination of different bits
	* Eg only infect if lsb of timestamp = 1, set lsb to 1 after infection
	* As a result, ~50% infection chance for an uninfected file


# Be Aware of
* switch/case
    * May be optimized to a lookup in other segment
    * As a result, fails on second+ gen, but not first gen
    * Solution: Disable optimizations
* Functions returning constant to be modified
    * May be optimized out
    * Solution: Disable optimizations
