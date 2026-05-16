1. **Add `lenSqr` to `v2t`**
   - Add `T lenSqr(void) const;` declaration to `src/libs/math/v2_t.h`.
   - Add inline definition returning `x*x+y*y`.

2. **Add `distSqr` to `straight2t`**
   - Add `T distSqr(const v2t<T> &p) const;` declaration to `src/libs/math/straight2_t.h`.
   - Add inline definition returning `d3.lenSqr()`.

3. **Optimize `bt` and `damned` drivers**
   - In `src/drivers/bt/opponent.cpp` and `src/drivers/damned/opponent.cpp`, replace `carFrontLine.dist(corner)` with `carFrontLine.distSqr(corner)`.
   - Track `mindistSqr` and compare against `distance * distance`.
   - After the loop, if `mindistSqr < distance * distance`, set `distance = sqrt(mindistSqr)`.

4. **Verify Syntax**
   - Run compilation or syntax checks to ensure the math library changes and driver changes are valid.

5. **Complete pre-commit steps**
   - Complete pre-commit steps to ensure proper testing, verification, review, and reflection are done.

6. **Submit**
   - Commit and submit the code with the PR title `⚡ Bolt: Optimize straight distance checks by avoiding sqrt in hot loops`.
