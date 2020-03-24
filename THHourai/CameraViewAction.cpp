using namespace Touhou;

using namespace std;

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
		// 180 * 0.15의 회전 각도로 회전합니다.
		quat = Quaternion::AngleAxis( -3.14 * 0.15, Vector3::Right );

		// 거리를 0.5배로 낮춥니다.
		range = 0.5;

		// 타겟의 위치를 조정합니다.
		target = Vector3( 0, 1, 0 );
	}
	else
	{
		range = 1;
		target = 0;
	}

	if ( input.KeyState[KeyCode::Delete] )
	{
		// 180 * 0.15의 회전 각도로 회전합니다.
		quat *= Quaternion::AngleAxis( 3.14 * 0.35, Vector3::Up );
	}

	if ( input.KeyState[KeyCode::PageDown] )
	{
		// 180 * 0.15의 회전 각도로 회전합니다.
		quat *= Quaternion::AngleAxis( -3.14 * 0.35, Vector3::Up );
	}

	distance = clamp( distance + -input.DeltaScroll.Y * 0.1, 0.5, 10.0 );

	auto t = time.DeltaTimeInSeconds * rotateSpeed;

	quat = pquat.Slerp( quat, t );
	pquat = quat;

	prange = prange * ( 1 - t ) + range * distance * t;
	Transform->Position = ( quat * Quaternion( pos, 0 ) * quat.Conjugate ).V * prange;
	Transform->LookAt( ptarg );
}