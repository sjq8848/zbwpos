#pragma once
#include <QString>
#include <QDateTime>
#include <QJsonObject>
#include <QJsonArray>

struct Product {
    int id = 0;
    QString cloudId;
    QString tenantId;
    QString storeId;
    QString barcode;
    QString sku;
    QString name;
    int categoryId = 0;
    QString unit = QStringLiteral("件");
    double price = 0.0;
    double costPrice = 0.0;
    double stockQty = 0.0;
    double minStock = 0.0;
    int status = 1;
    QString imageUrl;
    int syncStatus = 0;
    QDateTime createdAt;
    QDateTime updatedAt;

    QJsonObject toJson() const {
        return {
            {"cloud_id", cloudId},
            {"tenant_id", tenantId},
            {"store_id", storeId},
            {"barcode", barcode},
            {"sku", sku},
            {"name", name},
            {"category_id", categoryId},
            {"unit", unit},
            {"price", price},
            {"cost_price", costPrice},
            {"stock_qty", stockQty},
            {"min_stock", minStock},
            {"status", status},
            {"image_url", imageUrl},
            {"sync_status", syncStatus}
        };
    }

    static Product fromJson(const QJsonObject &json) {
        Product p;
        p.cloudId = json["cloud_id"].toString();
        p.tenantId = json["tenant_id"].toString();
        p.storeId = json["store_id"].toString();
        p.barcode = json["barcode"].toString();
        p.sku = json["sku"].toString();
        p.name = json["name"].toString();
        p.categoryId = json["category_id"].toInt();
        p.unit = json["unit"].toString(QStringLiteral("件"));
        p.price = json["price"].toDouble();
        p.costPrice = json["cost_price"].toDouble();
        p.stockQty = json["stock_qty"].toDouble();
        p.minStock = json["min_stock"].toDouble();
        p.status = json["status"].toInt(1);
        p.imageUrl = json["image_url"].toString();
        p.syncStatus = json["sync_status"].toInt();
        return p;
    }
};

struct Category {
    int id = 0;
    QString cloudId;
    QString tenantId;
    QString name;
    int parentId = 0;
    int sortOrder = 0;
    int syncStatus = 0;

    QJsonObject toJson() const {
        return {
            {"cloud_id", cloudId},
            {"tenant_id", tenantId},
            {"name", name},
            {"parent_id", parentId},
            {"sort_order", sortOrder},
            {"sync_status", syncStatus}
        };
    }

    static Category fromJson(const QJsonObject &json) {
        Category c;
        c.cloudId = json["cloud_id"].toString();
        c.tenantId = json["tenant_id"].toString();
        c.name = json["name"].toString();
        c.parentId = json["parent_id"].toInt();
        c.sortOrder = json["sort_order"].toInt();
        c.syncStatus = json["sync_status"].toInt();
        return c;
    }
};

struct Member {
    int id = 0;
    QString cloudId;
    QString tenantId;
    QString cardNo;
    QString name;
    QString phone;
    int gender = 0;
    QDate birthday;
    int levelId = 0;
    int points = 0;
    double balance = 0.0;
    double totalSpent = 0.0;
    int status = 1;
    int syncStatus = 0;
    QDateTime createdAt;
    QDateTime updatedAt;

    QJsonObject toJson() const {
        return {
            {"cloud_id", cloudId},
            {"tenant_id", tenantId},
            {"card_no", cardNo},
            {"name", name},
            {"phone", phone},
            {"gender", gender},
            {"birthday", birthday.toString(Qt::ISODate)},
            {"level_id", levelId},
            {"points", points},
            {"balance", balance},
            {"total_spent", totalSpent},
            {"status", status},
            {"sync_status", syncStatus}
        };
    }

    static Member fromJson(const QJsonObject &json) {
        Member m;
        m.cloudId = json["cloud_id"].toString();
        m.tenantId = json["tenant_id"].toString();
        m.cardNo = json["card_no"].toString();
        m.name = json["name"].toString();
        m.phone = json["phone"].toString();
        m.gender = json["gender"].toInt();
        m.birthday = QDate::fromString(json["birthday"].toString(), Qt::ISODate);
        m.levelId = json["level_id"].toInt();
        m.points = json["points"].toInt();
        m.balance = json["balance"].toDouble();
        m.totalSpent = json["total_spent"].toDouble();
        m.status = json["status"].toInt(1);
        m.syncStatus = json["sync_status"].toInt();
        return m;
    }
};

struct MemberLevel {
    int id = 0;
    QString cloudId;
    QString tenantId;
    QString name;
    double discount = 1.00;
    int minPoints = 0;

    QJsonObject toJson() const {
        return {
            {"cloud_id", cloudId},
            {"tenant_id", tenantId},
            {"name", name},
            {"discount", discount},
            {"min_points", minPoints}
        };
    }

    static MemberLevel fromJson(const QJsonObject &json) {
        MemberLevel ml;
        ml.cloudId = json["cloud_id"].toString();
        ml.tenantId = json["tenant_id"].toString();
        ml.name = json["name"].toString();
        ml.discount = json["discount"].toDouble(1.00);
        ml.minPoints = json["min_points"].toInt();
        return ml;
    }
};

struct OrderItem {
    int id = 0;
    QString tenantId;
    int orderId = 0;
    int productId = 0;
    QString barcode;
    QString productName;
    double unitPrice = 0.0;
    double quantity = 0.0;
    double discountRate = 1.00;
    double subtotal = 0.0;

    QJsonObject toJson() const {
        return {
            {"tenant_id", tenantId},
            {"order_id", orderId},
            {"product_id", productId},
            {"barcode", barcode},
            {"product_name", productName},
            {"unit_price", unitPrice},
            {"quantity", quantity},
            {"discount_rate", discountRate},
            {"subtotal", subtotal}
        };
    }

    static OrderItem fromJson(const QJsonObject &json) {
        OrderItem oi;
        oi.tenantId = json["tenant_id"].toString();
        oi.orderId = json["order_id"].toInt();
        oi.productId = json["product_id"].toInt();
        oi.barcode = json["barcode"].toString();
        oi.productName = json["product_name"].toString();
        oi.unitPrice = json["unit_price"].toDouble();
        oi.quantity = json["quantity"].toDouble();
        oi.discountRate = json["discount_rate"].toDouble(1.00);
        oi.subtotal = json["subtotal"].toDouble();
        return oi;
    }
};

struct Payment {
    int id = 0;
    QString tenantId;
    int orderId = 0;
    QString method;
    double amount = 0.0;
    QString referenceNo;
    int status = 1;
    QDateTime createdAt;

    QJsonObject toJson() const {
        return {
            {"tenant_id", tenantId},
            {"order_id", orderId},
            {"method", method},
            {"amount", amount},
            {"reference_no", referenceNo},
            {"status", status}
        };
    }

    static Payment fromJson(const QJsonObject &json) {
        Payment p;
        p.tenantId = json["tenant_id"].toString();
        p.orderId = json["order_id"].toInt();
        p.method = json["method"].toString();
        p.amount = json["amount"].toDouble();
        p.referenceNo = json["reference_no"].toString();
        p.status = json["status"].toInt(1);
        return p;
    }
};

struct Order {
    int id = 0;
    QString cloudId;
    QString tenantId;
    QString storeId;
    QString orderNo;
    int memberId = 0;
    double totalAmount = 0.0;
    double discountAmount = 0.0;
    double finalAmount = 0.0;
    QString paymentMethod;
    int status = 1;
    int cashierId = 0;
    int shiftId = 0;
    QString remark;
    int syncStatus = 0;
    QDateTime createdAt;
    QList<OrderItem> items;
    QList<Payment> payments;

    QJsonObject toJson() const {
        QJsonArray itemsArr;
        for (const auto &item : items)
            itemsArr.append(item.toJson());
        QJsonArray paymentsArr;
        for (const auto &pay : payments)
            paymentsArr.append(pay.toJson());

        return {
            {"cloud_id", cloudId},
            {"tenant_id", tenantId},
            {"store_id", storeId},
            {"order_no", orderNo},
            {"member_id", memberId},
            {"total_amount", totalAmount},
            {"discount_amount", discountAmount},
            {"final_amount", finalAmount},
            {"payment_method", paymentMethod},
            {"status", status},
            {"cashier_id", cashierId},
            {"shift_id", shiftId},
            {"remark", remark},
            {"sync_status", syncStatus},
            {"items", itemsArr},
            {"payments", paymentsArr}
        };
    }

    static Order fromJson(const QJsonObject &json) {
        Order o;
        o.cloudId = json["cloud_id"].toString();
        o.tenantId = json["tenant_id"].toString();
        o.storeId = json["store_id"].toString();
        o.orderNo = json["order_no"].toString();
        o.memberId = json["member_id"].toInt();
        o.totalAmount = json["total_amount"].toDouble();
        o.discountAmount = json["discount_amount"].toDouble();
        o.finalAmount = json["final_amount"].toDouble();
        o.paymentMethod = json["payment_method"].toString();
        o.status = json["status"].toInt(1);
        o.cashierId = json["cashier_id"].toInt();
        o.shiftId = json["shift_id"].toInt();
        o.remark = json["remark"].toString();
        o.syncStatus = json["sync_status"].toInt();

        const QJsonArray itemsArr = json["items"].toArray();
        for (const QJsonValue &v : itemsArr)
            o.items.append(OrderItem::fromJson(v.toObject()));

        const QJsonArray paymentsArr = json["payments"].toArray();
        for (const QJsonValue &v : paymentsArr)
            o.payments.append(Payment::fromJson(v.toObject()));

        return o;
    }
};

struct Cashier {
    int id = 0;
    QString cloudId;
    QString tenantId;
    QString username;
    QString passwordHash;
    QString name;
    QString role = QStringLiteral("cashier");
    int status = 1;
    int syncStatus = 0;

    QJsonObject toJson() const {
        return {
            {"cloud_id", cloudId},
            {"tenant_id", tenantId},
            {"username", username},
            {"name", name},
            {"role", role},
            {"status", status},
            {"sync_status", syncStatus}
        };
    }

    static Cashier fromJson(const QJsonObject &json) {
        Cashier c;
        c.cloudId = json["cloud_id"].toString();
        c.tenantId = json["tenant_id"].toString();
        c.username = json["username"].toString();
        c.passwordHash = json["password_hash"].toString();
        c.name = json["name"].toString();
        c.role = json["role"].toString(QStringLiteral("cashier"));
        c.status = json["status"].toInt(1);
        c.syncStatus = json["sync_status"].toInt();
        return c;
    }
};

struct Shift {
    int id = 0;
    QString cloudId;
    QString tenantId;
    QString storeId;
    int cashierId = 0;
    QDateTime startTime;
    QDateTime endTime;
    double startCash = 0.0;
    double endCash = 0.0;
    int status = 1;
    int syncStatus = 0;

    QJsonObject toJson() const {
        return {
            {"cloud_id", cloudId},
            {"tenant_id", tenantId},
            {"store_id", storeId},
            {"cashier_id", cashierId},
            {"start_time", startTime.toString(Qt::ISODate)},
            {"end_time", endTime.toString(Qt::ISODate)},
            {"start_cash", startCash},
            {"end_cash", endCash},
            {"status", status},
            {"sync_status", syncStatus}
        };
    }

    static Shift fromJson(const QJsonObject &json) {
        Shift s;
        s.cloudId = json["cloud_id"].toString();
        s.tenantId = json["tenant_id"].toString();
        s.storeId = json["store_id"].toString();
        s.cashierId = json["cashier_id"].toInt();
        s.startTime = QDateTime::fromString(json["start_time"].toString(), Qt::ISODate);
        s.endTime = QDateTime::fromString(json["end_time"].toString(), Qt::ISODate);
        s.startCash = json["start_cash"].toDouble();
        s.endCash = json["end_cash"].toDouble();
        s.status = json["status"].toInt(1);
        s.syncStatus = json["sync_status"].toInt();
        return s;
    }
};
