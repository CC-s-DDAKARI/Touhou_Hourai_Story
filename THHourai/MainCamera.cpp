using namespace Touhou;

using namespace std;

mutex MainCamera::locker;

MainCamera::MainCamera() : GameObject( "mainCamera" )
{
	IsLocked = true;

	var gameObject = new GameObject( "cameraPivot" );
	gameObject->Parent = this;

	var cam = gameObject->AddComponent<Camera>();
	cam->ClearMode = new CameraClearModeSkybox( new Texture2D( "skybox_texture", "Assets/Texture/DaylightSkybox.png" ) );

	// ���ͺ� ������ �����մϴ�.
	gameObject->AddComponent<CameraViewAction>();

	IsLocked = false;
}

RefPtr<MainCamera> MainCamera::GetInstance()
{
	lock_guard<mutex> lock( locker );
	static var obj = new MainCamera();
	return obj;
}