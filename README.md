# Blocking vs Non-blocking Communication in MPI

Repository for the laboratory and teaching concept for the trial lecture **Blocking vs Non-blocking Communication in Message Passing Interface (MPI)**.

## Software used

- [OpenMPI](https://www.open-mpi.org/)
- [Score-P](https://www.vi-hps.org/projects/score-p/)
- [Vampir](https://vampir.eu/)
- [Cube](https://www.scalasca.org/software/cube/)
- [Scalasca](https://www.scalasca.org/software/scalasca/)

## Getting Started

### Hello World
Start with the simplest possible MPI program:

```bash
make
mpirun -np 4 ./out/hello_mpi
```
This demonstrates basic MPI startup and rank-based output.

## 🔁 Ping Pong Communication

We compare blocking vs. non-blocking communication:

### Basic Ping-Pong
```bash
make
mpirun -np 2 ./out/pingpong_blocking
mpirun -np 2 ./out/pingpong_nonblocking
```
Both variants simulate back-and-forth communication, but non-blocking allows overlap with computation.

### With Artificial Delay (Overlap)
```bash
mpirun -np 2 ./out/pingpong_blocking_overlap
mpirun -np 2 ./out/pingpong_nonblocking_overlap
```

#### Expected Timing Behavior

| Variant      | Delay Location       | Overlap? | Expected Time |
|--------------|----------------------|----------|---------------|
| Blocking     | After `MPI_Recv`     | ❌  No    | ~1.00 sec     |
| Non-blocking | During `MPI_Irecv`   | ✅ Yes    | ~0.50 sec     |

In the non-blocking case, the delay overlaps with message transfer.

## 📊 Performance Analysis with Score-P

### Build instrumented binaries:
```bash
make build-scorep
```

### Set environment variables:
```bash
export SCOREP_TIMER=clock_gettime
export SCOREP_ENABLE_TRACING=true
export SCOREP_ENABLE_PROFILING=true
```

### Run and store results:
```bash
mpirun --bind-to core -np 2 ./out/scorep/pingpong_blocking_overlap
mv scorep-* pingpong_blocking_overlap-scorep

mpirun --bind-to core -np 2 ./out/scorep/pingpong_nonblocking_overlap
mv scorep-* pingpong_nonblocking_overlap-scorep
```

### View in Cube
```bash
cube pingpong_blocking_overlap-scorep/profile.cubex &
cube pingpong_nonblocking_overlap-scorep/profile.cubex &
```

```bash
cube_merge pingpong_blocking_overlap-scorep/profile.cubex pingpong_nonblocking_overlap-scorep/profile.cubex -o compare-pingpong.cubex
cube compare-pingpong.cubex &
```

### View in Vampir
```bash
vampir pingpong_blocking_overlap-scorep/traces.otf2 &
vampir pingpong_nonblocking_overlap-scorep/traces.otf2 &
```
Use Vampir's timeline and message view to visualize overlap.

## 🚨 Late Sender / Late Receiver
Following examples simulate unbalanced communication:
- **Late Sender**: Receiver waits because sender is delayed
- **Late Receiver**: Sender waits because receiver is delayed

### Run and collect traces:
```bash
make build-scorep
export SCOREP_TIMER=clock_gettime
export SCOREP_ENABLE_TRACING=true
export SCOREP_ENABLE_PROFILING=true

mpirun --bind-to core -np 2 ./out/scorep/late_sender
mv scorep-* late_sender-scorep

mpirun --bind-to core -np 2 ./out/scorep/late_sender_nonblocking
mv scorep-* late_sender_nonblocking-scorep

mpirun --bind-to core -np 2 ./out/scorep/late_receiver_small
mv scorep-* late_receiver_small-scorep

mpirun --bind-to core -np 2 ./out/scorep/late_receiver_barrier
mv scorep-* late_receiver_barrier-scorep

mpirun --bind-to core -np 2 ./out/scorep/late_receiver_big
mv scorep-* late_receiver_big-scorep

mpirun --bind-to core -np 2 ./out/scorep/late_receiver_big_nonblocking
mv scorep-* late_receiver_big_nonblocking-scorep
```

### Analyze in Cube and Vampir
```bash
cube_merge late_sender-scorep/profile.cubex late_receiver_small-scorep/profile.cubex -o compare-late.cubex
cube compare-late.cubex &

cube late_sender_nonblocking-scorep/profile.cubex &
cube late_receiver_small-scorep/profile.cubex &
cube late_receiver_barrier-scorep/profile.cubex &
cube late_receiver_big-scorep/profile.cubex &
cube late_receiver_big_nonblocking-scorep/profile.cubex &

vampir late_sender-scorep/traces.otf2 & 
vampir late_sender_nonblocking-scorep/traces.otf2 & 

vampir late_receiver_small-scorep/traces.otf2 &
vampir late_receiver_barrier-scorep/traces.otf2 &
vampir late_receiver_big-scorep/traces.otf2 &
vampir late_receiver_big_nonblocking-scorep/traces.otf2 &
```

#### Comparison Table
| Scenario                   | Communication Type   | Visible Blocking Location      | Vampir Behavior                          | Cube Observation                                 |
|---------------------------|----------------------|-------------------------------|-------------------------------------------|--------------------------------------------------|
| Late Sender (blocking)    | MPI_Send / Recv      | `MPI_Recv` blocks on receiver | Rank 0 blocked in `MPI_Recv`              | High time in `MPI_Recv` on receiver              |
| Late Sender (non-blocking)| MPI_Isend / Irecv    | `MPI_Wait` on receiver        | Rank 0 waits in `MPI_Wait`                | Time in `MPI_Wait`, better overlap               |
| Late Receiver (blocking)  | MPI_Send / Recv      | `MPI_Send` blocks on sender   | Rank 1 blocked in `MPI_Send`              | High time in `MPI_Send` on sender                |
| Late Receiver (non-blocking)| MPI_Isend / Irecv  | `MPI_Wait` on sender          | Rank 1 waits in `MPI_Wait`                | Time in `MPI_Wait`, less blocking in `Send`      |


## 🧠 Advanced: Scalasca
Scalasca automatically detects communication inefficiencies:

```bash
export SCOREP_TIMER=clock_gettime
export SCOREP_ENABLE_TRACING=true
export SCOREP_ENABLE_PROFILING=true
make build-scorep

scalasca -analyze -t mpirun -np 2 ./out/scorep/late_receiver_big
square scorep_late_receiver_big_2_trace &

scalasca -analyze -t mpirun -np 2 ./out/scorep/late_receiver_big_nonblocking
square scorep_late_receiver_big_nonblocking_2_trace &
```
Scalasca's `square` viewer highlights wait-state imbalances.

## 👤 Contributor
- Jonas Posner
