
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

## 2024-05-15 - [Avoid `sqrt` in distance comparisons]
**Learning:** `sqrt` is an expensive operation that can bottleneck hot loops. When comparing a distance against a threshold (e.g. `mindist < distance`), we can often compare the squared distance instead (`mindistSqr < distance * distance`), avoiding the `sqrt` calculation unless the threshold is met and we need to update the actual distance value. This assumes the threshold distance is non-negative (`distance > 0`).
**Action:** Always check if a square root calculation can be avoided by comparing squared values, especially in simulation hot loops like collision detection or nearest-neighbor searches.
