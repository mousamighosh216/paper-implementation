# F-FS 🚀

A Distributed File System inspired by Google File System (GFS), implemented in C++.

---

## 📌 Overview

F-FS is a scalable distributed file system designed to handle large files across multiple storage nodes. It separates metadata management from data storage and implements replication, fault tolerance, and efficient write pipelines.

---

## 🧠 Architecture

The system consists of three main components:

### 🔹 Master

* Maintains filesystem metadata
* Manages chunk allocation and replication
* Handles lease-based consistency

### 🔹 ChunkServer

* Stores file chunks
* Handles read/write/append operations
* Participates in replication pipelines

### 🔹 Shadow Master

* Replicates metadata via operation logs
* Provides read-only failover

---

## ⚙️ Features

* ✅ Chunk-based file storage
* ✅ Primary-secondary replication model
* ✅ Write pipeline with consistency guarantees
* ✅ Atomic record append
* ✅ Garbage collection of orphaned chunks
* ✅ Snapshot support (Copy-on-Write)
* ✅ Write-ahead logging (OpLog)
* ✅ Shadow master replication

---

## 📂 Project Structure

```
/master        → Metadata & coordination
/chunkserver   → Storage nodes
/shadow        → Read-only replica
/protocol      → Networking & framing
/messages      → RPC message definitions
/storage       → OpLog & checkpointing
/network       → TCP abstraction
/utils         → Logging, configs
/bin           → Executables
```

---

## 🔄 Write Flow

1. Client requests chunk allocation from Master
2. Master assigns primary + replicas
3. Client pushes data to all replicas
4. Client sends write to primary
5. Primary commits locally and forwards to secondaries
6. Acknowledgment chain ensures consistency

---

## 🔄 Read Flow

1. Client queries Master for chunk locations
2. Reads directly from nearest ChunkServer

---

## 🛠️ Build Instructions

```bash
mkdir build && cd build
cmake ..
make
```

---

## ▶️ Running the System

### Start Master

```bash
./master_main
```

### Start ChunkServer

```bash
./chunkserver_main <addr> <data_dir> <master_addr>
```

### Start Shadow Master

```bash
./shadow_main <primary_addr>
```

---

## 🚀 Future Improvements

* Raft-based leader election
* Client-side SDK
* Metrics & monitoring (Prometheus)
* Failure simulation framework
* Tiered storage support
* Web dashboard

---

## 📖 Inspiration

* Google File System (GFS)
* Hadoop Distributed File System (HDFS)

---

## 👤 Author

Built as a deep systems project to explore distributed storage systems and large-scale architecture design.

---
