# 패치 노트
## 2020-03-14. Ver 0.1.0
- 패치 노트 작성을 시작했습니다.
- PhysX 물리 엔진 기능에 Capsule Collider를 추가하였습니다. 이제 gameObject->AddComponent<CapsuleCollider>() 코드를 사용해 CapsuleCollider를 Rigidbody를 사용하는 게임 오브젝트 및 Rigidbody를 사용하지 않는 Static Rigidbody 게임 오브젝트에 사용할 수 있습니다.
- 모든 하위 물리 오브젝트가 부모 트랜스폼의 영향을 받도록 수정했습니다.
- Rigidbody의 성능을 대폭 개선했습니다. 이제 더 이상 매 프레임 Rigidbody 목록 및 Collider 목록을 재구성 하지 않습니다.
