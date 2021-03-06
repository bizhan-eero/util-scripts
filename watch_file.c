/******************************************************************************
*									      *
*	File:     watch_file.c						      *
*	Author:   Michael Kerrisk with some minor changes by Alison Chaiken   *
*	Created:  Sun Mar 15 17:49:07 PDT 2015				      *
*	Contents: basically Michael Kerrisk's demo_inotify.c from TLPI        *
*                 http://www.man7.org/tlpi/code/online/dist/inotify/demo_inotify.c.html
*									      *
*									      *
******************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/inotify.h>
#include <limits.h>

#define BUF_LEN (10 * (sizeof(struct inotify_event) + NAME_MAX + 1))

static void             /* Display information from inotify_event structure */
displayInotifyEvent(struct inotify_event *i)
{
    printf("    wd =%2d; ", i->wd);
    if (i->cookie > 0)
        printf("cookie =%4d; ", i->cookie);

    printf("mask = ");
    if (i->mask & IN_ACCESS)        printf("IN_ACCESS ");
    if (i->mask & IN_ATTRIB)        printf("IN_ATTRIB ");
    if (i->mask & IN_CLOSE_NOWRITE) printf("IN_CLOSE_NOWRITE ");
    if (i->mask & IN_CLOSE_WRITE)   printf("IN_CLOSE_WRITE ");
    if (i->mask & IN_CREATE)        printf("IN_CREATE ");
    if (i->mask & IN_DELETE)        printf("IN_DELETE ");
    if (i->mask & IN_DELETE_SELF)   printf("IN_DELETE_SELF ");
    if (i->mask & IN_IGNORED)       printf("IN_IGNORED ");
    if (i->mask & IN_ISDIR)         printf("IN_ISDIR ");
    if (i->mask & IN_MODIFY)        printf("IN_MODIFY ");
    if (i->mask & IN_MOVE_SELF)     printf("IN_MOVE_SELF ");
    if (i->mask & IN_MOVED_FROM)    printf("IN_MOVED_FROM ");
    if (i->mask & IN_MOVED_TO)      printf("IN_MOVED_TO ");
    if (i->mask & IN_OPEN)          printf("IN_OPEN ");
    if (i->mask & IN_Q_OVERFLOW)    printf("IN_Q_OVERFLOW ");
    if (i->mask & IN_UNMOUNT)       printf("IN_UNMOUNT ");
    printf("\n");

    if (i->len > 0)
        printf("        name = %s\n", i->name);
}

int main(int argc, char **argv) {
	int inotifyFD, wd, j;
	char buf[BUF_LEN];
	ssize_t numRead;
	char *p;
	struct inotify_event *event;

	if (argc < 2) {
		printf("%s pathname . . .\n", argv[0]);
		exit(-1);
	}

	inotifyFD = inotify_init();
	if (inotifyFD == -1) {
		printf("%s inotify_init() failure\n", argv[0]);
		exit(-2);
	}

	for (j = 1; j < argc; j++) {
		wd = inotify_add_watch(inotifyFD, argv[j], IN_ALL_EVENTS);
		if (wd == -1) {
			printf("%s inotify_add_watch() failure\n", argv[0]);
			exit(-3);
		}

		printf("Watching %s using wd %d\n", argv[j], wd);
	}

	for (;;) {
		numRead = read(inotifyFD, buf, BUF_LEN);
		if (numRead <= 0) {
			printf("%s failed to read inotifyFD\n", argv[0]);
			exit(-4);
		}

		for (p = buf; p < buf + numRead; ) {
			event = (struct inotify_event *) p;
			displayInotifyEvent(event);

			p += sizeof(struct inotify_event) + event->len;
		}
	}

	exit(EXIT_SUCCESS);
}
