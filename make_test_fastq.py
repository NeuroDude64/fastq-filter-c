"""Generates a small synthetic FASTQ file with known good/bad reads,
so we can verify the C filter engine does what it claims."""
import random

random.seed(42)
BASES = "ACGT"
ADAPTER = "AGATCGGAAGAGC"  # a real Illumina adapter sequence, used as our test adapter


def rand_seq(n):
    return "".join(random.choice(BASES) for _ in range(n))


def qual_string(n, phred):
    """All bases at a fixed phred quality, phred+33 encoded."""
    return chr(phred + 33) * n


records = []

# 1. A normal good read: length 100, quality 35 -> should PASS (min_qual=20, min_len=50)
records.append(("read_good_1", rand_seq(100), qual_string(100, 35)))

# 2. Too short: length 30 -> should FAIL length filter
records.append(("read_short", rand_seq(30), qual_string(30, 35)))

# 3. Low quality: length 100, quality 10 -> should FAIL quality filter
records.append(("read_lowqual", rand_seq(100), qual_string(100, 10)))

# 4. Contains adapter contamination -> should FAIL adapter filter
seq_with_adapter = rand_seq(40) + ADAPTER + rand_seq(40)
records.append(("read_adapter", seq_with_adapter, qual_string(len(seq_with_adapter), 35)))

# 5. Another good read, borderline quality right at threshold (avg ~20)
records.append(("read_borderline", rand_seq(80), qual_string(80, 20)))

# 6. Good read, longer (simulate a long nanopore-ish read)
records.append(("read_long_good", rand_seq(5000), qual_string(5000, 30)))

with open("test_reads.fastq", "w") as fh:
    for rid, seq, qual in records:
        fh.write(f"@{rid}\n{seq}\n+\n{qual}\n")

print(f"Wrote {len(records)} records to test_reads.fastq")
