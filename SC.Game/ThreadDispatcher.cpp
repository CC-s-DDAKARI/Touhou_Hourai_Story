using namespace SC;
using namespace SC::Game;

ThreadDispatcher::ThreadDispatcher() : Component()
{

}

object ThreadDispatcher::Clone()
{
	var clone = new ThreadDispatcher();
	clone->threadId = threadId;
	return clone;
}

int ThreadDispatcher::ThreadID_get()
{
	return threadId;
}

void ThreadDispatcher::ThreadID_set( int value )
{
	threadId = value;
}