/*
 * fastq_filter.c
 *
 * A streaming FASTQ parser + filter engine.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * getline() is a POSIX function -- it's provided by Linux/macOS's C
 * library, but NOT by Windows' native C runtime, which MinGW targets by
 * default. So on Windows, the real getline() simply doesn't exist to link
 * against. We provide our own minimal replacement here, ONLY when
 * compiling for Windows -- on Linux/macOS we keep using the real,
 * battle-tested system one.
 *
 * This is the exact same "grow the buffer with realloc, only when we run
 * out of room" pattern as the grow_buffer() exercise -- just applied to
 * reading one character at a time from a file instead of copying a
 * known-length string.
 */
#ifdef _WIN32

typedef long long ssize_t; /* Windows' C runtime has no ssize_t either */

static ssize_t getline(char **lineptr, size_t *n, FILE *stream) {
    if (lineptr == NULL || n == NULL || stream == NULL) return -1;

    /* If the caller hasn't given us a buffer yet, start with a small one. */
    if (*lineptr == NULL || *n == 0) {
        *n = 128;
        *lineptr = malloc(*n);
        if (*lineptr == NULL) return -1;
    }

    size_t pos = 0;
    int c;

    while ((c = fgetc(stream)) != EOF) {
        /* Leave room for the '\0' we'll add at the end -- hence pos + 1 */
        if (pos + 1 >= *n) {
            size_t new_size = (*n) * 2;
            char *new_buf = realloc(*lineptr, new_size);
            if (new_buf == NULL) return -1;
            *lineptr = new_buf;
            *n = new_size;
        }
        (*lineptr)[pos++] = (char)c;
        if (c == '\n') break;
    }

    if (pos == 0 && c == EOF) {
        return -1; /* nothing read: clean EOF */
    }

    (*lineptr)[pos] = '\0';
    return (ssize_t)pos;
}

#endif /* _WIN32 */

typedef struct {
    FILE *fp;

    char *line_id;   size_t cap_id;
    char *line_seq;  size_t cap_seq;
    char *line_plus; size_t cap_plus;
    char *line_qual; size_t cap_qual;

    int min_qual;
    int min_len;
    char *adapter;

    long total_reads;
    long passed_reads;
} FastqFilter;

static void strip_newline(char *s) {
    if (!s) return;
    size_t len = strlen(s);
    while (len > 0 && (s[len - 1] == '\n' || s[len - 1] == '\r')) {
        s[len - 1] = '\0';
        len--;
    }
}

FastqFilter *fastq_filter_open(const char *path, int min_qual, int min_len, const char *adapter) {
    FastqFilter *f = calloc(1, sizeof(FastqFilter));
    if (!f) return NULL;

    f->fp = fopen(path, "r");
    if (!f->fp) {
        free(f);
        return NULL;
    }

    f->min_qual = min_qual;
    f->min_len = min_len;

    if (adapter && adapter[0] != '\0') {
        f->adapter = strdup(adapter);
    } else {
        f->adapter = NULL;
    }

    return f;
}

int fastq_filter_next(FastqFilter *f, char *out_id, char *out_seq, char *out_qual, int bufsize) {
    if (!f || !f->fp) return 0;

    for (;;) {
        ssize_t r1 = getline(&f->line_id, &f->cap_id, f->fp);
        if (r1 == -1) return 0;

        ssize_t r2 = getline(&f->line_seq, &f->cap_seq, f->fp);
        ssize_t r3 = getline(&f->line_plus, &f->cap_plus, f->fp);
        ssize_t r4 = getline(&f->line_qual, &f->cap_qual, f->fp);
        if (r2 == -1 || r3 == -1 || r4 == -1) {
            return 0;
        }

        strip_newline(f->line_id);
        strip_newline(f->line_seq);
        strip_newline(f->line_plus);
        strip_newline(f->line_qual);

        if (f->line_id[0] != '@' || f->line_plus[0] != '+') {
            return -1;
        }

        f->total_reads++;

        int len = (int)strlen(f->line_seq);

        if (len < f->min_len) continue;

        if (f->adapter && strstr(f->line_seq, f->adapter) != NULL) continue;

        long qsum = 0;
        for (int i = 0; i < len; i++) {
            qsum += (unsigned char)f->line_qual[i] - 33;
        }
        double avg_q = (len > 0) ? ((double)qsum / len) : 0.0;
        if (avg_q < f->min_qual) continue;

        f->passed_reads++;
        snprintf(out_id, bufsize, "%s", f->line_id + 1);
        snprintf(out_seq, bufsize, "%s", f->line_seq);
        snprintf(out_qual, bufsize, "%s", f->line_qual);
        return 1;
    }
}

void fastq_filter_stats(FastqFilter *f, long *total, long *passed) {
    if (!f) return;
    if (total) *total = f->total_reads;
    if (passed) *passed = f->passed_reads;
}

void fastq_filter_close(FastqFilter *f) {
    if (!f) return;
    if (f->fp) fclose(f->fp);
    free(f->line_id);
    free(f->line_seq);
    free(f->line_plus);
    free(f->line_qual);
    free(f->adapter);
    free(f);
}
