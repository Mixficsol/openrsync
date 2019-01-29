/*	$Id$ */
/*
 * Copyright (c) 2019 Kristaps Dzonsons <kristaps@bsd.lv>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */
#include <sys/param.h>

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

#include "extern.h"

/*
 * TODO.
 */
int
sess_stats_send(struct sess *sess, int fd)
{

	assert(sess->opts->server);

	if ( ! io_write_int(sess, fd, 10)) {
		ERRX1(sess, "io_write_int: total read");
		return 0;
	} else if ( ! io_write_int(sess, fd, 20)) {
		ERRX1(sess, "io_write_int: total write");
		return 0;
	} else if ( ! io_write_int(sess, fd, 30)) {
		ERRX1(sess, "io_write_int: total size");
		return 0;
	}

	return 1;
}

/*
 * At the end of the transmission, we have some statistics to read.
 * Return zero on failure, non-zero on success.
 */
int
sess_stats_recv(struct sess *sess, int fd)
{
	size_t	 	 tread, twrite, tsize;
	float		 tr, tw, ts;
	const char	*tru = "B", *twu = "B", *tsu = "B";
	int		 trsz = 0, twsz = 0, tssz = 0;

	assert( ! sess->opts->server);

	if ( ! io_read_size(sess, fd, &tread)) {
		ERRX1(sess, "io_read_size: total read");
		return 0;
	} else if ( ! io_read_size(sess, fd, &twrite)) {
		ERRX1(sess, "io_read_size: total write");
		return 0;
	} else if ( ! io_read_size(sess, fd, &tsize)) {
		ERRX1(sess, "io_read_size: total size");
		return 0;
	}

	if (tread >= 1024 * 1024 * 1024) {
		tr = tread / (1024.0 * 1024.0 * 1024.0);
		tru = "GB";
		trsz = 3;
	} else if (tread >= 1024 * 1024) {
		tr = tread / (1024.0 * 1024.0);
		tru = "MB";
		trsz = 2;
	} else if (tread >= 1024) {
		tr = tread / 1024.0;
		tru = "KB";
		trsz = 1;
	} else
		tr = tread;

	if (twrite >= 1024 * 1024 * 1024) {
		tw = twrite / (1024.0 * 1024.0 * 1024.0);
		twu = "GB";
		twsz = 3;
	} else if (twrite >= 1024 * 1024) {
		tw = twrite / (1024.0 * 1024.0);
		twu = "MB";
		twsz = 2;
	} else if (twrite >= 1024) {
		tw = twrite / 1024.0;
		twu = "KB";
		twsz = 1;
	} else
		tw = twrite;

	if (tsize >= 1024 * 1024 * 1024) {
		ts = tsize / (1024.0 * 1024.0 * 1024.0);
		tsu = "GB";
		tssz = 3;
	} else if (tsize >= 1024 * 1024) {
		ts = tsize / (1024.0 * 1024.0);
		tsu = "MB";
		tssz = 2;
	} else if (tsize >= 1024) {
		ts = tsize / 1024.0;
		tsu = "KB";
		tssz = 1;
	} else
		ts = tsize;

	LOG1(sess, "Transfer complete: "
		"%.*f %s read, "
		"%.*f %s sent, "
		"%.*f %s file size",
		trsz, tr, tru, 
		twsz, tw, twu, 
		tssz, ts, tsu);
	return 1;
}

