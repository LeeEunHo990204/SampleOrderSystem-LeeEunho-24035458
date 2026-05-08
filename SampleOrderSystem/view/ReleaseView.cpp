#include "ReleaseView.h"
#include <iostream>

static const char* MAGENTA = "\033[95m";
static const char* CYAN    = "\033[96m";
static const char* GRAY    = "\033[90m";
static const char* RESET   = "\033[0m";

// UTF-8 문자열의 터미널 표시 너비 계산 (한글 = 2칸, ASCII = 1칸)
static int dispWidth(const std::string& s) {
    int w = 0;
    for (size_t i = 0; i < s.size(); ) {
        unsigned char c = static_cast<unsigned char>(s[i]);
        if      (c < 0x80) { w += 1; i += 1; }
        else if (c < 0xE0) { w += 1; i += 2; }
        else if (c < 0xF0) { w += 2; i += 3; }
        else               { w += 2; i += 4; }
    }
    return w;
}

static void padTo(const std::string& text, int colWidth) {
    std::cout << text;
    int pad = colWidth - dispWidth(text);
    if (pad > 0) std::cout << std::string(pad, ' ');
}

void ReleaseView::Render() {
    std::cout << "\033[2J\033[H";
    std::cout << " [6] 출고 처리\n";
    std::cout << GRAY << " ================================================\n" << RESET;
}

void ReleaseView::showConfirmedList(const std::vector<Order>& orders,
                                    const std::vector<Sample>& samples)
{
    std::cout << "\n 출고 가능 주문  (CONFIRMED)\n";
    std::cout << GRAY << " 번호    ";
    padTo("주문번호", 26);
    padTo("고객", 23);
    padTo("시료", 23);
    std::cout << "수량\n" << RESET;

    for (int i = 0; i < static_cast<int>(orders.size()); ++i) {
        const Order& o = orders[i];

        std::string sampleName = o.getSampleId();
        for (const auto& s : samples) {
            if (s.getId() == o.getSampleId()) {
                sampleName = s.getName();
                break;
            }
        }

        std::cout << " [" << (i + 1) << "]     ";
        padTo(o.getId(), 26);
        padTo(o.getCustomerName(), 23);
        padTo(sampleName, 23);
        std::cout << o.getQuantity() << " ea\n";
    }

    std::cout << "\n 출고할 번호 (0=뒤로) > ";
}

void ReleaseView::showReleaseResult(const Order& order) {
    std::cout << "\n 출고 처리 완료.\n\n";
    std::cout << " 주문번호    " << order.getId()      << "\n";
    std::cout << " 출고수량    " << order.getQuantity() << " ea\n";
    std::cout << " 처리일시    " << order.getReleasedAt() << "\n";
    std::cout << " 상태        CONFIRMED → "
              << MAGENTA << "[RELEASE]" << RESET << "\n";
}

void ReleaseView::showInlineMenu() {
    std::cout << "\n " << CYAN << "[1]" << RESET << " 계속  "
              << CYAN << "[0]" << RESET << " 뒤로";
}

int ReleaseView::inputListNo() {
    return inputInt("");
}
