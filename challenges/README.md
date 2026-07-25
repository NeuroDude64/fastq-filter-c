C fundamentals challenges

A set of small, self-contained exercises I worked through to build up the C skills needed for fastq_filter.c — structs, pointers, manual memory management — before writing the real thing. Each one maps directly to a pattern used in the actual filter engine.

File	Concept	Where it shows up in fastq_filter.c

cStruct.c	Structs, pass-by-value	Basic struct usage (FastqFilter fields)

cStruct_pointer.c	Pass-by-value vs. pass-by-pointer, . vs ->	Every function takes FastqFilter *f and mutates it through the pointer

Challenge2.1.c	Pointer-to-pointer (int **)	getline(&f->line_id, &f->cap_id, f->fp) needs to reassign the caller's pointer, not just use it

Challenge_2.2.c	realloc, growable buffers	The buffer-growth logic inside getline (and the custom getline replacement written for Windows/MinGW, which doesn't provide one natively)

Challenge3.1.c	Dynamic arrays, opaque struct ownership	The general "own your allocations, free them all in the right order" discipline used throughout fastq_filter.c

Each file is standalone and compiles on its own, e.g.:

bash
gcc -Wall -Wextra -o point3d 1_1_point3d.c -lm
./point3d

These aren't meant to be polished — they're working notes from learning, kept as-is (including some of my own comments-to-self while figuring things out) rather than cleaned up after the fact.
