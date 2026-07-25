from fastqfilter import FastqFilter

print("=== Filtering test_reads.fastq (min_qual=20, min_len=50, adapter=AGATCGGAAGAGC) ===\n")

with FastqFilter("test_reads.fastq", min_qual=20, min_len=50, adapter="AGATCGGAAGAGC") as ff:
    for rec in ff:
        print(f"PASSED: {rec.id:20s} len={len(rec.seq):5d}  seq[:20]={rec.seq[:20]}...")
    print()
    print("Stats:", ff.stats())
