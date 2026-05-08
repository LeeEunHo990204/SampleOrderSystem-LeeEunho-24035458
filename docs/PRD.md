# PRD - 반도체 시료 생산주문관리 시스템

## 1. 시스템 개요

- **콘솔 기반** 동작: 담당자가 직접 명령을 입력하여 시료를 등록하고 주문을 처리
- **생산 라인**: 공장에서 시료 하나를 생산하는 설비 흐름. 하나의 생산 라인은 시료를 하나씩 생산하며, 주문이 들어온 시료에 대해서만 생산

### 시스템 처리 흐름도

```
[주문 담당자]
      │
      ▼
  주문 등록
 (RESERVED)
      │
      ▼
  승인 여부?
  ┌───┴───┐
 거절     승인
  │        │
  ▼        ▼
REJECTED  재고 확인
          ┌────┴────┐
       재고 충분  재고 부족
          │        │
          ▼        ▼
       출고 준비  생산 요청       ← [생산 담당자]
      (CONFIRMED) (PRODUCING)
          │        │
          │     생산 완료
          │        │
          └────┬───┘
               ▼
           출고 처리
           (RELEASED)
               │
               ▼
           고객 도달              ← [고객]
```

---

## 2. 역할

| 역할 | 담당 |
|------|------|
| 고객 (시료 요청자) | 필요한 시료를 이메일로 요청 |
| 주문 담당자 | 요청에 맞게 주문서 작성 및 관리 |
| 생산 담당자 | 개발 시료 등록, 주문 수신 후 승인 또는 거절 |

---

## 3. 주문 상태 흐름

| 상태 | 의미 |
|------|------|
| `RESERVED` | 주문 접수 |
| `REJECTED` | 주문 거절 (모니터링 제외) |
| `PRODUCING` | 주문 승인 완료 및 재고 부족으로 생산 중 |
| `CONFIRMED` | 주문 승인 완료 및 출고 대기 중 |
| `RELEASE` | 출고 완료 |

> `REJECTED`는 정상 흐름 외의 상태이며 모니터링에서 제외됩니다.

---

## 4. 메인 메뉴 구성

기능별 선택 화면을 표시하며, 전체 시료에 대한 요약 정보를 확인할 수 있습니다.

| 메뉴 | 기능 |
|------|------|
| [시료 관리](features/01-sample-management.md) | 시료 등록, 목록 조회, 이름 검색 |
| [시료 주문](features/02-sample-order.md) | 고객 주문 접수 |
| [주문 승인/거절](features/03-order-approval.md) | 생산 라인 담당자의 승인·거절 처리 |
| [모니터링](features/04-monitoring.md) | 상태별 주문 수 및 시료별 재고 현황 |
| [생산라인 조회](features/05-production-line.md) | 현재 생산 중인 시료 및 대기 중인 생산 큐 확인 |
| [출고 처리](features/06-release-processing.md) | CONFIRMED 상태 주문에 대해 출고 실행 |

---

## 5. 기능 요구사항

각 기능의 상세 명세는 아래 링크를 참고하세요.

| 기능 | 상세 명세 |
|------|-----------|
| 시료 관리 | [features/01-sample-management.md](features/01-sample-management.md) |
| 시료 주문 | [features/02-sample-order.md](features/02-sample-order.md) |
| 주문 승인/거절 | [features/03-order-approval.md](features/03-order-approval.md) |
| 모니터링 | [features/04-monitoring.md](features/04-monitoring.md) |
| 생산 라인 | [features/05-production-line.md](features/05-production-line.md) |
| 출고 처리 | [features/06-release-processing.md](features/06-release-processing.md) |
