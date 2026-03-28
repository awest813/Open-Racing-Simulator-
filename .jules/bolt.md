## 2024-05-20 - Fast Sound Source Allocation

**Learning:** When managing a shared pool of limited OpenAL sound sources where elements are frequently allocated and de-allocated on every frame (like cars starting/stopping sounds), iterating over the pool sequentially causes an $O(N)$ hit per request. This isn't scalable and creates micro-stutters when multiple sources compete.

**Action:** Replace $O(N)$ pooling searches with an $O(1)$ doubly-linked list of free items. Use array indices for `next_free` and `prev_free` inside the source pool structs to avoid runtime memory allocation while achieving immediate $O(1)$ retrieval of the next available source and $O(1)$ LRU (Least Recently Used) ordering when sources are released.
