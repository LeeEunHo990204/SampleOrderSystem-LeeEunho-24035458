# 반도체 시료 생산주문관리 시스템

> 전체 요구사항 및 기능 명세(시스템 개요, 처리 흐름도, 주문 상태 정의, 핵심 공식 포함)는 [docs/PRD.md](./docs/PRD.md)를 참고하세요.

---

## 개발 가이드

### 아키텍처 및 참조 프로젝트

| 항목 | 설명 | 참조 프로젝트 |
|------|------|--------------|
| MVC 패턴 | Model / Controller / View 패키지 구조와 역할 분리 | ConsoleMVC |
| 데이터 영속성 | 파일, JSON, DB 등 자유롭게 선택. 애플리케이션 재실행 후에도 데이터 유지 필요 | DataPersistence |
| 데이터 모니터링 툴 | 데이터 현황 모니터링 | DataMonitor |
| Dummy 데이터 생성 툴 | 테스트용 더미 데이터 생성 | DummyDataGenerator |

### 개발 주안점
1. CLAUDE.md, PRD.md 등의 문서 관리
2. CleanCode 원칙 준수
3. 테스트 스크립트 작성
4. 체계적인 Commit 이력 관리

---

## Phase 구현 절차

각 Phase를 구현할 때는 아래 순서를 따릅니다.

### 1단계 — 문서 충돌 확인 (`document-consistency-verify` agent)

- md 파일 간 요구사항 충돌을 확인한다.
- 충돌이 있을 경우 User에게 보여주고 User가 선택하게 한다.
- 중복 내용은 한 군데에만 남도록 나머지를 삭제한다.

### 2단계 — 코드 작성 (`ai-action` agent)

- 해당 Phase의 코드를 작성한다.
- `.claude/skills/test-driven-development/SKILL.md` skill을 사용한다 (TDD: Red → Green → Refactor).

### 3단계 — 테스트 검증 (`test-verify` agent)

- 전체 unit test 결과가 정상적으로 통과하는지 확인한다.
- 실패한 테스트가 있을 경우 `ai-action` agent가 코드를 수정한다.

### 4단계 — 요구사항 준수 확인 (`compliance-verify` agent)

- PLAN.md 기준으로 현재 코드가 해당 Phase의 요구사항을 모두 만족하는지 확인한다.
- 누락된 요구사항이 있을 경우 `ai-action` agent가 해당 코드를 추가한다.

### 5단계 — Commit

- User가 변경 내용을 확인하고 이상 없을 시 Commit한다.

---

## 문서 구조

```
SampleOrderSystem/
├── CLAUDE.md                            # 프로젝트 개발 가이드 (이 파일)
└── docs/
    ├── PRD.md                           # 전체 요구사항 요약 및 기능 링크
    └── features/
        ├── 01-sample-management.md      # 시료 관리 상세 명세
        ├── 02-sample-order.md           # 시료 주문 상세 명세
        ├── 03-order-approval.md         # 주문 승인/거절 상세 명세
        ├── 04-monitoring.md             # 모니터링 상세 명세
        ├── 05-production-line.md        # 생산 라인 상세 명세
        └── 06-release-processing.md     # 출고 처리 상세 명세
```
