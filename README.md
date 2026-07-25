# fastq-filter-c

A streaming FASTQ quality/length/adapter filter, written in C for speed and
exposed to Python via `ctypes`. Built as a learning project to combine C
fundamentals (pointers, manual memory management, structs) with a real
bioinformatics use case.

Filters reads on:
- **Minimum length**
- **Minimum average Phred quality** (Phred+33 encoding)
- **Adapter contamination** (exact substring match)

Roughly **4x faster** than an equivalent pure-Python implementation on a
300k-read test file (see [`pure_python_filter.py`](pure_python_filter.py) /
[`time_c_filter.py`](time_c_filter.py) for the comparison).

## How it works

- **`fastq_filter.c`** — the actual parsing/filtering engine. Streams a
  FASTQ file four lines at a time, checking each read against the
  configured filters, and only returns reads that pass. No dependencies
  beyond the standard C library.
- **`fastqfilter.py`** — a Python wrapper around the compiled C library
  using `ctypes`, exposing a plain, Pythonic iterator:

  ```python
  from fastqfilter import FastqFilter

  with FastqFilter("reads.fastq", min_qual=20, min_len=50, adapter="AGATCGGAAGAGC") as ff:
      for record in ff:
          print(record.id, record.seq, record.qual)
      print(ff.stats())  # {'total': N, 'passed': M}
  ```

`fastq_filter.c` has no `main()` — it's a library, not a standalone
program. It only does anything once it's compiled into a shared library
and called into from `fastqfilter.py`.

## Building

This project is developed and tested on **Linux / WSL**. Native Windows
builds (MinGW) are possible but have known DLL-loading friction (MinGW
runtime dependency conflicts, `WinError 193`) that aren't fully resolved
here — WSL is the recommended path on Windows.

### Linux / WSL / macOS

```bash
gcc -shared -fPIC -O2 -Wall -Wextra -o libfastqfilter.so fastq_filter.c
```

### Windows (native, via MinGW) — use at your own risk

```powershell
gcc -shared -o fastq_filter.dll fastq_filter.c -O2 -Wall -Wextra -static
```

`fastqfilter.py` auto-detects which file to load (`libfastqfilter.so` /
`fastq_filter.dll` / `libfastqfilter.dylib`) based on the OS at runtime, via
`platform.system()`.

## Running the tests

```bash
python3 make_test_fastq.py   # generates test_reads.fastq, a small synthetic file
python3 test_run.py          # runs the filter against it and prints results
```

## Project structure

```
fastq_filter.c          C parsing/filtering engine
fastqfilter.py           ctypes wrapper / Python API
make_test_fastq.py        generates a small synthetic test FASTQ file
test_run.py                runs the filter end-to-end and prints results
pure_python_filter.py      pure-Python equivalent, for timing comparison
time_c_filter.py            timing harness for the C-backed version
```

(Compiled outputs like `*.so` / `*.dll` / `*.o` are not checked in — see
`.gitignore` — build them locally from source.)

## Known limitations / next steps

- Adapter filtering is exact-substring matching only; doesn't catch
  partial adapter matches at read ends (unlike `cutadapt`/`fastp`).
- No gzip support yet — plain-text FASTQ only.
- Rejects reads outright rather than trimming (e.g. no adapter trimming or
  quality-based end-trimming yet).
- No BAM/VCF support yet — planned to link against `htslib` rather than
  reimplementing those binary formats from scratch.

## Background

Built as part of a self-directed C learning project — see the C challenge
exercises (structs, pointers-to-pointers, `malloc`/`realloc`, opaque handle
patterns) that led up to this, documented separately.
