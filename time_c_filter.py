import time
from fastqfilter import FastqFilter

start = time.time()
with FastqFilter("big_test.fastq", min_qual=20, min_len=50) as ff:
    passed = sum(1 for _ in ff)
    stats = ff.stats()
elapsed = time.time() - start
print(f"[C via ctypes] total={stats['total']} passed={stats['passed']} time={elapsed:.3f}s")
