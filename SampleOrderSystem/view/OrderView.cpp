// OrderView.cpp — Phase 3: 시료 주문 서브메뉴 뷰
#include "OrderView.h"
#include <iostream>
#include <iomanip>

static const char* CYAN  = "\033[96m";
static const char* GRAY  = "\033[90m";
static const char* RESET = "\033[0m";

void OrderView::Render() {
    std::cout << "\033[2J\033[H";
    std::cout << " [2] 시료 주문\n";
    std::cout << GRAY << " ================================================\n" << RESET;
}

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

void OrderView::showInlineMenu() {
    std::cout << "\n " << CYAN << "[1]" << RESET << " 주문 접수    "
              << CYAN << "[0]" << RESET << " 뒤로\n";
}

std::string OrderView::inputOrderId() {
    return inputString(" 주문번호: ");
}
