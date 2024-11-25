#pragma D option quiet

syscall::read:entry
{
	self->t = timestamp;
}

syscall::sendto:return
{
	printf("%d\n", (timestamp - self->t));
}
