#pragma once

namespace Touhou
{
	class CameraViewAction : public Component
	{
		double rotateSpeed = 5;
		double range = 1;

		double prange = 1;

		Quaternion pquat;
		Vector3 ptarg;
		Vector3 target;

	public:
		CameraViewAction();

		object Clone() override;

		void Start() override;
		void Update( Time& time, Input& input ) override;
	};
}