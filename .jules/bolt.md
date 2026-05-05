
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
## 2024-05-18 - Optimize Distance Checks

**Learning:** When calculating minimum distances from objects/corners to a line inside hot paths in bot logic, calling `sqrt` (via `dist()` functions) repeatedly in tight loops adds unnecessary overhead. The `olethros` and `sparkle` drivers already implemented optimizations to use `distSqr()` and compare squared distances, only calling `sqrt` once at the end if needed. However, the standard math library `straight2t` lacked a `distSqr()` method, leading other bots like `damned` and `bt` to use the unoptimized `dist()` method.
**Action:** Introduced `distSqr()` into the core math library `src/libs/math/straight2_t.h` and updated the `damned` and `bt` bot distance checking loops in `opponent.cpp` to use squared distances, matching the optimized patterns of the newer bots.
