#pragma once

namespace SC::Game::Details
{
	class AsyncLoad : virtual public Object, virtual public IAsyncLoad
	{
		RefPtr<IAsyncOperation<object>> asyncLoad;
		double progress = 0;

	public:
		AsyncLoad();

		void Bind( RefPtr<IAsyncOperation<object>> asyncLoad );
		void Cancel() override;
		void Close() override;
		int64 Id_get() override;
		RefPtr<Exception> Error_get() override;
		Threading::AsyncStatus Status_get() override;
		object GetResults() override;
		RefPtr<IAsyncOperation<object>> Then( Threading::AsyncTaskChainDelegate<object> asyncTask, object args ) override;
		double Progress_get() override;
		void Progress_set( double value ) override;

	private:
		void OnCanceled( object sender );
	};
}