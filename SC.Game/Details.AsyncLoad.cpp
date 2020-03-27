using namespace SC;
using namespace SC::Game;
using namespace SC::Game::Details;
using namespace SC::Threading;

using namespace std;

AsyncLoad::AsyncLoad()
{

}

void AsyncLoad::Bind( RefPtr<IAsyncOperation<object>> asyncLoad )
{
	this->asyncLoad = asyncLoad;
	this->asyncLoad->Canceled += EventHandler<>( this, &AsyncLoad::OnCanceled );
}

void AsyncLoad::Cancel()
{
	asyncLoad->Cancel();
}

void AsyncLoad::Close()
{
	asyncLoad->Close();
}

int64 AsyncLoad::Id_get()
{
	return asyncLoad->Id;
}

RefPtr<Exception> AsyncLoad::Error_get()
{
	return asyncLoad->Error;
}

AsyncStatus AsyncLoad::Status_get()
{
	return asyncLoad->Status;
}

object AsyncLoad::GetResults()
{
	return asyncLoad->GetResults();
}

RefPtr<IAsyncOperation<object>> AsyncLoad::Then( AsyncTaskChainDelegate<object> asyncTask, object args )
{
	return asyncLoad->Then( asyncTask, args );
}

double AsyncLoad::Progress_get()
{
	return progress;
}

void AsyncLoad::Progress_set( double value )
{
	progress = std::clamp<double>( value, 0, 1 );
}

void AsyncLoad::OnCanceled( object sender )
{
	Canceled( sender );
}