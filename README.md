# 패치 노트
## 2020-03-26. ver 0.4.1.1415
* HeapAllocator 클래스에서 Expand 함수 호출 시 데이터 공간 확장과 함께 이전 데이터가 소멸하는 문제를 수정하였습니다.
## 2020-03-26. ver 0.4.0.1219
* 엔진 내부 클래스의 관계를 개선했습니다.
* HeapAllocator 클래스가 생겼습니다.
  * 이제 매 프레임 갱신되는 것이 아닌 모든 개체는 HeapAllocator에 의해 Default GPU 힙에 생성됩니다.
  * Default GPU 힙은 갱신 요청시에만 갱신되며, 병합 기능을 통해 드로우 콜을 최적화합니다.
* LargeHeap 클래스가 생겼습니다.
  * 정기적 갱신이 필요하지만 매 프레임 갱신하는 것이 아닌 버퍼는 LargeHeap 클래스를 사용합니다.
  * HeapAllocator에서 할당한 개체와 비슷하게 작동하지만, 단일 리소스를 사용하며 Allocator의 영향을 받지 않습니다.
  * 단, Alloctor 클래스에 등록되어 Commit 명령을 같이 수행합니다.
* 더 이상 렌더링을 별도의 전용 스레드에서 진행하지 않습니다.
  * 성능 이익에 비해 유지가 매우 어려운 단점이 있었습니다.
  * Release 모드 시 드로우콜을 진행하는 동안 CPU를 충분히 활용하기 때문에 렌더링 전용 스레드를 사용해서 얻을 수 있는 이익이 거의 없었습니다.
  * 여전히 멀티 스레드를 이용하여 명령 목록을 작성합니다.
## 2020-03-24. ver 0.3.0.778
* Transform 컴포넌트에 ActualPosition, ActualScale, ActualRotation, ActualForward 속성이 추가되었습니다.
  * 이 속성은 렌더링 시 정확한 월드 변환 데이터를 제공합니다.
  * 마지막 Update 이후 적용되는 속성입니다.
* PhysX 물리 엔진에서 충돌 리포트 및 트리거 기능을 추가하였습니다.
  * 게임 오브젝트 및 컴포넌트에서 OnCollisionEnter/Exit/Stay 함수를 재정의하여 기능을 만들 수 있습니다.
  * Collider 컴포넌트에서 IsTrigger 속성에 true를 넣음으로 트리거 기능을 활성화할 수 있습니다.
  * 트리거는 OnTriggerEnter/Exit/Stay 함수를 재정의하여 기능을 만들 수 있습니다.
* Transform 업데이트 성능을 크게 개선했습니다.
  * 이제 게임 오브젝트가 Static 리지드바디를 사용하는 경우 더 이상 매 프레임 갱신하지 않습니다.
* Terrain 컴포넌트 및 HeightMap 텍스처 Asset이 추가되었습니다.
* TerrainCollider 컴포넌트가 추가되었습니다.
## 2020-03-21. ver 0.2.1.427
* 멀티 스레드 렌더링 성능 개선
  * 모든 명령 목록 작성에 새로운 스레드를 효과적으로 배치하도록 개선했습니다.
  * 렌더링 전체를 하나의 추가 스레드로 배치하였습니다.
  * 이제 CPU 업데이트, 명령 목록 작성, 그리고 GPU 명령 실행 3개의 항목이 동시에 진행됩니다.
  * 멀티스레드 환경에서 동기 상태가 제대로 적용되지 않아 Reset 오류가 발생하던 문제를 수정하였습니다.
* 애니메이션 오류 수정
  * 스키닝 셰이더에서 LOWORD 및 HIWORD 분리의 잘못됨을 수정하였습니다.
## 2020-03-16. ver 0.2.0.331
* 메쉬 스키닝 셰이더 추가
  * 이제 매 라이팅/기하 버퍼 작성 시 새로 스키닝 메쉬를 계산하지 않습니다.
  * 대신 스키닝 메쉬의 분량만큼 그래픽 메모리를 사용하여 스키닝 메쉬 데이터를 저장하게 됩니다.
* 입력 서명 번호를 더 이상 문자열로 검색하지 않습니다. 상수 연결을 사용하니다.
* 비슷한 입력 서명 개체를 사용하는 셰이더를 묶어 정리했습니다.
* 멀티 스레드 기능을 추가하였습니다.
  * 오브젝트는 ThreadDispatcher 컴포넌트를 추가하여 개체가 업데이트 될 스레드를 제어할 수 있습니다.
  * 개체가 서로 다른 스레드를 이용할 경우 서로 종속성이 없어야 합니다.
* 렌더링 장면 그래프가 이제 번들로 초기화됩니다.
  * 장면에 변경 사항이 있을 경우 번들이 새로 생성됩니다.
  * 번들은 라이팅, 기하 렌더링으로 나누어져, 각각 다른 스레드에서 실행됩니다.
* 이제 더 이상 엔진 내부에서 레퍼런스 카운팅 방식을 사용하지 않습니다.
  * 레퍼런스 카운팅은 Atomic 연산을 진행하므로 일반적인 연산보다 많은 오버헤드를 가집니다.
  * 이제 엔진 내부에서 참조하는 대부분의 개체는 Raw 포인터 형식으로 사용됩니다.
  * 클라이언트에서 레퍼런스 카운팅을 사용하려면 RefPtr 클래스 혹은 var 매크로를 참조하십시오.
* 게임 오브젝트의 Update 및 FixedUpdate 함수의 성능을 크게 개선했습니다.
* 애니메이션 목록이 비어있는 경우 매 프레임 애니메이션 키프레임을 갱신하지 않습니다.
## 2020-03-16. ver 0.1.3.232
* Bug-fix
  * 게임 개체에서 Camera, Light 등의 특수 컴포넌트가 추가될 때 장면 그래프를 새로 작성하지 못하는 문제를 수정하였습니다.
  * VisibleViewStorage 개체에서 수용 가능한 뷰 한도를 넘었을 때 정상적인 기본 처리를 수행하도록 변경하였습니다.
## 2020-03-16. ver 0.1.2
* 라이브러리 빌드 시 발생하는 링크 경고를 모두 제거했습니다.
  * 라이브러리에서 종속성 링크를 사용하지 않습니다.
  * 이제 라이브러리 링크는 클라이언트 프로젝트에서 링크합니다.
* 다수 오브젝트 렌더링 기능을 크게 개선했습니다.
  * 이제 매 프레임 장면 그래프를 새로 구성하지 않습니다.
  * 장면 개체에 Add, Remove 등의 함수가 호출될 때 장면 그래프가 새로 작성됩니다.
  * 장면 내부에 있는 게임 오브젝트의 개체에서 부모가 변경될 때, 제거될 때 장면 그래프가 새로 작성됩니다.
## 2020-03-14. ver 0.1.1
* 텍스트 렌더링 기능을 크게 개선했습니다.
  * 텍스트가 흐리게 출력되는 문제를 수정했습니다.
    * 텍스트 버퍼 작성에서 각 버퍼의 시작 위치를 정수 픽셀 포인트 단위를 사용하도록 변경되었습니다.
    * 텍스트를 출력할 때 정확한 위치에서 텍스처 정수 단위 로딩을 사용하도록 수정하였습니다. 더 이상 샘플러를 사용하지 않습니다.
  * 텍스트 공간 Update 기능을 개선했습니다.
    * 이제 텍스트 공간을 매 프레임 새로 계산하지 않습니다. 변경 사항이 있을 경우만 계산합니다.
  * 텍스트 출력 시 매 프레임 글리프 목록을 새로 구성하는 문제를 해결하였습니다.
    * 텍스트 출력 요청이 있을 경우 목록에 없는 글리프만 그리기를 새로 요청합니다.
    * 글리프 렌더링은 기존의 코드를 그대로 사용하지만, 글리프 검색 시간을 크게 제거할 수 있었습니다.
## 2020-03-14. ver 0.1.0
- 패치 노트 작성을 시작했습니다.
- PhysX 물리 엔진 기능에 Capsule Collider를 추가하였습니다. 이제 gameObject->AddComponent<CapsuleCollider>() 코드를 사용해 CapsuleCollider를 Rigidbody를 사용하는 게임 오브젝트 및 Rigidbody를 사용하지 않는 Static Rigidbody 게임 오브젝트에 사용할 수 있습니다.
- 모든 하위 물리 오브젝트가 부모 트랜스폼의 영향을 받도록 수정했습니다.
- Rigidbody의 성능을 대폭 개선했습니다. 이제 더 이상 매 프레임 Rigidbody 목록 및 Collider 목록을 재구성 하지 않습니다.
