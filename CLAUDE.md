# 반도체 시료 생산주문관리 시스템

> 전체 요구사항 및 기능 명세(시스템 개요, 처리 흐름도, 주문 상태 정의, 핵심 공식 포함)는 [docs/PRD.md](./docs/PRD.md)를 참고하세요.

---

## 개발 가이드

### 아키텍처
- **MVC 패턴** 적용: Model / Controller / View 패키지 구조와 역할 분리

### 데이터
- 데이터 영속성 처리: 파일, JSON, DB 등 자유롭게 선택
- 애플리케이션 재실행 후에도 데이터 유지 필요

### 개발 주안점
1. CLAUDE.md, PRD.md 등의 문서 관리
2. CleanCode 원칙 준수
3. 테스트 스크립트 작성
4. 체계적인 Commit 이력 관리

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
