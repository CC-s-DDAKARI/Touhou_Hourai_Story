using namespace SC;
using namespace SC::Game::Details;

using namespace std;

void GarbageCollector::Add( const object& garbage )
{
	lock_guard<mutex> locker( lock );
	if ( !AppShutdown )
		garbages.push( { garbage, frameIndex } );
}

void GarbageCollector::Collect()
{
	lock_guard<mutex> locker( lock );
	while ( garbages.size() )
	{
		if ( garbages.front().second <= frameIndex - 4 )
		{
			garbages.pop();
		}
		else
		{
			break;
		}
	}

	frameIndex += 1;
}

void GarbageCollector::CollectAll()
{
	lock_guard<mutex> locker( lock );
	while ( garbages.size() )
	{
		garbages.pop();
	}
}