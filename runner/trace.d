#pragma D option quiet

syscall::getpid:return
{
	self->t = timestamp;
}

syscall::getpgrp:entry
{
	printf("%d\n", (timestamp - self->t));
}
