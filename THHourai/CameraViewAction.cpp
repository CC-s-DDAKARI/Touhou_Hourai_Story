using namespace Touhou;

CameraViewAction::CameraViewAction() : Component()
{

}

object CameraViewAction::Clone()
{
	return new CameraViewAction();
}

void CameraViewAction::Start()
{
	Transform->Position = Vector3( 0, 8, -7 );
	Transform->LookAt( Vector3::Zero );
}

void CameraViewAction::Update( Time& time, Input& input )
{
	auto pos = Vector3( 0, 8, -7 );
	Quaternion quat;

	if ( input.KeyState[KeyCode::End] )
	{
		// 180 * 0.15�� ȸ�� ������ ȸ���մϴ�.
		quat = Quaternion::AngleAxis( -3.14 * 0.15, Vector3::Right );

		// �Ÿ��� 0.5��� ����ϴ�.
		range = 0.5;

		// Ÿ���� ��ġ�� �����մϴ�.
		target = Vector3( 0, 1, 0 );
	}
	else
	{
		range = 1;
		target = 0;
	}

	if ( input.KeyState[KeyCode::Delete] )
	{
		// 180 * 0.15�� ȸ�� ������ ȸ���մϴ�.
		quat *= Quaternion::AngleAxis( 3.14 * 0.35, Vector3::Up );
	}

	if ( input.KeyState[KeyCode::PageDown] )
	{
		// 180 * 0.15�� ȸ�� ������ ȸ���մϴ�.
		quat *= Quaternion::AngleAxis( -3.14 * 0.35, Vector3::Up );
	}

	auto t = time.DeltaTimeInSeconds * rotateSpeed;

	quat = pquat.Slerp( quat, t );
	pquat = quat;

	prange = prange * ( 1 - t ) + range * t;
	Transform->Position = ( quat * Quaternion( pos, 0 ) * quat.Conjugate ).V * prange;
	Transform->LookAt( ptarg );
}