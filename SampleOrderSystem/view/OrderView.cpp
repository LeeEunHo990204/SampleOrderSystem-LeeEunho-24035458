// OrderView.cpp — Phase 3 + Phase 4
#include "OrderView.h"
#include <iostream>
#include <iomanip>
#include <sstream>

static const char* GREEN   = "\033[92m";
static const char* YELLOW  = "\033[93m";
static const char* RED     = "\033[91m";
static const char* MAGENTA = "\033[95m";
static const char* CYAN    = "\033[96m";
static const char* GRAY    = "\033[90m";
static const char* RESET   = "\033[0m";

// ---------------------------------------------------------------------------
// 상태 뱃지 색상 반환
// ---------------------------------------------------------------------------
static const char* statusColor(OrderStatus status) {
    switch (status) {
    case OrderStatus::CONFIRMED:  return GREEN;
    case OrderStatus::PRODUCING:  return YELLOW;
    case OrderStatus::REJECTED:   return RED;
    case OrderStatus::RESERVED:   return CYAN;
    case OrderStatus::RELEASE:    return MAGENTA;
    default:                      return RESET;
    }
}

// ---------------------------------------------------------------------------
// Phase 4: Render — [3] 주문 승인/거절 헤더만
// ---------------------------------------------------------------------------
void OrderView::Render() {
    std::cout << "\033[2J\033[H";
    std::cout << " [3] 주문 승인/거절\n";
    std::cout << GRAY << " ================================================\n" << RESET;
}

// ---------------------------------------------------------------------------
// Phase 3: renderCreateOrder — [2] 시료 주문 헤더
// ---------------------------------------------------------------------------
void OrderView::renderCreateOrder() {
    std::cout << "\033[2J\033[H";
    std::cout << " [2] 시료 주문\n";
    std::cout << GRAY << " ================================================\n" << RESET;
}

// ---------------------------------------------------------------------------
// Phase 4: showInlineMenu — 처리 후 계속/뒤로 메뉴
// ---------------------------------------------------------------------------
void OrderView::showInlineMenu() {
    std::cout << "\n " << CYAN << "[1]" << RESET << " 계속    "
              << CYAN << "[0]" << RESET << " 뒤로\n";
}

// ---------------------------------------------------------------------------
// Phase 3: showCreateOrderMenu — 주문 접수 후 인라인 메뉴
// ---------------------------------------------------------------------------
void OrderView::showCreateOrderMenu() {
    std::cout << "\n " << CYAN << "[1]" << RESET << " 주문 접수    "
              << CYAN << "[0]" << RESET << " 뒤로\n";
}

// ---------------------------------------------------------------------------
// Phase 3: showConfirm
// ---------------------------------------------------------------------------
void OrderView::showConfirm(const Sample& sample,
                            const std::string& customerName,
                            int qty)
{
    std::cout << "\n";
    std::cout << " 시료 ID   > " << sample.getId()  << "\n";
    std::cout << " 고객명    > " << customerName     << "\n";
    std::cout << " 주문 수량 > " << qty              << "\n";
    std::cout << "\n";
    std::cout << " 입력 내용 확인\n";
    std::cout << " 시료    " << sample.getName() << "  (" << sample.getId() << ")\n";
    std::cout << " 고객    " << customerName  << "\n";
    std::cout << " 수량    " << qty << " ea\n";
    std::cout << "\n";
    std::cout << " " << CYAN << "[Y]" << RESET << " 예약 접수    "
              << CYAN << "[N]" << RESET << " 취소\n";
    std::cout << "선택 > ";
}

// ---------------------------------------------------------------------------
// Phase 3: showOrderCreated
// ---------------------------------------------------------------------------
void OrderView::showOrderCreated(const Order& order,
                                 const std::string& sampleName)
{
    std::cout << "\n 예약 접수 완료.\n";
    std::cout << "\n";
    std::cout << " 주문번호    " << order.getId() << "\n";
    std::cout << " 현재 상태   "
              << CYAN << "[RESERVED]" << RESET << "\n";
    std::cout << "\n";
    std::cout << " ※ 재고 확인은 [3] 승인 메뉴에서 직접 진행하세요.\n";
    (void)sampleName;
}

// ---------------------------------------------------------------------------
// Phase 4: showReservedList
// ---------------------------------------------------------------------------
void OrderView::showReservedList(const std::vector<Order>& orders,
                                 const std::vector<Sample>& samples)
{
    std::cout << "\n 승인 대기 중인 예약 목록  (RESERVED)\n";
    std::cout << GRAY
              << " 번호    주문번호              고객               시료                  수량      상태\n"
              << RESET;

    for (int i = 0; i < static_cast<int>(orders.size()); ++i) {
        const Order& o = orders[i];

        std::string sampleName = o.getSampleId();
        for (const auto& s : samples) {
            if (s.getId() == o.getSampleId()) {
                sampleName = s.getName();
                break;
            }
        }

        std::cout << " [" << (i + 1) << "]"
                  << "     " << std::left << std::setw(20) << o.getId()
                  << "   " << std::setw(17) << o.getCustomerName()
                  << "   " << std::setw(20) << sampleName
                  << "   " << std::setw(6)  << o.getQuantity() << " ea"
                  << "   " << statusColor(o.getStatus())
                  << "[" << toString(o.getStatus()) << "]"
                  << RESET << "\n";
    }
    std::cout << " " << CYAN << "[0]" << RESET << " 뒤로\n\n";
}

// ---------------------------------------------------------------------------
// Phase 4: showStockChecking
// ---------------------------------------------------------------------------
void OrderView::showStockChecking() {
    std::cout << "\n 재고 확인 중...\n";
}

// ---------------------------------------------------------------------------
// Phase 4: showOrderInfo — 재고·부족분 표시 후 [V]/[N] 선택
// ---------------------------------------------------------------------------
void OrderView::showOrderInfo(const Sample& sample,
                              const Order& order,
                              int shortage,
                              int actualQty,
                              double totalTime)
{
    std::cout << "\n";
    std::cout << " 시료       " << std::left << std::setw(20) << sample.getName()
              << "   현재 재고  " << sample.getStock() << " ea\n";
    std::cout << " 주문 수량   " << order.getQuantity() << " ea";

    if (shortage <= 0) {
        std::cout << "               " << GREEN << "재고 충분\n" << RESET;
    } else {
        std::cout << "               부족분    " << shortage
                  << " ea  (실생산량 " << actualQty
                  << " ea / " << static_cast<int>(totalTime) << " min)\n";
    }

    std::cout << "\n";
    std::cout << " " << CYAN << "[Y]" << RESET << " 승인    "
              << CYAN << "[N]" << RESET << " 거절\n";
    std::cout << "선택 > ";
}

// ---------------------------------------------------------------------------
// Phase 4: showApproveResult
// ---------------------------------------------------------------------------
void OrderView::showApproveResult(const Order& order, OrderStatus newStatus) {
    std::cout << "\n 승인 완료.\n";
    std::cout << "\n";
    std::cout << " 상태 변경   RESERVED → "
              << statusColor(newStatus)
              << "[" << toString(newStatus) << "]"
              << RESET << "\n";
    std::cout << " 주문번호    " << order.getId() << "\n";
}

// ---------------------------------------------------------------------------
// Phase 4: showRejectResult
// ---------------------------------------------------------------------------
void OrderView::showRejectResult(const Order& order) {
    std::cout << "\n 거절 완료.\n";
    std::cout << "\n";
    std::cout << " 상태 변경   RESERVED → "
              << RED << "[REJECTED]" << RESET << "\n";
    std::cout << " 주문번호    " << order.getId() << "\n";
}

// ---------------------------------------------------------------------------
// 입력 메서드
// ---------------------------------------------------------------------------
std::string OrderView::inputSampleId() {
    return inputString(" 시료 ID: ");
}

std::string OrderView::inputCustomerName() {
    return inputString(" 고객명: ");
}

int OrderView::inputQuantity() {
    return inputInt(" 주문 수량 (ea): ");
}

char OrderView::inputConfirm() {
    std::string line;
    if (std::cin.peek() == '\n') std::cin.ignore();
    std::getline(std::cin, line);
    if (line.empty()) return 'N';
    return line[0];
}

int OrderView::inputListNo() {
    return inputInt(" 번호 (0=뒤로): ");
}

std::string OrderView::inputOrderId() {
    return inputString(" 주문번호: ");
}
