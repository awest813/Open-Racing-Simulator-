
## 2025-02-14 - Optimized distance calculations in drivers
**Learning:** Found multiple instances where `sqrt()` is calculated inside hot loops (e.g., iterating through corners to find the minimum distance) by using `dist()` methods on vector and straight classes.
**Action:** Introduced `lenSqr()` and `distSqr()` to avoid `sqrt` calculation until after finding the minimum squared distance. Always check if a squared distance comparison `distSqr < mindistSqr` can replace `dist < mindist` inside loops to avoid expensive operations.

## 2025-03-29 - O(1) Doubly Linked Free List for Object Pools
**Learning:** In hot loops like an audio engine's free list management (e.g. `SharedSourcePool`), an $O(n)$ search to find the next free source introduces unnecessary linear scan overheads on each retrieval and release.
**Action:** Always replace basic O(n) array-based free list searches with $O(1)$ operations by weaving a doubly linked list explicitly through the array elements using `prev_free` and `next_free` pointer offsets. A doubly linked list is preferable over a singly linked list if objects can be "resurrected" out of order (removed from the middle of the free list).

## 2025-04-03 - Restrict Scene Graph Intersection Queries
**Learning:** In PLIB/SSG modules, global intersection/raycasting queries against the root scene graph (`TheScene`) using methods like `ssgHOT` traverse everything, including cars, UI, and particle effects.
**Action:** Always restrict scene graph traversal algorithms (`ssgHOT`, `ssgLOS`) to their minimal necessary sub-branches. For height-over-terrain calculations, always pass the `LandAnchor` branch instead of `TheScene` to eliminate unnecessary $O(N)$ branch evaluation.

## 2025-05-18 - Optimized redundant math in driver hot loops
**Learning:** Found multiple instances where expensive math operations like `sqrt(psdyn->getSpeedsqr(seg))` were called twice per O(N) loop iteration due to being defined inline in the catchdist mathematical expression without caching.
**Action:** Always extract repeated expensive calculations (`sqrt`, `log`, etc) into local variables within loops instead of calculating them multiple times inline.
## 2025-01-20 - Bypass expensive sqrt in distance check
**Learning:** Checking distances between dynamic objects can trigger excessive, expensive `sqrt()` evaluations in hot loops (e.g. iterating over opponents).
**Action:** Always prefer squared distance comparisons first (`if (distSqr < dist * dist)`) to bypass the expensive root calculation unless mathematically necessary, significantly improving execution performance.

## 2024-04-27 - Extracted Expensive Function Calls from MIN/MAX Macros
**Learning:** In the TORCS codebase, `MIN` and `MAX` are often defined as multi-evaluating macros (e.g., `#define MIN(x,y) ((x) < (y) ? (x) : (y))`). Passing an expensive function call like `sqrt()` directly into these macros causes it to be executed redundantly when it is the selected value. This can create performance bottlenecks, especially in high-frequency logic like collision detection (`collide.cpp`) or pathfinding (`pathfinder.cpp`).
**Action:** Always extract expensive mathematical function calls (like `sqrt`) into local variables before passing them into `MIN`/`MAX` macros, or use `std::min`/`std::max` instead if the environment permits, to ensure the function is only evaluated once.
## 2024-05-16 - Optimize straight distance checks by avoiding sqrt in hot loops
**Learning:** In TORCS bot pathfinding (`bt` and `damned` drivers), distance checks to car frontlines were repeatedly calculating `sqrt()` inside loops (4 times per opponent iteration). The underlying math library (`v2_t` and `straight2t`) only provided `len()` and `dist()` which invoked `sqrt()`.
**Action:** Introduced `lenSqr()` and `distSqr()` to the math library to defer `sqrt()` evaluation. In hot loops, always compute and compare squared distances against `threshold * threshold`, and only call `sqrt()` once outside the loop if the squared condition is met.

## 2024-05-30 - Optimize threshold comparisons using squared values in simuv3
**Learning:** In the TORCS simulation engine (`simuv3/collide.cpp`, `simuv3/aero.cpp`), threshold checks using `sgLengthVec3` invoke expensive `sqrt()` operations in hot loops during damage/collision checks.
**Action:** Always replace `sgLengthVec3` with `sgLengthSquaredVec3` when making simple threshold comparisons (e.g. `len < threshold` -> `lenSqr < threshold * threshold`), completely bypassing unnecessary square roots.

## 2024-06-03 - Cache Expensive Method Calls in Identical Execution Paths
**Learning:** Found an instance in `src/modules/simu/simuv2/aero.cpp` where the same exact `sqrt` expression was being calculated twice within the identical execution flow (once to compute `atan2` and once for a simple check `> 0.0f`).
**Action:** Always extract redundantly computed expensive operations (`sqrt()`) into local variables within loops or identical code paths to avoid re-evaluating the same expression twice.
## 2024-06-02 - Conditional sqrt evaluation in collision hot loop
**Learning:** In hot loops like physics collision response (`SimCarCollideResponse`), unconditional `sgLengthVec2()` or `sgLengthVec3()` calls incur heavy `sqrt()` overhead. When the length is only used in a clamping/min function (e.g. `MIN(distpab, 0.05)`), we can first check the squared distance against the squared threshold (`distpabSqr < 0.05*0.05`) and only compute `sqrt()` when the condition is met.
**Action:** Replace direct length calls with conditional `sqrt()` evaluation using squared distance functions (`sgLengthSquaredVec2`, `sgLengthSquaredVec3`) when the length is compared against or clamped by a known threshold.
## 2025-06-11 - Deferred sqrt calculation with bounded clamped limits
**Learning:** Found an instance in `src/modules/simu/simuv2/collide.cpp` where the `sqrt` function via `sgLengthVec2(n)` was executed indiscriminately, even though its output was immediately constrained within tight min-max bounds (`MIN(MAX(x, min), max)`).
**Action:** Always refactor bounding constraint operations to compare against the squared boundaries (`CAR_MIN_MOVEMENT_SQR`, `CAR_MAX_MOVEMENT_SQR`) before conditionally evaluating the square root, effectively skipping expensive math operations for bounded values.
