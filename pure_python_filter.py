import time

def filter_fastq_python(path, min_qual=20, min_len=50, adapter=None):
    total = 0
    passed = 0
    with open(path) as fh:
        while True:
            header = fh.readline()
            if not header:
                break
            seq = fh.readline().rstrip("\n")
            plus = fh.readline()
            qual = fh.readline().rstrip("\n")
            total += 1

            if len(seq) < min_len:
                continue
            if adapter and adapter in seq:
                continue
            avg_q = sum(ord(c) - 33 for c in qual) / len(qual)
            if avg_q < min_qual:
                continue
            passed += 1
    return total, passed


if __name__ == "__main__":
    start = time.time()
    total, passed = filter_fastq_python("big_test.fastq", min_qual=20, min_len=50)
    elapsed = time.time() - start
    print(f"[pure Python] total={total} passed={passed} time={elapsed:.3f}s")
