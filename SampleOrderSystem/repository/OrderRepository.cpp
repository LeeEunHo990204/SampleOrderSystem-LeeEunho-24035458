#include "OrderRepository.h"
#include "../util/JsonParser.h"
#include <fstream>
#include <sstream>

OrderRepository::OrderRepository(const std::string& filePath)
    : filePath_(filePath)
{
}

std::vector<Order> OrderRepository::loadAll() {
    std::ifstream file(filePath_);
    if (!file) return {};

    std::string text((std::istreambuf_iterator<char>(file)), {});
    if (text.empty()) return {};

    JsonValue root = JsonParser::parse(text);
    std::vector<Order> result;
    for (const auto& item : root.getArray())
        result.push_back(fromJson(item));
    return result;
}

void OrderRepository::saveAll(const std::vector<Order>& orders) {
    JsonValue::Array arr;
    for (const auto& o : orders)
        arr.push_back(toJson(o));

    std::ofstream file(filePath_);
    file << JsonParser::serialize(JsonValue(arr));
}

Order OrderRepository::fromJson(const JsonValue& obj) const {
    std::string id           = obj["id"].getString();
    std::string sampleId     = obj["sampleId"].getString();
    std::string customerName = obj["customerName"].getString();
    int         quantity     = static_cast<int>(obj["quantity"].getNumber());
    OrderStatus status       = fromString(obj["status"].getString());
    int         producedQty  = static_cast<int>(obj["producedQty"].getNumber());

    Order order(id, sampleId, customerName, quantity);
    order.setStatus(status);
    order.setProducedQty(producedQty);
    return order;
}

JsonValue OrderRepository::toJson(const Order& o) const {
    JsonValue::Object obj;
    obj["id"]           = JsonValue(o.getId());
    obj["sampleId"]     = JsonValue(o.getSampleId());
    obj["customerName"] = JsonValue(o.getCustomerName());
    obj["quantity"]     = JsonValue(o.getQuantity());
    obj["status"]       = JsonValue(toString(o.getStatus()));
    obj["producedQty"]  = JsonValue(o.getProducedQty());
    return JsonValue(obj);
}
