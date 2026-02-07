# Static Container Pool (SCP) - Design Notes

This document explains the ideas, data model, and allocation behavior of the Static Container Pool (SCP). It is intentionally non-code and focuses on how the system behaves in common scenarios.

## Purpose and Use Case
SCP is a small, static-memory container library aimed at embedded systems or constrained environments where dynamic allocation is undesirable or forbidden. All containers (queues, stacks, etc.) are created inside a single, pre-allocated byte buffer. This gives predictable memory usage and avoids heap fragmentation.

## High-Level Model
SCP manages three core pieces of state:

- A fixed-size byte buffer where all container data lives.
- A fixed-size map (array) of container pointers, indexed by container ID.
- A "nextFree" pointer that tracks the end of allocated data in the buffer.

Each container has a small header (metadata) followed by its data region. The header stores things like container type, element size, element count, and pointers used by that container (for example, head/tail for queues).

## Container Lifecycle Overview
1. **Initialization**
   - `SCP_init()` clears the buffer, clears the map, and sets `nextFree` to the start of the buffer.

2. **Creation**
   - The allocator first tries to reuse a previously freed container if it can fit the new data.
   - If a freed container fits exactly, it is reused in place.
   - If a freed container is larger, it is only reused if the remaining space can form another valid container (metadata + at least 1 byte of data). The remaining space becomes a new free container.
   - If no suitable freed container exists, the allocator appends a new container at `nextFree`, and moves `nextFree` forward.

3. **Deletion**
   - If the container being deleted is the last allocated one, `nextFree` moves backward and the map entry is cleared.
   - Otherwise, the container is marked as "free" and kept in the map so it can be reused later.
   - Free containers can be merged with adjacent free containers to reduce fragmentation.

## Free Container Merging
When a container is freed in the middle of the buffer, SCP attempts to merge it with immediately adjacent free containers that follow it. This creates larger free blocks and increases the chance that future allocations can reuse freed space.

## Allocation Rules (Why They Exist)
A freed container can only be split if the leftover region is large enough to become another valid container. A "valid container" means:

- It has a metadata header.
- It has at least 1 byte of data.

This rule prevents creation of tiny unusable gaps that cannot be represented in the map.

## Example Scenarios
The exact sizes below are illustrative. Assume a buffer size of 1000 bytes and a container header size of 32 bytes.

### 1) Simple Sequential Allocation
- Create Queue A: 3 elements x 2 bytes
- Create Queue B: 3 elements x 2 bytes
- Create Queue C: 100 elements x 8 bytes

These are all appended at the end of the buffer. `nextFree` moves forward each time.

### 2) Delete Last Container
- Create Queue A, B, C
- Delete Queue C (the last one)

Result: `nextFree` moves backward. The map entry for C is cleared. No free container remains in the middle.

### 3) Delete Middle Container
- Create Queue A, B, C
- Delete Queue B (middle)

Result: Queue B becomes a "free" container. It stays in the map, marked free, and can be reused later.

### 4) Reuse a Free Container (Exact Fit)
- A free container has 64 bytes of data
- You create a new container needing 64 bytes

Result: The free container is reused in place. No splitting occurs.

### 5) Reuse a Free Container (Split)
- A free container has 200 bytes of data
- You create a new container needing 100 bytes
- A container header is 32 bytes

Remaining space: 200 - 100 - 32 = 68 bytes

Result: SCP reuses the first part for the new container and creates a new free container in the remaining 68 bytes (since 68 >= 1 byte + header).

### 6) Reuse a Free Container (Not Enough for Split)
- A free container has 140 bytes of data
- You create a new container needing 120 bytes
- A container header is 32 bytes

Remaining space: 140 - 120 - 32 = -12 bytes

Result: The free container is too small to split, so SCP will not reuse it. The allocator will either find another free container or append at the end (if space exists).

### 7) Container Map Limit
Even if there is buffer space left, SCP cannot create more containers than the size of the container map. Once the map is full, new creation fails until at least one container is deleted.

### 8) Merge Free Containers
- Free container X is followed immediately by free container Y

Result: SCP merges X and Y into a single larger free container and removes Y from the map.

## Defragmentation (Planned)
A future defragmentation routine can periodically move free containers toward the end of the buffer. This would consolidate free space and allow larger allocations without increasing buffer size.

A "one-step" defrag strategy is recommended for embedded systems:
- Each defrag call moves a single free container.
- This keeps execution time bounded and avoids long critical sections.
- The map and any container-internal pointers must be updated after a move.

## API Scope
The current public API focuses on queues. Stack support is scaffolded but not implemented yet. The internal allocation mechanism is designed to handle any container type that fits the common header + data layout.

## Summary
SCP is a static, deterministic container pool with:

- Predictable memory usage
- Configurable buffer size and container count
- Reuse and merging of freed containers
- A queue implementation using a ring buffer
- A clear path to defragmentation

It is a good fit for embedded or real-time systems that need stable memory behavior without a heap.
