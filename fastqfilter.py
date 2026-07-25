"""
fastqfilter.py

Pythonic wrapper around libfastqfilter.so, using ctypes.

ctypes in one paragraph: a .so file is just compiled machine code with a
table of exported function names (symbols). ctypes.CDLL() loads that file
into your Python process and lets you call those functions directly --
but Python has no idea what types those functions expect, so YOU have to
tell it, via .argtypes and .restype. Get those wrong and you'll segfault
the process instead of getting a nice Python exception, so we're careful
here.
"""

import ctypes
import os
from collections import namedtuple

# Record returned to the user: three plain Python strings.
FastqRecord = namedtuple("FastqRecord", ["id", "seq", "qual"])

# Buffer sizes for the fixed-size C-side output buffers. Nanopore reads can
# be very long, so we size generously. If you ever hit reads longer than
# this, bump it here (and recompile is NOT required -- this is Python-side).
_ID_BUFSIZE = 1024
_SEQ_BUFSIZE = 1 << 20  # 1 MiB, comfortably covers ultra-long reads

_lib = None


def _load_lib():
    """Load the shared library once and cache it at module level."""
    global _lib
    if _lib is not None:
        return _lib

    here = os.path.dirname(os.path.abspath(__file__))
    lib_path = os.path.join(here, "libfastqfilter.so")
    lib = ctypes.CDLL(lib_path)

    # --- Tell ctypes the real C signatures. This is the part that's easy
    # to get wrong and dangerous if you do, so we spell every one out. ---

    lib.fastq_filter_open.argtypes = [
        ctypes.c_char_p,  # path
        ctypes.c_int,     # min_qual
        ctypes.c_int,     # min_len
        ctypes.c_char_p,  # adapter
    ]
    lib.fastq_filter_open.restype = ctypes.c_void_p  # opaque handle

    lib.fastq_filter_next.argtypes = [
        ctypes.c_void_p,  # handle
        ctypes.c_char_p,  # out_id buffer
        ctypes.c_char_p,  # out_seq buffer
        ctypes.c_char_p,  # out_qual buffer
        ctypes.c_int,     # bufsize
    ]
    lib.fastq_filter_next.restype = ctypes.c_int

    lib.fastq_filter_stats.argtypes = [
        ctypes.c_void_p,
        ctypes.POINTER(ctypes.c_long),
        ctypes.POINTER(ctypes.c_long),
    ]
    lib.fastq_filter_stats.restype = None

    lib.fastq_filter_close.argtypes = [ctypes.c_void_p]
    lib.fastq_filter_close.restype = None

    _lib = lib
    return _lib


class FastqFilter:
    """
    Streams FASTQ records from `path`, yielding only records that pass
    the given filters. Backed by a C engine, so it's fast and holds only
    one record's worth of data in memory at a time -- safe for huge files.

    Usage:
        with FastqFilter("reads.fastq", min_qual=20, min_len=50) as ff:
            for record in ff:
                print(record.id, len(record.seq))
            print(ff.stats())
    """

    def __init__(self, path, min_qual=0, min_len=0, adapter=None):
        self._lib = _load_lib()
        self._handle = self._lib.fastq_filter_open(
            str(path).encode("utf-8"),
            int(min_qual),
            int(min_len),
            (adapter or "").encode("utf-8"),
        )
        if not self._handle:
            raise IOError(f"could not open FASTQ file: {path}")

        # Reusable output buffers -- allocated once, reused every call.
        self._id_buf = ctypes.create_string_buffer(_ID_BUFSIZE)
        self._seq_buf = ctypes.create_string_buffer(_SEQ_BUFSIZE)
        self._qual_buf = ctypes.create_string_buffer(_SEQ_BUFSIZE)

    def __iter__(self):
        return self

    def __next__(self):
        if self._handle is None:
            raise StopIteration

        ret = self._lib.fastq_filter_next(
            self._handle,
            self._id_buf,
            self._seq_buf,
            self._qual_buf,
            _SEQ_BUFSIZE,
        )
        if ret == 0:
            raise StopIteration
        if ret == -1:
            raise ValueError("malformed FASTQ record encountered")

        return FastqRecord(
            id=self._id_buf.value.decode("utf-8", errors="replace"),
            seq=self._seq_buf.value.decode("utf-8", errors="replace"),
            qual=self._qual_buf.value.decode("utf-8", errors="replace"),
        )

    def stats(self):
        """Return {'total': N, 'passed': M} read counts seen so far."""
        total = ctypes.c_long(0)
        passed = ctypes.c_long(0)
        self._lib.fastq_filter_stats(
            self._handle, ctypes.byref(total), ctypes.byref(passed)
        )
        return {"total": total.value, "passed": passed.value}

    def close(self):
        if self._handle is not None:
            self._lib.fastq_filter_close(self._handle)
            self._handle = None

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        self.close()

    def __del__(self):
        # Safety net in case someone forgets to close()/use `with`.
        try:
            self.close()
        except Exception:
            pass
