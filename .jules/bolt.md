
## 2025-02-14 - Optimized distance calculations in drivers
**Learning:** Found multiple instances where `sqrt()` is calculated inside hot loops (e.g., iterating through corners to find the minimum distance) by using `dist()` methods on vector and straight classes.
**Action:** Introduced `lenSqr()` and `distSqr()` to avoid `sqrt` calculation until after finding the minimum squared distance. Always check if a squared distance comparison `distSqr < mindistSqr` can replace `dist < mindist` inside loops to avoid expensive operations.

## 2025-03-29 - O(1) Doubly Linked Free List for Object Pools
**Learning:** In hot loops like an audio engine's free list management (e.g. `SharedSourcePool`), an $O(n)$ search to find the next free source introduces unnecessary linear scan overheads on each retrieval and release.
**Action:** Always replace basic O(n) array-based free list searches with $O(1)$ operations by weaving a doubly linked list explicitly through the array elements using `prev_free` and `next_free` pointer offsets. A doubly linked list is preferable over a singly linked list if objects can be "resurrected" out of order (removed from the middle of the free list).
