using namespace SC;
using namespace SC::Threading;

namespace
{
	struct MyWorkItem
	{
		AsyncTaskDelegate<void> Task;
		object Args;
	};

	DWORD WINAPI StartRoutine( LPVOID lpThreadParameter )
	{
		RefPtr workItem = ( ThreadWorkItem* )lpThreadParameter;
		workItem->IsLocked = false;
		workItem->Run();

		return 0;
	}

	DWORD WINAPI StartRoutine1( LPVOID lpThreadParameter )
	{
		std::unique_ptr<MyWorkItem> item( ( MyWorkItem* )lpThreadParameter );
		item->Task( item->Args );
		return 0;
	}
}

RefPtr<IAsyncOperation<object>> ThreadPool::QueueUserWorkItem( AsyncTaskDelegate<object> asyncTask, object args )
{
	RefPtr workItem = new ThreadWorkItem( asyncTask, args );
	workItem->IsLocked = true;
	::QueueUserWorkItem( StartRoutine, workItem.Get(), WT_EXECUTEDEFAULT );
	return workItem;
}

void ThreadPool::QueueUserWorkItem( AsyncTaskDelegate<void> asyncTask, object args )
{
	::QueueUserWorkItem( StartRoutine1, new MyWorkItem{ asyncTask, args }, WT_EXECUTEDEFAULT );
}

RefPtr<IAsyncOperation<object>> ThreadPool::CreateTask( AsyncTaskDelegate<object> asyncTask, object args )
{
	RefPtr workItem = new ThreadWorkItem( asyncTask, args );
	workItem->IsLocked = true;
	::QueueUserWorkItem( StartRoutine, workItem.Get(), WT_EXECUTELONGFUNCTION );
	return workItem;
}