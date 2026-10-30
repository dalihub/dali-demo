# dali-core / dali-adaptor public API 확정 전 검토

최초 작성일: 2026-10-30. 소스와 사용처를 읽어 정리한 **설계 검토 후보**다. 2026-10-31부터 승인된 ID의 패치와 진행 상태를 함께 기록한다. 빌드, 실행 테스트, 바이너리 ABI 비교는 수행하지 않았다.

## 검토 범위와 판단 기준

- 기준 커밋: core `a71ba077024791cc81583e29b0c4acec2b59b0a7`, adaptor `0cd5500eb886b9640eb73484cfa9b34c69c5136d`.
- 후보 검색 범위: core public 헤더 122개, adaptor public 헤더 37개. 검색 결과 중 문제가 의심되는 선언의 본문, 대응 구현, 워크스페이스 내 사용처를 확인했다. 모든 선언에 대한 완전한 API 명세 검증을 뜻하지 않는다.
- 기준 문서: [public-api-abi.md](dali-ui/rules/public-api-abi.md), [api-naming.md](dali-ui/rules/api-naming.md), [handle-body-pattern.md](dali-ui/rules/handle-body-pattern.md), [bitfield-enums.md](dali-ui/rules/bitfield-enums.md).
- 위 규칙은 원래 UI를 대상으로 한다. 여기서는 core/adaptor 재설계의 판단 기준으로 적용하며, 기존 core/adaptor 선언을 일괄 규칙 위반으로 판정하지 않는다.
- UI/toolkit의 public API 자체는 검토 대상에서 제외했다. 변경의 파급 범위를 파악할 때만 해당 저장소의 소비 코드를 참조했다.
- 다른 PR에서 진행 중인 **IntPair alias 전환, viewport 제거, PixelBuffer의 core 이동, math constexpr 확장**은 제안에서 제외했다.
- 워크스페이스에서 사용처가 적거나 발견되지 않았다는 사실만으로 외부 앱에서도 미사용이라고 단정하지 않았다.

우선순위의 의미:

| 우선순위 | 의미 |
| --- | --- |
| P0 | 확정 전에 공개 계약/구조를 먼저 결정할 것을 권장. 이후 ABI 유지 상태에서 바꾸기 특히 어렵다. |
| P1 | 삭제·비공개화·이름 변경의 근거가 비교적 명확하다. 확정 전에 정리할 것을 권장. |
| P2 | 사용성 또는 장기 설계 개선 후보. 기능의 실제 소비자와 지원 범위를 먼저 합의해야 한다. |

`public-api` 경로, C++ `public:` 접근 권한, export 여부, Doxygen 문서 노출은 서로 다르다. `DALI_INTERNAL`이나 `@cond internal`은 C++ 접근을 private로 만들지 않는다. 반대로 private 멤버와 템플릿 본문도 앱에 컴파일되므로 ABI와 무관한 것은 아니다.

## 우선 검토 목록

검토 단계는 `검토 대기 → 방향 결정 → 패치 작성 → 검증 대기 → 완료`로 관리한다.
변경하지 않기로 결정한 항목은 `유지 결정`으로 표시한다. 앞으로 작업 지시는 ID별 처리 방향을 기준으로 반영한다.
2026-10-31: 08은 기존 API/동작을 유지하고 주석만 보완, 10·14는 유지, 11·18·19는 패치를 작성하고 사용자 빌드·테스트를 기다린다.

| 검토 단계 | ID | 우선순위 | 대상 | 권장 방향 |
| --- | --- | --- | --- | --- |
| 검토 대기 | 01 | P1 | RenderTask 기본값 6개, Constraint 기본값 | public 상수 제거, 구현 상수와 기본값 문서 유지 |
| 검토 대기 | 02 | P0 | Signal 연결 풀·노드·순회 상태 | 앱 계약에서 제외하고 ABI 변경 가능한 구현 경계 마련 |
| 검토 대기 | 03 | P0 | CallbackBase 저장 구조와 dispatcher, 인자 수 제한 | typed callback을 중심으로 구현 노출과 제한 정리 |
| 검토 대기 | 04 | P1 | Any의 container/clone/delete/assert 보조 API | private/detail로 축소 |
| 검토 대기 | 05 | P1 | SharedPtr/ManagedOpenHashMap/Vector 구현용 이름 | public 구현 타입·정책 상수 축소 |
| 검토 대기 | 06 | P1 | WindowData 및 core의 Impl/raw-storage 접근자 | 내부 접근 경로로 이동 |
| 검토 대기 | 07 | P0 | Property::Map의 const 조회 | const/non-const 반환 계약 분리 |
| 유지 결정 · 주석 반영 | 08 | P0 | Property 값·Map·Array의 hash와 equality | API/동작 유지, 캐시용 fingerprint 주석 추가 |
| 검토 대기 | 09 | P0 | NativeImageInterface의 backend virtual API | 앱 소비 API와 provider 구현 계약 분리 검토 |
| 유지 결정 | 10 | P1 | PropertyRanges의 인덱스 할당 체계 | API와 숫자 값 유지 |
| 검증 대기 | 11 | P2 | Property::Key, ConstraintSource의 변경 가능한 tag | private 멤버 + 읽기 전용 getter 패치 작성 |
| 검토 대기 | 12 | P1 | UiContext/Window의 deprecated raw callback | 이미 있는 typed 대체 API로 정리 |
| 검토 대기 | 13 | P2 | RenderTask 화면 좌표 변환 함수 포인터 상수 | 기본값 상수와 기능 선택 API를 분리 |
| 유지 결정 | 14 | P2 | RenderTask::GetRenderTaskId | API와 동작 유지 |
| 검토 대기 | 15 | P1 | Window의 플랫폼 한정 조작과 native ID | 지원 결과와 ID 의미를 명확히 하는 계약 |
| 검토 대기 | 16 | P2 | VideoSourceDescriptor::GetVersion | plugin protocol과 앱 descriptor 구분 |
| 검토 대기 | 17 | P1 | MouseInOutEvent::Type::IN/OUT | ENTER/LEAVE 등으로 변경, 헤더의 매크로 해제 제거 |
| 검증 대기 | 18 | P1 | bool 옵션 API 이름 | Set/Is의 동일한 명사구로 통일 |
| 검증 대기 | 19 | P1 | 읽기 전용 입력의 non-const 참조 | const 참조 또는 값 전달로 수정 |
| 검토 대기 | 20 | P2 | Shader의 backend prefix/version API | 앱의 shader 언어 계약과 backend 전처리 구분 |
| 검토 대기 | 21 | P2 | flags enum의 표현과 AUTO 정책 혼합 | unsigned flags와 자동 선택 정책 구분 검토 |

## 01. RenderTask::DEFAULT_*: 단순 기본값은 public에서 제거 권장

근거: [render-task.h](dali-core/dali/public-api/render-tasks/render-task.h):212, [render-task.cpp](dali-core/dali/public-api/render-tasks/render-task.cpp):45.

대상은 `DEFAULT_EXCLUSIVE`, `DEFAULT_INPUT_ENABLED`, `DEFAULT_CLEAR_COLOR`, `DEFAULT_CLEAR_ENABLED`, `DEFAULT_CULL_MODE`, `DEFAULT_REFRESH_RATE` 6개다. 각각 생성 시 적용되는 설정을 나타내며 앱은 이미 setter/getter로 기능을 사용할 수 있다. 공개 데이터 심볼과 타입까지 영구 계약으로 유지할 필요는 작다.

**권장:** 값은 내부의 공통 기본값 정의로 옮기고, 생성/설정 API에 기본 동작을 문서화한다. `REFRESH_ONCE`와 `REFRESH_ALWAYS`는 앱이 설정에 사용하는 의미 있는 값이므로 유지한다. 기본값을 문서화한 뒤에도 기본 동작 변경의 호환성 문제는 남는다. 상수 제거가 기본 동작을 자유롭게 변경할 수 있다는 뜻은 아니다.

**유사 패턴 확인 결과:** RenderTask만의 패턴은 아니다.

| 유사 항목 | 판단 |
| --- | --- |
| [Constraint::DEFAULT_REMOVE_ACTION](dali-core/dali/public-api/animation/constraint.h):311 | `BAKE`라는 초기값 정보. 동일한 제거 후보. 내부 `constraint-base.cpp`에서 사용 중. |
| [PanGestureDetector::DEFAULT_THRESHOLD](dali-core/dali/public-api/events/pan-gesture-detector.h):109 | `AddAngle`/`AddDirection`의 기본 인자이고 toolkit `ScrollView`의 선언에도 쓰인다. 단순 삭제보다 기본 인자 표현/기본값을 적용하는 overload까지 함께 검토해야 한다. |
| `ParentOrigin::DEFAULT`, `AnchorPoint::DEFAULT` | 좌표 프리셋으로 앱에서 직접 값을 선택하는 용도. 이름에 DEFAULT가 있다고 일괄 삭제할 대상은 아니다. |
| 화면→framebuffer 변환 함수 상수 | 기능 선택에도 사용 중. 13번에서 별도 검토. |

영향: core event/update 양쪽 RenderTask 초기화, 기본값 UTC, 상수에 의존하는 외부 코드. [render-task-impl.cpp](dali-core/dali/internal/event/render-tasks/render-task-impl.cpp):1322와 [scene-graph-render-task.cpp](dali-core/dali/internal/update/render-tasks/scene-graph-render-task.cpp):610이 같은 기본값을 사용하므로 내부화 후에도 두 정의가 갈라지지 않도록 한다.

## 02. Signal의 메모리 풀과 순회 구조는 앱 API로 고정하지 않는 편이 좋음

근거: [signal-connection-pool.h](dali-core/dali/public-api/signals/signal-connection-pool.h):49, :85, :122, [base-signal.h](dali-core/dali/public-api/signals/base-signal.h):125, :407.

`SignalConnectionNode`, `SignalConnectionBlock`, `SignalConnectionPool`이 연결 리스트 인덱스, 블록 용량, high-water mark, raw slot 접근, index↔node 변환까지 공개한다. `BaseSignal::EmitGuard`의 `mFlag` 역시 public이다. 이들은 앱의 connect/disconnect/emit 기능보다 현재 메모리 풀 구현을 설명하는 API에 가깝다.

더 중요한 제약은 `BaseSignal`이 풀과 순회 상태를 직접 멤버로 가지고, `Emit*` 템플릿이 그 레이아웃을 앱 바이너리에 컴파일한다는 점이다. 추후 연결 수가 적은 경우의 최적화나 저장 방식 교체가 ABI 문제로 이어진다.

**권장:** 앱 계약은 `Signal<Signature>`, 연결/해제, 수명 추적, 호출 순서와 emit 중 변경 동작에 집중한다. 풀/노드/guard는 내부 또는 detail로 옮긴다. 장기 ABI 유연성이 필요하면 비템플릿 구현 경계 뒤로 연결 저장·순회를 옮기는 설계도 지금 결정한다.

**주의:** namespace 이동이나 private 전환만으로 이미 앱에 컴파일되는 레이아웃 의존성이 사라지지는 않는다. 템플릿에 필요한 정의는 소비자에게 계속 배포해야 할 수 있다. 가상 observer를 사용하는 확장 소비자와 호출 성능도 함께 검토해야 한다.

영향: `base-signal.*`, `dali-signal.h`, connection tracker, callback, Signal/ConnectionTracker UTC. 기존 외부 사용 가능성이 있으므로 단순 파일 삭제로 처리하지 않는다.

## 03. CallbackBase의 raw 저장 구조와 dispatcher 공개 축소

근거: [callback.h](dali-core/dali/public-api/signals/callback.h):45, :207, :248, :1511, :1596.

`CallbackBase::Impl`, `mImpl`, member/static function 포인터 union이 public이고 `Dispatcher0…3`, `FunctorDispatcher*`, `Destroyer` 등 보조 템플릿이 `Dali` namespace에 노출된다. 앱이 타입 소거 구조와 destructor dispatcher까지 직접 만질 수 있다. 헤더 자체에도 이 레이아웃이 앱에 컴파일된다는 설명이 있다.

새 `Callback<Signature>`도 member function/functor에 대해 최대 인자 수 3개 및 copy-constructible functor 제한을 둔다. 이는 앱 기능의 본질적인 제약보다는 현재 dispatcher 구현에서 비롯된 제약이다. free function은 다른 인자 수 규칙을 사용한다.

**권장:** typed callback을 정식 앱 진입점으로 삼고 저장 구조·dispatcher는 private/detail로 축소한다. 인자 수 제한과 move-only functor 지원 여부도 확정 전에 결정한다. `Release()`는 raw callback API에 소유권을 넘기기 위한 bridge이므로 12번 정리 뒤에도 공개할 필요가 있는지 재평가한다.

영향: Signal, MakeCallback, functor delegate, adaptor callbacks, 바인더. 타입 검증, 소유권, 원래 객체의 수명, 인자 전달 규약을 유지하는 재설계가 필요하다. 단순히 public을 private로 바꾸면 기존 dispatcher 구현부터 수정해야 한다.

## 04. Any의 저장용 타입과 AssertAlways는 비공개화 후보

근거: [any.h](dali-core/dali/public-api/object/any.h):72, :305.

`AnyContainerBase`, `AnyContainerImpl`, clone/delete helper, `CloneFunc`, `DeleteFunc`가 public이다. 앱에는 값 저장, 타입 확인, 값 추출이 필요하지만 현재 container 구현과 삭제 함수 포인터를 공개할 이유는 작다. `Any::AssertAlways()` 역시 일반 앱 기능보다는 내부 오류 처리 경로다.

**권장:** 이 이름들을 private nested type/helper 또는 detail로 옮긴다. 템플릿 구현은 헤더에 남아도 된다. type erasure가 앱에서 인스턴스화되는 특성 때문에 타입의 접근 권한 정리와 바이너리 호환성은 별도로 판단한다.

사용처: container helper는 조사한 저장소에서 주로 `Any` 자신이 사용한다. 다만 [dali-core-wrap.cpp](dali-csharp-binder/dali-csharp-binder/dali-core/dali-core-wrap.cpp):1668은 `Any::AssertAlways()`를 호출한다. 삭제 시 기존 C# extern 이름을 지우는 방식으로 처리하지 말고 내부 compatibility 경로를 마련해야 한다.

## 05. 공통 컨테이너의 구현 타입·성장 정책 이름 축소

근거:

- [shared-ptr.h](dali-core/dali/public-api/common/shared-ptr.h):82의 `RefCountControlBlock`: strong/weak count와 초기값을 공개한다.
- [open-hash-map-managed.h](dali-core/dali/public-api/common/open-hash-map-managed.h):40, :140의 `EntryState`, `ManagedOpenHashMap::Entry`: tombstone 등 저장 방식을 공개한다.
- [dali-vector-base.h](dali-core/dali/public-api/common/dali-vector-base.h):79의 `SHRINK_REQUIRED_RATIO`: 자동 축소 비율이라는 구현 정책이다.

**권장:** control block, slot state, entry는 private/detail로, 자동 축소 상수는 구현 전용으로 축소한다. 앱용 `InsertResult`, iterator의 key/value 조회, `Capacity()` 같은 컨테이너 기능은 유지한다. 공유 포인터나 컨테이너 자체의 삭제를 제안하는 것은 아니다.

특히 ManagedOpenHashMap은 public 설명에 75% load factor, tombstone, `new Entry[]`, `Erase/Clear` 후 리소스 지연 해제까지 명시한다. 필요한 참조 무효화/수명 규칙은 문서화하되 교체 가능한 구현 알고리즘까지 앱 보장으로 약속할지는 결정할 필요가 있다.

영향: `Vector::Erase` 계열의 축소 판단과 core `transform-manager.cpp`도 `SHRINK_REQUIRED_RATIO`를 참조한다. 템플릿 레이아웃의 ABI 의존성은 02번과 같은 주의가 필요하다.

## 06. Impl/raw-storage 접근자는 사용자용 public 메서드에서 분리

| 대상 | 현재 노출과 권장 방향 |
| --- | --- |
| [WindowData](dali-adaptor/dali/public-api/adaptor-framework/window-data.h):179 | `Impl`, mutable/const `GetImplementation()`이 public이고 메서드는 DALI_INTERNAL 표시도 없다. 실제 창 생성 코드의 접근은 friend/internal accessor로 옮기는 것을 우선 권장. |
| [CustomActorImpl](dali-core/dali/public-api/actors/custom-actor-impl.h):391 | `Initialize(Internal::CustomActor&)`, `GetOwner()`는 core owner를 직접 노출. custom control 개발 기능은 유지하고, core 소유권 초기화/내부 owner 접근만 분리. 공개 `Self()`와의 역할도 정리. |
| [String](dali-core/dali/public-api/common/dali-string.h):286 | inline `GetRawStorage()`가 mutable `void*`를 반환. `Internal::StringImpl`의 변환 구현만 접근하도록 friend 경로 검토. |
| [Property::Value](dali-core/dali/public-api/object/property-value.h):555 | `Impl`, `Read()`, `Write()`는 DALI_INTERNAL이지만 C++ public. 구현·직렬화 helper의 접근 경로를 유지하며 비공개화 검토. |
| [Property::Map](dali-core/dali/public-api/object/property-map.h):384 / [Array](dali-core/dali/public-api/object/property-array.h):269 | `Impl`과 `Read()` 역시 같은 후보. |

근거 사용처: [window-impl.cpp](dali-adaptor/dali/internal/window-system/common/window-impl.cpp):235, [custom-actor-internal.cpp](dali-core/dali/internal/event/actors/custom-actor-internal.cpp):33, [dali-string-impl.cpp](dali-core/dali/integration-api/dali-string-impl.cpp):53, :94.

이 변경은 저장 구조 자체를 새로 설계해야만 가능한 작업은 아니다. 다만 class-level export, generated wrapper, integration helper가 이미 접근하는 범위를 확인해야 한다. `BaseHandle::GetBaseObject()` 같은 모든 저수준 접근자를 일괄 제거하는 것은 별도 확장/바인더 설계 없이 권장하지 않는다.

## 07. Property::Map의 const 조회가 mutable 값을 반환

근거: [property-map.h](dali-core/dali/public-api/object/property-map.h):156, :188, :198, :211, :222, :233.

`Value& GetValue(...) const`와 5개 `Value* Find(...) const` overload가 const Map을 통해 값을 수정할 수 있게 한다. `Find` 주석은 “const pointer”라고 설명하지만 반환 타입은 `const Value*`가 아니다. 같은 클래스의 const `operator[]`와 `Property::Array`는 const 값을 반환하므로 일관성도 부족하다.

**권장:** const overload는 `const Value&`/`const Value*`, mutable overload는 `Value&`/`Value*`로 나눈다. 앱이 읽기만 수행하는 경로와 수정하는 경로가 타입에 드러나도록 한다.

구현상의 후속 확인점: [property-map.cpp](dali-core/dali/public-api/object/property-map.cpp):230의 `Find()`는 mutable pointer를 반환하기 때문에 hash를 `ALWAYS_REHASH`로 바꾼다. 반면 :172의 `GetValue()`는 mutable reference를 반환하면서 같은 처리를 하지 않는다. const 계약 정리와 함께 hash 무효화 누락 가능성을 검증해야 한다. **이 문서에서는 실행으로 재현하지 않았다.**

영향: `const Property::Map&`를 받은 뒤 `Property::Value*`를 쓰는 다수 소비자의 타입 수정, 의도적으로 수정하던 코드의 입력 계약 변경. UI/toolkit API 재설계 대상은 아니지만 해당 소비 코드의 수정은 필요할 수 있다.

## 08. Property::Value/Map/Array의 GetHash와 동등성 계약

**2026-10-31 결정:** API, hash 알고리즘 및 equality 동작을 유지한다. Value/Map/Array의 GetHash 주석에 캐시 조회용 fingerprint이며 operator==에 부합하는 일반 hash가 아님을 명시했다. 아래 이동/재설계 제안은 채택하지 않았다.

근거: [property-value.h](dali-core/dali/public-api/object/property-value.h):540, [property-map.h](dali-core/dali/public-api/object/property-map.h):369, [property-array.h](dali-core/dali/public-api/object/property-array.h):250.

헤더가 이미 “두 값의 equality가 true여도 hash는 다를 수 있음”을 명시한다. 이는 내부 캐시의 정확한 표현 비교용 hash로는 선택할 수 있는 동작이지만, 앱이 `operator==`와 조합하여 일반 hash container의 hash로 사용하기에는 맞지 않는다.

**권장:** 캐시용 fingerprint인지, 동등성 기반 hash인지 API 이름과 계약에서 구분한다. 내부 캐시 용도만 필요하면 public 제거/이동을 검토한다. 일반 hash로 제공하려면 대응하는 equality도 함께 정의해야 한다. 근사 float 비교는 단순 반올림 hash 추가만으로 해결된다고 가정하면 안 된다.

영향: property 기반 shader/visual/cache key, property equality/hash UTC. 기존 cache key 의미를 확인하지 않고 hash 알고리즘만 교체하는 것은 권장하지 않는다. math constexpr PR과 별개인 공개 계약 문제다.

## 09. NativeImageInterface: 앱 기능과 graphics provider 계약 혼재

근거: [native-image-interface.h](dali-core/dali/public-api/images/native-image-interface.h):48, [native-image.h](dali-adaptor/dali/public-api/adaptor-framework/native-image.h):66, [texture.h](dali-core/dali/public-api/rendering/texture.h):109.

public virtual interface에 `CreateResource`, `DestroyResource`, `TargetTexture`, `PrepareTexture`, `GetTextureTarget`, `ApplyNativeFragmentShader`, `GetCustomSamplerTypename`, `PostRender` 등이 있다. GL texture target, shader 텍스트 보정, rendering lifecycle까지 한 계약으로 고정한다. 외부 구현자가 override하는 vtable은 이후 backend 추가/변경 때 제약이 크다.

**권장:** 앱이 native image를 texture로 사용하는 계약과, 플랫폼/graphics provider가 native resource를 구현하는 계약을 분리할지 지금 결정한다. 전자는 public에 유지하고 후자는 integration 또는 ABI-stable extension 계층 중 실제 안정성 요구에 맞게 둔다. 기존 `GetExtension()`도 어떤 확장 계약을 제공할지 정해야 한다.

단순 header 이동만으로 해결되지는 않는다. `Texture::New(NativeImageInterface&)`와 `NativeImage`의 상속 관계를 함께 검토해야 한다. 앱/외부 라이브러리가 custom native-image provider를 구현하는 사용례는 정당하므로 기능 자체를 제거하자는 제안은 아니다.

영향: adaptor native image 구현, graphics backends, 외부 provider, Texture 생성 및 native-image UTC. Ubuntu 외 backend에 대한 실제 검증은 별도로 필요하다.

## 10. PropertyRanges는 앱 속성과 구현 인덱스 배분을 함께 노출

**2026-10-31 결정:** 인덱스 범위 API와 숫자 값을 현상태로 유지한다. 아래 재분류 제안은 채택하지 않았다.

근거: [property-index-ranges.h](dali-core/dali/public-api/object/property-index-ranges.h):39.

`DEFAULT_PROPERTY_MAX_COUNT_PER_DERIVATION`, `ANIMATABLE_PROPERTY_WITHOUT_UNIFORM_REGISTRATION_START_INDEX`, `CORE_PROPERTY_MAX_INDEX` 등은 상속 깊이, event/update 저장 방식, uniform mapping 구분을 숫자 구간으로 공개한다. 향후 속성 분류/할당 전략 변경 시 외부 코드의 범위 비교와 충돌할 수 있다.

**권장:** 앱에 필요한 `Actor::Property::*`, `Property::Index`, `INVALID_INDEX`, 등록 API는 유지한다. framework/component 등록에 필요한 범위 계약은 해당 개발자 계층으로 분리하고, 순수 구현용 숫자 구간은 내부화하는 방향을 검토한다.

이 항목은 01번과 달리 즉시 상수만 삭제할 수 없다. public property enum 초기화에 참조되고 외부 component 등록에서 쓰일 수 있다. 기존 property ID 숫자를 의도 없이 재배치하거나 저장된 property/JSON 형식을 바꾸지 않는다.

## 11. 변경 가능한 tag와 payload를 모두 공개하는 값 타입

**2026-10-31 패치:** Property::Key의 tag/index/string을 private으로 전환하고 GetType/GetIndexKey/GetStringKey를 제공했다. ConstraintSource의 type/index/object도 private으로 전환하고 GetSourceType/GetPropertyIndex/GetObject를 제공했다. 값 전체의 복사·대입은 유지하며 개별 payload를 통한 tag 불일치 상태 생성을 막는다. p-impl 추가는 하지 않았다. C#의 기존 Key extern entry는 유지하고 field setter를 Key 전체 대입으로 변환했다. index/string setter는 해당 key 종류도 함께 전환하며, type setter로 종류를 바꾸면 이전 종류의 payload는 보존하지 않는다. 이는 tag/payload 불일치 상태를 허용하지 않기 위한 호환 wrapper의 동작 변경이다. 관련 소비 코드와 기존 UTC를 getter 기반으로 수정했으며 빌드·실행 검증은 사용자 수행 대기 상태다.

**2026-11-01 결정 — Key 저장소:** 정렬 padding으로 인한 메모리 이득이 없어 union 전환을 원복했다. mStringKey/mIndexKey는 별도 private 멤버로 유지하고 복사·이동·소멸은 컴파일러가 생성하는 기본 동작을 사용한다. union 전용으로 추가한 UTC 4개도 제거했다. 기존 멤버 비공개화와 getter, 18·19번 패치는 유지한다. 빌드·테스트는 실행하지 않았다.

근거: [property-key.h](dali-core/dali/public-api/object/property-key.h):45, [constraint-source.h](dali-core/dali/public-api/animation/constraint-source.h):111.

`Property::Key`는 `type`, `indexKey`, `stringKey`가 모두 public이다. `ConstraintSource`도 `sourceType`, `propertyIndex`, `object`가 public이어서 tag와 payload가 어긋난 상태를 만들 수 있다. 이후 다른 key/source 종류를 추가하려면 공개 멤버·레이아웃 및 외부 직접 접근을 고려해야 한다.

**권장:** app-facing 값 타입은 유지하되 생성자/factory와 `GetType`, 값 조회를 통해 유효 상태를 유지하는 형태를 검토한다. 새 종류 추가가 예상되면 p-impl 또는 확장 가능한 저장 계약까지 판단한다. 작은 불변 값 타입이라는 의도가 확실하면 고정 레이아웃 자체는 허용할 수 있다.

수학 vector처럼 독립 성분을 수정하는 것과 동일한 이유로 일괄 캡슐화하지 않는다. 여기서는 tag와 payload의 상호 불변식이 검토 이유다.

## 12. typed 대체 API가 이미 있는 deprecated raw callback 제거

근거: [ui-context.h](dali-adaptor/dali/public-api/adaptor-framework/ui-context.h):142, :154, [window.h](dali-adaptor/dali/public-api/adaptor-framework/window.h):907, :938.

- `UiContext::AddIdle(CallbackBase*)` / `RemoveIdle(CallbackBase*)` → `Callback<bool()>`, `IdleCallbackId` overload.
- `Window::AddFrameRenderedCallback(CallbackBase*, ...)` / `AddFramePresentedCallback(CallbackBase*, ...)` → `Callback<void(int32_t)>` overload.

**권장:** ABI 변경이 허용되는 이번 공개 API 정리에서 deprecated raw overload를 제거할지 결정한다. 이미 같은 기능의 typed 대체가 있어 새로운 wrapper API를 추가할 필요가 없다. raw 경로는 호출 signature와 ownership을 문서에만 의존한다.

영향: adaptor/UI의 forwarding, samples, UTC, C# binder. 삭제 전에 포인터를 callback 취소 ID로 쓰던 소비자를 확인한다. 바인더의 기존 extern entry는 별도 호환 계층을 통해 유지한다. `CallbackBase` 전체의 즉시 제거와는 구분한다.

## 13. RenderTask의 두 함수 포인터 상수는 기본값 6개와 구분

근거: [render-task.h](dali-core/dali/public-api/render-tasks/render-task.h):143, :153, :166, :175, :403.

`DEFAULT_SCREEN_TO_FRAMEBUFFER_FUNCTION`은 항상 false, `FULLSCREEN_FRAMEBUFFER_FUNCTION`은 좌표를 변경하지 않고 true를 반환한다. `ConstScreenToFrameBufferFunction`은 **포인터 자체가 const인 함수 포인터 alias**이며, 변경 가능한 전역 함수 포인터로 오해해서는 안 된다.

실제 소비자: core 초기화, adaptor [capture-impl.cpp](dali-adaptor/dali/internal/system/common/capture-impl.cpp):292, toolkit/UI builder의 기본 변환 모드 설정. 따라서 단순히 기본값 주석으로 대체하면 기능 선택 호출부가 남는다.

**권장:** 기본값 데이터 심볼의 노출은 줄이되, “off-screen hit testing 비활성”, “full-screen 좌표 그대로 사용”, “custom 변환”을 명시적으로 표현할 방법을 함께 정한다. enum/명명된 동작 API 또는 typed callback이 후보이며 확정된 제안은 아니다. custom callback이 capture할 context를 지원할지도 검토한다.

`nullptr`를 기본값 대신 쓰는 방안은 [render-task-impl.cpp](dali-core/dali/internal/event/render-tasks/render-task-impl.cpp):810의 분기와 getter 반환 계약을 같이 검토해야 한다. 원래 콜백 포인터 identity를 사용하는 코드까지 같은 동작이라고 가정하지 않는다.

## 14. RenderTask::GetRenderTaskId는 진단 API로 분리 검토

**2026-10-31 결정:** GetRenderTaskId API와 동작을 현상태로 유지한다. 아래 이동 제안은 채택하지 않았다.

근거: [render-task.h](dali-core/dali/public-api/render-tasks/render-task.h):703, [render-task-impl.cpp](dali-core/dali/internal/event/render-tasks/render-task-impl.cpp):1344.

현재 ID는 scene object의 notify ID에서 가져온다. 앱은 이미 RenderTask handle로 객체를 식별할 수 있으며, 조사한 일반 구현 소비자는 adaptor automation의 JSON 출력이다. C# binder와 전용 UTC도 참조한다.

**권장:** 진단/자동화 식별자라면 integration/devel로 이동한다. 앱 기능으로 유지한다면 lifetime, 재사용 가능성, process 간 의미가 없는 점과 backend notify ID와의 독립성을 계약으로 명확히 한다. “식별자가 필요 없어 보인다”는 이유만으로 소비자 확인 없이 제거하지 않는다.

## 15. Window의 플랫폼 조작과 native ID 계약

근거: [window.h](dali-adaptor/dali/public-api/adaptor-framework/window.h):367, :545, :696.

`SetLayout(numCols, numRows, column, row, colSpan, rowSpan)`은 일반적인 window layout처럼 보이지만 실제로 [Windows](dali-adaptor/dali/internal/window-system/windows/window-base-win.cpp):870, [Ubuntu Ecore X11](dali-adaptor/dali/internal/window-system/ubuntu-x11/window-base-ecore-x.cpp):741 구현은 빈 함수이고, [Tizen Ecore](dali-adaptor/dali/internal/window-system/tizen/ecore/window-base-ecore-wl2.cpp):2632는 버전 조건 아래 구현된다. public 반환값은 void여서 앱은 미지원 여부를 알 수 없다. `EnableFloatingMode`는 public 주석부터 특정 display server 지원을 명시한다.

**권장:** 공통 기능으로 유지한다면 `WindowOperationResult` 등으로 미지원을 표현하거나 capability query를 제공한다. 플랫폼 전용 기능이라면 플랫폼 integration 계약으로 옮기는 것을 검토한다. 서버에 보낸 비동기 요청의 “요청 접수”와 “최종 적용”도 구분해야 한다.

`GetNativeId(): int32_t`도 별도 확인이 필요하다. [Windows](dali-adaptor/dali/internal/window-system/windows/window-base-win.cpp):805는 저장된 ID, [Android](dali-adaptor/dali/internal/window-system/android/window-base-android.cpp):130은 0, X11은 native window 값을 반환한다. 이를 모든 플랫폼에서 native handle이나 유일 ID라고 해석할 수 없다. 실제 native 객체 접근은 기존 `GetNativeHandle()`과 구분하고, ID를 유지할 목적과 유효 범위를 문서화하거나 진단 계층으로 이동한다. 단순 정수 폭 확대만으로 의미의 차이는 해결되지 않는다.

## 16. VideoSourceDescriptor::GetVersion의 앱 공개 필요성

근거: [video-source-descriptor.h](dali-adaptor/dali/public-api/adaptor-framework/video-source-descriptor.h):188, [구현](dali-adaptor/dali/public-api/adaptor-framework/video-source-descriptor.cpp):29.

현재 버전은 private Impl에서 `1u`로 시작하며 앱이 version을 설정하는 API는 없다. 확인된 소비자는 [video-player-plugin-factory.cpp](dali-extension/dali-extension/video-player/base/video-player-plugin-factory.cpp):54의 plugin 호환성 검사다. descriptor는 이미 p-impl이므로 앱이 구조체 ABI 버전을 직접 관리해야 하는 형태도 아니다.

**권장:** plugin protocol/capability를 의미한다면 provider 계층의 검증으로 이동한다. 앱이 실제로 version별 입력 규칙을 알아야 한다면 그 목적과 호환성 규칙부터 명시한다. plugin factory가 현재 사용 중이므로 그냥 삭제하는 것은 불가하다. `VideoSourceOwnership`처럼 앱이 native session 수명을 지정하는 정보는 유지한다.

## 17. MouseInOutEvent::IN/OUT 때문에 Windows 매크로를 해제함

근거: [mouse-in-out-event.h](dali-adaptor/dali/public-api/adaptor-framework/mouse-in-out-event.h):34, :58.

공개 enum의 `IN`, `OUT`을 사용하기 위해 `_WIN32`에서 같은 이름의 매크로를 `#undef`한다. 공개 DALi 헤더를 include하는 것만으로 뒤에 포함되는 Windows/앱 헤더의 전처리 환경이 달라진다. enum class로 scope가 있어도 매크로 치환 문제는 피할 수 없다.

**권장:** `ENTER`/`LEAVE` 또는 `ENTERED`/`LEFT`처럼 충돌하지 않는 enum 이름으로 변경하고 해당 `#undef`를 제거한다. 직렬화된 숫자 값은 필요하면 그대로 유지한다.

영향: Windows와 타 플랫폼 mouse-in/out 생성 코드, Window signal 소비자, UTC, 바인더. 나중에 deprecated alias `IN`/`OUT`을 남기면 매크로 충돌이 계속되므로 API 확정 전에 이름을 정하는 편이 좋다.

## 18. boolean 옵션 이름 정리

**2026-10-31 패치:** 아래 표의 대안으로 C++ 선언·구현·호출부를 변경했다. Actor는 IsLeaveRequired로 getter만 변경했고 CameraActor::IsYAxisInverted에는 const를 적용했다. Window/WindowData의 bool transparency setter는 SetTransparent로 통일했다. C# extern 함수 이름, property/JSON key는 유지했다. 빌드·실행 검증은 사용자 수행 대기 상태다.

[api-naming.md](dali-ui/rules/api-naming.md)의 `Set*Enabled` / `Is*Enabled`, 또는 자연스러운 `Set*` / `Is*` 짝을 적용할 후보다. 아래 이름은 최종 확정안이 아니라 구체적인 대안이다.

| 현재 | 대안 | 근거 |
| --- | --- | --- |
| `RenderTask::GetInputEnabled()` | `IsInputEnabled()` | [render-task.h](dali-core/dali/public-api/render-tasks/render-task.h):333 |
| `RenderTask::GetClearEnabled()` | `IsClearEnabled()` | 같은 파일 :583 |
| `RenderTask::SetCullMode(bool)` / `GetCullMode()` | `SetCullingEnabled(bool)` / `IsCullingEnabled()` | 같은 파일 :594. bool인데 Mode라는 이름은 enum 상태처럼 읽힘. |
| `CameraActor::SetInvertYAxis(bool)` / `GetInvertYAxis()` | `SetYAxisInverted(bool)` / `IsYAxisInverted() const` | [camera-actor.h](dali-core/dali/public-api/actors/camera-actor.h):350 |
| `Actor::SetLeaveRequired(bool)` / `GetLeaveRequired()` | 최소 `IsLeaveRequired()`로 getter 정리. 이벤트 의미까지 이름에 드러낼지도 검토 | [actor.h](dali-core/dali/public-api/actors/actor.h):1973 |
| `Window::EnableFloatingMode(bool)` | `SetFloatingModeEnabled(bool)` | [window.h](dali-adaptor/dali/public-api/adaptor-framework/window.h):545 |
| `Window::EnableScreenReaderAutoRead(bool)` | `SetScreenReaderAutoReadEnabled(bool)` | 같은 파일 :893 |
| `WindowData::SetTransparency(bool)` / `GetTransparency()` | `SetTransparent(bool)` / `IsTransparent()` | [window-data.h](dali-adaptor/dali/public-api/adaptor-framework/window-data.h):80. Window의 동일 setter도 함께 검토. |

`GetPosition(..., out) -> bool`, `GetBounds(...) -> bool`처럼 성공 여부를 반환하는 조회 함수는 이 규칙의 boolean 옵션 getter가 아니다. `Activate`, `Show`, `Maximize` 같은 window 명령도 일괄 이름 변경하지 않는다. property/JSON key는 C++ 이름 정리와 별개로 유지한다.

## 19. 읽기 전용 입력을 non-const 참조로 받는 선언

**2026-10-31 패치:** WindowData::SetPositionSize는 const BoundsInteger&, VertexBuffer::New와 내부 format 파싱은 const Property::Map&, FrameBuffer::AttachColorTexture는 Texture 값 전달, Source 생성자는 const Handle&로 변경했다. 빌드·실행 검증은 사용자 수행 대기 상태다.

| 선언 | 문제와 권장 방향 |
| --- | --- |
| [WindowData::SetPositionSize(BoundsInteger&)](dali-adaptor/dali/public-api/adaptor-framework/window-data.h):64 | 구현은 값 복사뿐이다. `const BoundsInteger&` 또는 작은 값 전달로 변경하면 const 객체와 임시 값을 전달할 수 있다. |
| [VertexBuffer::New(Property::Map&)](dali-core/dali/public-api/rendering/vertex-buffer.h):82 | format 입력 용도다. 내부 format 파싱까지 확인하면서 `const Property::Map&`로 정리. 07번 const Map 변경과 연계. |
| [FrameBuffer::AttachColorTexture(Texture&)](dali-core/dali/public-api/rendering/frame-buffer.h):169, :184 | caller의 handle을 재할당하는 출력 인자가 아니다. 다른 DALi handle API처럼 `Texture` 값 전달 등을 검토. |
| [Source::Source(Handle&, Property::Index)](dali-core/dali/public-api/animation/constraint-source.h):97 | 구현에서 handle을 복사한다. 값 또는 const 참조로 받을 수 있다. |

앱 호출을 불필요하게 제한하는 선언을 미리 정리하는 작은 변경 후보다. 내부 객체 상태를 변경하는 handle API와 caller의 handle 변수 자체를 변경하는 out parameter는 구분한다.

## 20. Shader의 version/prefix API: 사용자 shader 기능을 유지하면서 경계 검토

근거: [shader.h](dali-core/dali/public-api/rendering/shader.h):208, :217, :226, :235, [shader-impl.cpp](dali-core/dali/internal/event/rendering/shader-impl.cpp):459.

`GetShaderLanguageVersion`, `GetShaderVersionPrefix`, `GetVertexShaderPrefix`, `GetFragmentShaderPrefix`는 현재 graphics 환경의 문자열 생성 규칙을 앱에 노출한다. public 문서도 GL 초기화까지 block할 수 있다고 설명하고, 구현은 thread-local graphics 정보에서 prefix를 생성한다.

**검토 방향:** 앱이 작성할 shader의 언어/버전 계약과 backend가 삽입하는 전처리 문자열을 분리할 필요가 있는지 판단한다. 앞으로 여러 backend/context 또는 다른 shader 입력 형식을 지원할 계획이라면 정적 전역 조회 형태와 문자열 결합 방식이 제약이 된다.

custom shader를 작성하는 앱에는 실제로 필요한 기능이므로 현 시점의 단순 삭제는 권장하지 않는다. 자동 전처리 또는 source descriptor 같은 대체 설계가 준비된 경우에만 축소한다. `UniformBlock`, `Texture::Upload`, RenderTask readback도 고급 앱에 유용한 기능이므로 공개 규모가 크다는 이유로 함께 제거하지 않는다.

## 21. flags enum과 자동 선택 sentinel 정리

근거: [window-definitions.h](dali-adaptor/dali/public-api/adaptor-framework/window-definitions.h):182, [shader.h](dali-core/dali/public-api/rendering/shader.h):90, [frame-buffer.h](dali-core/dali/public-api/rendering/frame-buffer.h):64.

`WindowInsetsPartFlags`는 명시적 unsigned underlying type 없이 `1 << n` 값을 사용한다. Shader hints와 FrameBuffer attachments는 unscoped enum이다. 규칙의 방향대로 unsigned 고정 폭, unsigned flag literal, 타입에 맞는 연산자로 정리할 후보다. scoped enum의 기본 underlying type이 `int`라는 점과, 그 자체가 당장 ABI 오류라는 주장은 구분한다.

특히 `FrameBuffer::Attachment::AUTO = -1`은 depth/stencil/color 비트 조합과 “플랫폼이 선택”이라는 정책을 같은 enum에 섞는다. 단순히 unsigned로 바꾸는 대신 AUTO를 조합 가능한 flag로 취급할지, 별도 생성 정책으로 분리할지 먼저 결정한다.

기존 정상 mask 조합과 property/binder의 정수 표현을 함께 확인해야 한다. 모든 enum을 일괄 enum class로 바꾸는 작업은 범위 대비 파급이 크므로 권장하지 않는다.

## 이번 검토에서 삭제 대상으로 보지 않은 것

- `Shader(Internal::Shader*)`, `Texture(Internal::Texture*)`, `UniformBlock(Internal::UniformBlock*)` 등 `DALI_INTERNAL` 생성자: 여러 handle에 반복되는 의도적인 handle/body 연결 패턴이다. 규칙 문서도 이를 허용한다. 문서 노출 통일은 가능하지만 “이 클래스만 특이하다”는 이유로 삭제할 대상은 아니다.
- CustomActorImpl, PropertyInput, FrameCallbackInterface의 가상 함수 전체: 외부 component/provider의 실제 확장점이다. 필요하면 계층을 재분류하되 extension 안정성 계약까지 고려해야 한다.
- native image, frame readback, custom shader, UniformBlock: 일반 UI 앱의 빈도만으로 필요 없다고 판단하지 않는다.
- `Vector2::ZERO`, 색상/좌표 프리셋, 유효한 상태/flag enum: 앱이 코드에 사용하는 의미 있는 값이다. 기본 설정을 설명하는 상수와 구분한다.
- Dali::String/Vector/UniquePtr 등 DALi 소유 타입: `std::`가 주석·구현 본문·type trait·private storage 설명에 보인다는 것만으로 STL 경계 노출이라고 판정하지 않았다. `std::size_t` 역시 STL container 노출이 아니다. inline storage의 플랫폼별 ABI 조건은 별도 검토할 수 있지만 이번에는 직접 STL API 노출로 분류하지 않았다.
- 이미 진행 중인 네 PR의 대상: 이번 목록을 적용할 때 최신 PR 상태와 다시 대조한다.

## 제안하는 후속 작업 순서

1. **작은 공개면 정리:** 01, 04, 06 중 WindowData/raw storage, 12, 17. 18·19는 패치 작성 후 검증 대기다. 기능의 목적을 유지하면서 불필요한 노출이나 호출 제약을 줄이는 작업이다.
2. **확정 전에 설계 결정:** 02, 03, 07, 09. 08은 유지 및 주석 보완으로 결정했다. 템플릿 ABI, ownership, const/hash 계약, provider 경계를 먼저 정한다.
3. **소비자 합의 후 이동/축소:** 13, 15, 16, 20, 21. 10·14는 유지하기로 결정했고, 11은 패치 작성 후 검증 대기다. 플랫폼 기능과 확장 소비자가 있어 단순 삭제보다 대체 계약이 필요하다.

실제 구현 단계에서는 각 항목별로 public 선언/정의, 내부 forwarding, generated binding, 문서·샘플, UTC를 함께 확인한다. C# extern의 기존 이름/entry는 별도 호환 정책 없이 제거하지 않는다. Ubuntu 빌드 성공을 Windows/Tizen 등 타 플랫폼 성공으로 간주하지 않는다.

2026-10-31에 작성한 패치는 빌드·테스트·coverage 측정을 실행하지 않았다. 실제 검증은 사용자가 별도로 수행한다.

이 문서의 P0/P1은 실행으로 확인한 결함 등급이 아니라 **API 확정 전 의사결정의 우선순위**다. 테스트 실행 및 변경 라인 coverage 측정은 실제 구현 PR에서 수행한다.
