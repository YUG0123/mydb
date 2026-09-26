# MyDB — A Multi-Tenant In-Memory Database with a REST API

A small database engine built from scratch in C++: a hash table of AVL trees, exposed over HTTP via the Crow framework. Built as a learning project to go deep on data structures, persistence, concurrency, and API design — not a production database, but a genuine implementation of the core ideas behind one.

## What it does

Each "owner" (e.g. a teacher, a namespace, a tenant) gets their own database, indexed by a hash table. Inside each owner's database, records are stored in a self-balancing AVL tree, sorted by name. You interact with it entirely over HTTP — create databases, insert/update/read/delete records — using standard REST conventions.

## Architecture

```
HashTable (FNV-1a hash, separate chaining)
   │
   ├── owner_key "rachna"  →  AVLTree (balanced, sorted by record name)
   │                             ├── mayhul
   │                             ├── rohan
   │                             └── ...
   └── owner_key "archna"  →  AVLTree
                                 └── ...
```

- **Hash table**: FNV-1a hash function, fixed capacity (101 buckets, prime-sized), collisions handled via separate chaining (linked list per bucket). No dynamic resizing in this version — a known, documented limitation (see below).
- **AVL tree**: standard self-balancing BST, storing records sorted alphabetically by name. All four rotation cases (LL, RR, LR, RL) implemented for both insert and delete, keeping worst-case height within the theoretical AVL bound (~1.44 × log₂n).
- **API layer**: built with [Crow](https://crowcpp.org/), a lightweight C++ web framework. Routes are kept thin — they parse the request, call into the hash table/AVL tree, and format a JSON response. No business logic lives in the routing layer.
- **Persistence**: append-only log (plain text, one line per operation). Every write is logged to disk immediately; on startup, the log is replayed top-to-bottom to rebuild the in-memory state.
- **Concurrency**: a single global mutex guards every route, since Crow runs multithreaded by default and AVL rotations mutate shared pointers mid-operation — an unprotected read during a rotation would be undefined behavior, not just stale data.
- **Auth**: a static, shared API key checked via a custom `X-API-Key` header on every route.

## API Reference

All endpoints require the header `X-API-Key: <your-key>`. Responses are JSON.

### Create a database
```
POST /api/create_database
Body: { "owner_key": "rachna" }
```

### Add or update a record
```
PATCH /api/update_record
Body: { "owner_key": "rachna", "name": "mayhul", "age": 19, "weight": 197, "cgpa": 9.7 }
```
If `name` already exists under this owner, its data is overwritten (this correctly *updates* — see Known Issues Fixed below).

### Read a record
```
GET /api/display_record?owner_key=rachna&name=mayhul
```

### Delete a record
```
DELETE /api/delete_record
Body: { "owner_key": "rachna", "name": "mayhul" }
```

### Example (curl)
```bash
curl -X POST http://127.0.0.1:3000/api/create_database \
  -H 'X-API-Key: your-secret-here' \
  -H 'Content-Type: application/json' \
  -d '{"owner_key":"rachna"}'
```

## Setup & Running

**Dependencies** (Linux/WSL):
```bash
sudo apt-get install libboost-all-dev libasio-dev
```
Crow is bundled as a single header (`include/crow_all.h`) — no separate install needed.

**Build:**
```bash
g++ -std=c++17 src/api.cpp -Iinclude -lpthread -o server
```

**Run:**
```bash
./server
# Server starts on http://127.0.0.1:3000
```

**Run integration tests** (in a second terminal, while the server is running):
```bash
bash tests/integration/test_api.sh
```

**Run unit tests** (AVL tree + hash table, no server needed):
```bash
g++ -std=c++17 tests/unit/test_avl.cpp -o test_avl && ./test_avl
```

**Run benchmarks:**
```bash
g++ -std=c++17 -O2 benchmarks/benchmark_avl.cpp -Iinclude -o benchmark_avl && ./benchmark_avl
```

## Design Decisions

**Why AVL trees, not B+ trees?**
The dataset lives entirely in memory, not on disk — B+ trees are optimized for minimizing disk block reads (wide, shallow nodes matching disk page sizes), which doesn't apply here. AVL trees give strong O(log n) guarantees for an in-memory workload and were the right complexity trade-off given the timeline. If this were extended to true disk-backed storage at scale, B+ trees would be the more appropriate choice — that's a deliberate scope decision, not an oversight.

**Why a single global mutex instead of fine-grained locking?**
Coarse-grained locking is simple to reason about and verifiably correct. Fine-grained (e.g. per-bucket) locking would improve throughput under high concurrency but adds real complexity (avoiding deadlocks across nested lock acquisitions, etc.) that wasn't justified at this project's scale. This was stress-tested with Apache Bench at 40 concurrent connections hammering the same record — 0 failures, no corruption (see Benchmarks below).

**Why append-only log instead of a proper write-ahead log (WAL) or binary format?**
Simplicity and correctness first. A plain-text, human-readable log is easy to debug and reason about. The trade-off: the log grows unboundedly and must be fully replayed on every startup — a real production system would periodically compact it. Documented as a known limitation, not fixed in this version.

**Why a single static API key instead of per-user accounts?**
Scoped deliberately — a full auth system (accounts, password hashing, sessions/tokens) is a meaningfully larger project. A shared API key closes the "anyone can read/write anyone's data" gap cheaply and correctly, and was the right-sized fix given the project's actual scope.

## Benchmarks

**AVL tree scaling** (measured via `benchmarks/benchmark_avl.cpp`, `-O2`):

| n | Insert time (total) | Search time (avg) | Tree height | log₂(n) |
|---|---|---|---|---|
| 100 | 29 μs | 0.16 μs | 7 | 6.64 |
| 1,000 | 318 μs | 0.22 μs | 11 | 9.97 |
| 10,000 | 4,155 μs | 0.26 μs | 15 | 13.29 |
| 100,000 | 52,833 μs | 0.64 μs | 19 | 16.61 |

Tree height stays within ~1.05–1.15× of log₂(n) across all tested sizes — well within the AVL worst-case theoretical bound of ~1.44× log₂(n), confirming the rebalancing logic holds at scale, not just on small test cases.

**API throughput** (measured via Apache Bench):
```
ab -n 500 -c 40 -p payload.json -T application/json -H "X-API-Key: ..." -m PATCH http://127.0.0.1:3000/api/update_record
```
500 requests at 40 concurrent connections, all targeting the same record (worst-case contention scenario) — 0 failed requests, ~9,360 requests/sec.

## Known Issues Fixed During Development
- **Duplicate-name update bug**: early versions of `insertHelper` silently ignored writes to an existing name instead of updating it — `update_record` looked successful but didn't persist changes. Fixed by overwriting `node->data` on the duplicate-name branch.
- **Route auth-check placement bug**: a misplaced opening brace in `display_record` briefly caused a compile failure when auth was added — a good reminder to actually compile after every change, not just trust it looks right.

## Known Limitations (by design, not oversights)
- Hash table has a fixed capacity (101 buckets); no dynamic resizing on high load factor.
- Persistence log grows unboundedly; no compaction/snapshotting implemented.
- Single shared API key, not per-user accounts.
- Entire dataset must fit in memory (log replay rebuilds everything in RAM on startup).

## Project Structure
```
mydb/
├── include/
│   ├── avl_tree.h       # Core AVL tree + Record schema
│   ├── hash_table.h      # Hash table (FNV-1a, separate chaining)
│   ├── persistence.h     # Append-only log writer/reader
│   ├── auth.h             # API key check
│   └── crow_all.h          # Crow framework (third-party, bundled)
├── src/
│   └── api.cpp              # Crow routes — thin, calls into headers above
├── tests/
│   ├── unit/
│   │   └── test_avl.cpp     # AVL tree + hash table correctness tests
│   └── integration/
│       └── test_api.sh      # Full API test suite (auth, CRUD, persistence)
├── benchmarks/
│   └── benchmark_avl.cpp    # Scaling benchmarks
└── data/
    └── log.txt              # Persistence log (generated at runtime)
```

## Future Directions
- Dynamic hash table resizing based on load factor
- Log compaction / periodic snapshots
- Per-owner passphrases instead of a single shared API key
- Reader-writer lock instead of a single global mutex
- Flexible (non-fixed) schema support
