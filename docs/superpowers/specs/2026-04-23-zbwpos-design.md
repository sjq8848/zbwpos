# ZBW POS 收银端设计文档

## 概述

基于Qt5.15/Qt6双版本构建的SaaS零售POS跨平台收银客户端。面向通用零售业态（混合），支持离线优先+云同步，运行于Windows 7+、macOS、Linux、Android。

## 需求摘要

| 项目 | 决定 |
|------|------|
| 零售业态 | 通用零售（混合业态） |
| 目标平台 | Win7+ (Qt5.15)、Win10+/macOS/Linux/Android (Qt6) |
| 数据模式 | 离线优先 + 云同步 |
| 后端 | 已有 REST API + JSON，JWT认证 |
| SaaS模式 | 多租户、多门店，设备绑定单门店 |
| 第一版模块 | 收银台、商品管理、会员管理 |
| 外设 | 扫码枪、小票打印机、钱箱、顾客显示屏 |
| 支付 | 现金、银行卡、微信/支付宝、储值/会员卡 |
| Qt版本 | Qt5.15 + Qt6 双版本构建 |
| 界面 | QML（触屏收银）+ Widgets（管理表单） |
| 构建 | qmake |
| 语言 | C++为主 + QML/JS |
| 开发 | 2-3人团队 + Claude Code辅助 |

---

## 1. 整体架构：MVVM + 服务层

```
┌─────────────────────────────────────────────────────────┐
│                      View 层                             │
│  ┌─────────────────┐    ┌─────────────────────────────┐  │
│  │   QML 视图       │    │   Widgets 视图               │  │
│  │  • 收银主界面    │    │  • 商品管理表格              │  │
│  │  • 快捷操作面板  │    │  • 会员列表/编辑             │  │
│  │  • 支付弹窗      │    │  • 报表查询界面              │  │
│  └────────┬────────┘    └─────────────┬───────────────┘  │
└───────────┼───────────────────────────┼───────────────────┘
            │ QML属性绑定                │ 信号/槽
            ▼                           ▼
┌─────────────────────────────────────────────────────────┐
│                    ViewModel 层                          │
│  • CashierVM: 当前订单、购物车、支付流程状态             │
│  • CartVM: 购物车数据、小计、折扣                        │
│  • PaymentVM: 支付方式、金额、找零                       │
│  • ProductVM: 商品列表、搜索、分类筛选                   │
│  • MemberVM: 会员信息、积分、储值                        │
│  • 暴露 Q_PROPERTY 供 QML 绑定                           │
│  • 提供 Q_INVOKABLE 方法响应 QML 操作                    │
└─────────────────────────┬───────────────────────────────┘
                          │ 调用
                          ▼
┌─────────────────────────────────────────────────────────┐
│                    Service 层                            │
│  • OrderService: 订单创建、支付、退货、挂单             │
│  • PaymentService: 支付执行、移动支付对接               │
│  • ProductService: 商品CRUD、库存查询                    │
│  • MemberService: 会员查询、积分操作、储值               │
│  • SyncService: 后端同步、冲突解决                       │
│  • AuthService: JWT认证、登录登出                        │
├─────────────────────────────────────────────────────────┤
│  中间件/横切关注点                                       │
│  • Logger: 操作日志                                     │
│  • Validator: 数据校验                                  │
└─────────────────────────┬───────────────────────────────┘
                          │ 调用
                          ▼
┌─────────────────────────────────────────────────────────┐
│                   Repository 层                          │
│  • ProductRepo: 本地SQLite + 远程API双源                │
│  • MemberRepo: 本地SQLite + 远程API双源                 │
│  • OrderRepo: 本地订单存储 + 批量同步                    │
│  • OfflineQueue: 操作记录，网络恢复后重试               │
└─────────────────────────┬───────────────────────────────┘
                          │
                          ▼
┌─────────────────────────────────────────────────────────┐
│                   基础设施层                             │
│  • Database: SQLite本地库（QtSql模块）                   │
│  • Network: QNetworkAccessManager + REST客户端          │
│  • Hardware: 外设抽象接口（打印机/扫码枪/钱箱）          │
│  • Setting: 配置持久化（QSettings）                      │
└─────────────────────────────────────────────────────────┘
```

**设计原则：**
- ViewModel是QML与C++的桥梁，所有QML数据绑定都通过VM
- Service层封装业务规则，处理同步逻辑
- Repository双源模式：本地优先，后台同步
- 层间通过接口解耦，支持单元测试Mock

---

## 2. 目录结构

```
zbwpos/
├── src/
│   ├── core/                      # 核心基础设施
│   │   ├── database/              # SQLite封装
│   │   │   ├── DatabaseManager.cpp/h
│   │   │   └── Migration.cpp/h
│   │   ├── network/               # 网络层
│   │   │   ├── ApiClient.cpp/h
│   │   │   ├── AuthManager.cpp/h
│   │   │   ├── SyncManager.cpp/h
│   │   │   ├── OfflineQueue.cpp/h
│   │   │   └── endpoints/
│   │   │       ├── ProductApi.cpp/h
│   │   │       ├── MemberApi.cpp/h
│   │   │       ├── OrderApi.cpp/h
│   │   │       └── AuthApi.cpp/h
│   │   ├── hardware/              # 硬件抽象
│   │   │   ├── IHardwareDevice.h
│   │   │   ├── IBarcodeScanner.h
│   │   │   ├── IPrinter.h
│   │   │   ├── ICashDrawer.h
│   │   │   ├── ICustomerDisplay.h
│   │   │   ├── HardwareManager.cpp/h
│   │   │   ├── DeviceFactory.cpp/h
│   │   │   └── platform/
│   │   │       ├── BarcodeScannerUsb.cpp/h
│   │   │       ├── PrinterEscPos.cpp/h
│   │   │       ├── CashDrawerSerial.cpp/h
│   │   │       ├── windows/
│   │   │       ├── linux/
│   │   │       ├── macos/
│   │   │       └── android/
│   │   ├── settings/
│   │   │   └── AppSettings.cpp/h
│   │   ├── QtCompat.h             # Qt5/Qt6兼容层
│   │   └── Common.h
│   │
│   ├── cashier/                   # 收银模块
│   │   ├── viewmodel/
│   │   │   ├── CashierVM.cpp/h
│   │   │   ├── CartVM.cpp/h
│   │   │   ├── PaymentVM.cpp/h
│   │   │   ├── ProductSearchVM.cpp/h
│   │   │   └── models/
│   │   │       ├── CartItemModel.cpp/h
│   │   │       └── PaymentMethodModel.cpp/h
│   │   ├── service/
│   │   │   ├── OrderService.cpp/h
│   │   │   ├── PaymentService.cpp/h
│   │   │   └── ShiftService.cpp/h
│   │   ├── repository/
│   │   │   └── OrderRepo.cpp/h
│   │   └── qml/
│   │       ├── CashierPage.qml
│   │       ├── components/
│   │       │   ├── TopStatusBar.qml
│   │       │   ├── CategoryTabBar.qml
│   │       │   ├── ProductGrid.qml
│   │       │   ├── ProductCard.qml
│   │       │   ├── CartPanel.qml
│   │       │   ├── CartItemDelegate.qml
│   │       │   ├── BarcodeInput.qml
│   │       │   ├── MemberInfo.qml
│   │       │   ├── ActionButtonBar.qml
│   │       │   └── NumberPad.qml
│   │       ├── dialogs/
│   │       │   ├── PaymentDialog.qml
│   │       │   ├── MemberSearchDialog.qml
│   │       │   ├── ProductSearchDialog.qml
│   │       │   ├── SuspendOrderDialog.qml
│   │       │   ├── RefundDialog.qml
│   │       │   └── ShiftDialog.qml
│   │       └── styles/
│   │           ├── AppColors.qml
│   │           ├── AppFonts.qml
│   │           └── AppTheme.qml
│   │
│   ├── product/                   # 商品模块
│   │   ├── viewmodel/
│   │   │   └── ProductVM.cpp/h
│   │   ├── service/
│   │   │   ├── ProductService.cpp/h
│   │   │   └── CategoryService.cpp/h
│   │   ├── repository/
│   │   │   ├── ProductRepo.cpp/h
│   │   │   └── CategoryRepo.cpp/h
│   │   └── qml/
│   │       └── ProductManagePage.qml
│   │
│   ├── member/                    # 会员模块
│   │   ├── viewmodel/
│   │   │   └── MemberVM.cpp/h
│   │   ├── service/
│   │   │   └── MemberService.cpp/h
│   │   ├── repository/
│   │   │   └── MemberRepo.cpp/h
│   │   └── qml/
│   │       └── MemberPage.qml
│   │
│   └── app/                       # 应用入口
│       ├── main.cpp
│       ├── Application.cpp/h
│       ├── MainWindow.cpp/h
│       ├── NavigationManager.cpp/h
│       └── qml/
│           ├── main.qml
│           └── AppNavigation.qml
│
├── resources/
│   ├── qml.qrc
│   ├── images/
│   ├── fonts/
│   ├── icons/
│   └── translations/
│
├── tests/
│   ├── unit/
│   └── integration/
│
├── android/
│   ├── AndroidManifest.xml
│   └── android.pri
│
├── zbwpos.pro
├── common.pri
├── qt5_compat.pri
├── qt6_compat.pri
├── docs/
└── README.md
```

---

## 3. 网络与同步层

### 数据流向

```
UI操作 → ViewModel → Service → Repository
                             │
                             ▼
             ┌─────────────────────────────┐
             │      本地SQLite（主数据源）   │   ← 离线可用
             └─────────────┬───────────────┘
                           │
                           ▼
             ┌─────────────────────────────┐
             │     SyncManager（后台线程）   │
             │  • 监听网络状态               │
             │  • 处理OfflineQueue          │
             │  • 增量同步到云端             │
             └─────────────┬───────────────┘
                           │
                           ▼
             ┌─────────────────────────────┐
             │      云端REST API            │   ← 在线同步
             │  • JWT认证                  │
             │  • 商品/会员/订单CRUD        │
             └─────────────────────────────┘
```

### 关键组件

**ApiClient** - REST API客户端基类
- setAuthToken(token) / clearAuthToken()
- get/post/put/del 方法
- 信号：unauthorized() / networkError(code) / rateLimited()

**AuthManager** - JWT管理
- login(username, password) → AuthResult
- currentToken() / refreshToken() / isTokenValid() / isTokenExpiringSoon()
- refreshAccessToken()
- logout()
- 信号：loginSuccess / loginFailed / tokenRefreshed / tokenExpired

**SyncManager** - 同步调度器
- syncAll() / syncProducts() / syncMembers() / syncOrders()
- status() / pendingCount() / lastSyncTime()
- resolveConflict(tableName, recordId, strategy)
- 信号：syncStarted / syncProgress / syncCompleted / syncError / conflictDetected

**OfflineQueue** - 离线操作队列
- push(operation, tableName, recordId, payload)
- getPending() → QList<OfflineTask>
- markCompleted(taskId) / markFailed(taskId, error)
- retry(taskId) / retryAll()
- cleanup(daysToKeep)

### 同步策略
- 本地SQLite是主数据源，所有读操作优先查本地
- 写操作立即写入本地，同时加入OfflineQueue
- SyncManager后台轮询，有网络时自动同步队列
- JWT token安全存储：使用平台原生密钥库（Windows DPAPI / macOS Keychain / Linux libsecret / Android Keystore），通过qt5keychain或qt6keychain库统一接口；token绝不以明文存储

### API端点结构
```
src/core/network/endpoints/
├── ProductApi.cpp/h     # /api/products
├── MemberApi.cpp/h      # /api/members
├── OrderApi.cpp/h       # /api/orders
└── AuthApi.cpp/h        # /api/auth
```

API数据模型在 `src/core/network/models/` 中定义（JSON序列化）：
- ProductDto.h / MemberDto.h / OrderDto.h / ApiResponse.h

---

## 4. 本地数据模型（SQLite）

### SaaS多租户设计
- 所有业务表带 `tenant_id`（租户/企业ID）和 `store_id`（门店ID）
- 设备启动时绑定租户+门店，写入 tenant_config 表
- `cloud_id` 标识云端记录唯一ID
- 会员表只带 `tenant_id`（会员租户级共享，跨门店）
- 订单、班次带 `tenant_id` + `store_id`（门店级数据）
- UNIQUE约束加入租户维度

### 数据库表

```sql
-- 租户配置表（登录后从云端拉取）
CREATE TABLE tenant_config (
    id              INTEGER PRIMARY KEY,
    tenant_id       TEXT NOT NULL,
    tenant_name     TEXT,
    store_id        TEXT NOT NULL,
    store_name      TEXT,
    config_json     TEXT,
    bind_time       DATETIME
);

-- 商品表
CREATE TABLE products (
    id              INTEGER PRIMARY KEY AUTOINCREMENT,
    cloud_id        TEXT,
    tenant_id       TEXT NOT NULL,
    store_id        TEXT NOT NULL,
    barcode         TEXT NOT NULL,
    sku             TEXT,
    name            TEXT NOT NULL,
    category_id     INTEGER,
    unit            TEXT DEFAULT '件',
    price           DECIMAL(10,2) NOT NULL,
    cost_price      DECIMAL(10,2),
    stock_qty       DECIMAL(10,3) DEFAULT 0,
    min_stock       DECIMAL(10,3) DEFAULT 0,
    status          INTEGER DEFAULT 1,
    image_url       TEXT,
    sync_status     INTEGER DEFAULT 0,
    created_at      DATETIME DEFAULT CURRENT_TIMESTAMP,
    updated_at      DATETIME DEFAULT CURRENT_TIMESTAMP,
    UNIQUE(tenant_id, store_id, barcode)
);

-- 商品分类表
CREATE TABLE categories (
    id              INTEGER PRIMARY KEY,
    cloud_id        TEXT,
    tenant_id       TEXT NOT NULL,
    name            TEXT NOT NULL,
    parent_id       INTEGER,
    sort_order      INTEGER DEFAULT 0,
    sync_status     INTEGER DEFAULT 0
);

-- 会员表
CREATE TABLE members (
    id              INTEGER PRIMARY KEY AUTOINCREMENT,
    cloud_id        TEXT,
    tenant_id       TEXT NOT NULL,
    card_no         TEXT,
    name            TEXT,
    phone           TEXT,
    gender          INTEGER DEFAULT 0,
    birthday        DATE,
    level_id        INTEGER,
    points          INTEGER DEFAULT 0,
    balance         DECIMAL(10,2) DEFAULT 0,
    total_spent     DECIMAL(10,2) DEFAULT 0,
    status          INTEGER DEFAULT 1,
    sync_status     INTEGER DEFAULT 0,
    created_at      DATETIME DEFAULT CURRENT_TIMESTAMP,
    updated_at      DATETIME DEFAULT CURRENT_TIMESTAMP,
    UNIQUE(tenant_id, card_no)
);

-- 会员等级表
CREATE TABLE member_levels (
    id              INTEGER PRIMARY KEY,
    cloud_id        TEXT,
    tenant_id       TEXT NOT NULL,
    name            TEXT NOT NULL,
    discount        DECIMAL(3,2) DEFAULT 1.00,
    min_points      INTEGER DEFAULT 0
);

-- 订单主表
CREATE TABLE orders (
    id              INTEGER PRIMARY KEY AUTOINCREMENT,
    cloud_id        TEXT,
    tenant_id       TEXT NOT NULL,
    store_id        TEXT NOT NULL,
    order_no        TEXT UNIQUE NOT NULL,
    member_id       INTEGER,
    total_amount    DECIMAL(10,2) NOT NULL,
    discount_amount DECIMAL(10,2) DEFAULT 0,
    final_amount    DECIMAL(10,2) NOT NULL,
    payment_method  TEXT NOT NULL,
    status          INTEGER DEFAULT 1,
    cashier_id      INTEGER,
    shift_id        INTEGER,
    remark          TEXT,
    sync_status     INTEGER DEFAULT 0,
    created_at      DATETIME DEFAULT CURRENT_TIMESTAMP
);

-- 订单明细表
CREATE TABLE order_items (
    id              INTEGER PRIMARY KEY,
    order_id        INTEGER NOT NULL,
    product_id      INTEGER NOT NULL,
    barcode         TEXT NOT NULL,
    product_name    TEXT NOT NULL,
    unit_price      DECIMAL(10,2) NOT NULL,
    quantity        DECIMAL(10,3) NOT NULL,
    discount_rate   DECIMAL(3,2) DEFAULT 1.00,
    subtotal        DECIMAL(10,2) NOT NULL,
    FOREIGN KEY (order_id) REFERENCES orders(id)
);

-- 支付记录表
CREATE TABLE payments (
    id              INTEGER PRIMARY KEY,
    order_id        INTEGER NOT NULL,
    method          TEXT NOT NULL,
    amount          DECIMAL(10,2) NOT NULL,
    reference_no    TEXT,
    status          INTEGER DEFAULT 1,
    created_at      DATETIME DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (order_id) REFERENCES orders(id)
);

-- 离线操作队列表
CREATE TABLE offline_queue (
    id              INTEGER PRIMARY KEY,
    tenant_id       TEXT NOT NULL,
    store_id        TEXT NOT NULL,
    operation       TEXT NOT NULL,
    table_name      TEXT NOT NULL,
    record_id       INTEGER NOT NULL,
    payload         TEXT,
    retry_count     INTEGER DEFAULT 0,
    last_error      TEXT,
    created_at      DATETIME DEFAULT CURRENT_TIMESTAMP
);

-- 收银员表
CREATE TABLE cashiers (
    id              INTEGER PRIMARY KEY,
    cloud_id        TEXT,
    tenant_id       TEXT NOT NULL,
    username        TEXT NOT NULL,
    password_hash   TEXT NOT NULL,
    name            TEXT,
    role            TEXT DEFAULT 'cashier',
    status          INTEGER DEFAULT 1,
    sync_status     INTEGER DEFAULT 0,
    UNIQUE(tenant_id, username)
);

-- 班次表
CREATE TABLE shifts (
    id              INTEGER PRIMARY KEY,
    cloud_id        TEXT,
    tenant_id       TEXT NOT NULL,
    store_id        TEXT NOT NULL,
    cashier_id      INTEGER NOT NULL,
    start_time      DATETIME NOT NULL,
    end_time        DATETIME,
    start_cash      DECIMAL(10,2),
    end_cash        DECIMAL(10,2),
    status          INTEGER DEFAULT 1,
    sync_status     INTEGER DEFAULT 0
);
```

### 设计要点
- 订单明细冗余存储商品名，避免商品改名后历史订单显示异常
- 离线队列记录操作快照，支持失败重试和冲突处理
- sync_status: 0=已同步, 1=待上传, 2=冲突
- **分类表说明**：`categories`表是租户级数据（非门店级），分类在租户内统一管理，故无`store_id`，UNIQUE约束为`UNIQUE(tenant_id, name)`
- **子表冗余设计**：`order_items`和`payments`通过`order_id`关联订单，查询时通过JOIN获取租户信息；为支持直接数据隔离审计，订单明细加`tenant_id`

```sql
-- 订单明细表（补充tenant_id用于数据隔离）
CREATE TABLE order_items (
    id              INTEGER PRIMARY KEY,
    tenant_id       TEXT NOT NULL,              -- 冗余字段，便于数据隔离
    order_id        INTEGER NOT NULL,
    product_id      INTEGER NOT NULL,
    barcode         TEXT NOT NULL,
    product_name    TEXT NOT NULL,
    unit_price      DECIMAL(10,2) NOT NULL,
    quantity        DECIMAL(10,3) NOT NULL,
    discount_rate   DECIMAL(3,2) DEFAULT 1.00,
    subtotal        DECIMAL(10,2) NOT NULL,
    FOREIGN KEY (order_id) REFERENCES orders(id)
);

-- 支付记录表（补充tenant_id用于数据隔离）
CREATE TABLE payments (
    id              INTEGER PRIMARY KEY,
    tenant_id       TEXT NOT NULL,              -- 冗余字段，便于数据隔离
    order_id        INTEGER NOT NULL,
    method          TEXT NOT NULL,
    amount          DECIMAL(10,2) NOT NULL,
    reference_no    TEXT,
    status          INTEGER DEFAULT 1,
    created_at      DATETIME DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (order_id) REFERENCES orders(id)
);

-- 商品分类表（补充UNIQUE约束）
CREATE TABLE categories (
    id              INTEGER PRIMARY KEY,
    cloud_id        TEXT,
    tenant_id       TEXT NOT NULL,
    name            TEXT NOT NULL,
    parent_id       INTEGER,
    sort_order      INTEGER DEFAULT 0,
    sync_status     INTEGER DEFAULT 0,
    UNIQUE(tenant_id, name, parent_id)
);
```

---

## 5. 硬件抽象层

### 接口层次

```
IHardwareDevice              # 基础设备接口
├── IBarcodeScanner          # 扫码枪
├── IPrinter                 # 打印机
├── ICashDrawer              # 钱箱
└── ICustomerDisplay         # 顾客显示屏
```

### IHardwareDevice 基础接口
- open() / close() / isConnected() / deviceName() / lastError()

### IBarcodeScanner
- startScanning() / stopScanning()
- 信号：barcodeRead(QString barcode)

### IPrinter
- printReceipt(ReceiptData) / printTestPage() / cutPaper() / openCashDrawer() / printerStatus()

### ICashDrawer
- open() / isOpen()

### ICustomerDisplay
- showWelcome() / showItem(name, price) / showTotal(amount) / showPayment(paid, change) / clear()

### HardwareManager
统一管理所有外设的生命周期，ViewModel通过HardwareManager间接使用设备。DeviceFactory根据平台创建具体实现。

**扫码枪事件路由机制：**
大多数USB扫码枪模拟键盘输入，事件流为：
```
扫码枪 ─(键盘事件)─► QML窗口 ─► 全局事件过滤器 ─► CashierVM.scanBarcode()
```

实现方式：
1. Application层安装全局事件过滤器，监听KeyPress事件
2. 检测短时间内连续输入+回车结尾 → 判定为条码扫描
3. 过滤掉输入框焦点状态（对话框打开时，扫码输入到输入框而非直接加入购物车）
4. 过滤后条码通过信号传递给CashierVM
5. CashierVM调用ProductService.findByBarcode()

**BarcodeInput.qml** 职责：
- 提供手动输入条码的入口
- 显示扫码状态（扫码成功/失败动画）
- 不处理扫码枪硬件事件（由全局过滤器处理）

### 平台策略

| 外设 | 桌面端（Win/Mac/Linux） | Android端 |
|------|------------------------|-----------|
| 扫码枪 | 键盘模拟事件监听（通用） | 键盘模拟 + 蓝牙扫码枪 |
| 小票打印机 | ESC/POS协议（串口/USB） | 蓝牙ESC/POS打印机 |
| 钱箱 | 打印机RJ11口联动/串口控制 | 一般不使用 |
| 顾客显示屏 | 串口/USB/VFD协议 | 一般不使用 |

### 文件结构
```
src/core/hardware/
├── IHardwareDevice.h / IBarcodeScanner.h / IPrinter.h / ICashDrawer.h / ICustomerDisplay.h
├── HardwareManager.cpp/h / DeviceFactory.cpp/h
└── platform/
    ├── BarcodeScannerUsb.cpp/h     # USB扫码枪（全平台通用）
    ├── PrinterEscPos.cpp/h         # ESC/POS热敏打印机（全平台通用）
    ├── CashDrawerSerial.cpp/h      # 串口钱箱（全平台通用）
    ├── windows/                    # Windows特有实现
    ├── linux/                      # Linux特有实现
    ├── macos/                      # macOS特有实现
    └── android/                    # Android特有实现
```

---

## 6. ViewModel层

### 核心ViewModel

**CashierVM** - 收银台主控制器
- 属性：cart(CartVM*) / productSearch(ProductSearchVM*) / payment(PaymentVM*) / currentMember / isProcessing / statusMessage
- 方法：scanBarcode() / inputBarcode() / selectMember() / clearMember() / suspendOrder() / resumeOrder() / startPayment() / clearAll()
- 信号：memberChanged / processingChanged / statusChanged / showPaymentDialog / orderCompleted

**CartVM** - 购物车
- 属性：items(QAbstractListModel*) / itemCount / subtotal / discount / total / memberId
- 方法：addItem() / removeItem() / updateQuantity() / applyDiscount() / clear() / getItemData()
- 信号：cartChanged / memberChanged

**PaymentVM** - 支付流程（支持组合支付）
- 属性：amountDue / amountPaid / change / currentMethod / paymentRecords(QAbstractListModel*) / canComplete / paymentState
- 方法：setMethod() / inputAmount() / quickCash() / addPartialPayment() / removePartialPayment(int index) / complete() / cancel()
- 信号：amountChanged / methodChanged / paymentCompleted / paymentFailed / partialPaymentAdded
- **支付状态机**：Idle → SelectingMethod → InputtingAmount → PartialPaid → Completed
- **组合支付**：一笔订单可用多种支付方式（如现金+微信），每笔部分支付记录到paymentRecords，amountDue实时更新剩余金额

**ProductSearchVM** - 商品搜索
- 属性：searchResults(QAbstractListModel*) / keyword / isSearching
- 方法：search() / selectProduct()
- 信号：resultsChanged / productSelected

**MemberVM** - 会员
- 属性：currentMember / searchResults / isSearching
- 方法：findByCardNo() / findByPhone() / clear()
- 信号：memberChanged / resultsChanged

### 数据模型
- CartItemModel(QAbstractListModel) - 购物车列表
- PaymentMethodModel(QAbstractListModel) - 支付方式
- PaymentRecordModel(QAbstractListModel) - 组合支付记录列表

**ShiftVM** - 班次管理
- 属性：currentShift / shiftOrders / totalSales / cashInDrawer
- 方法：startShift(startCash) / endShift(endCash) / getShiftSummary()
- 信号：shiftStarted / shiftEnded

### ViewModel职责边界
- 只做：数据转换、QML交互、界面状态管理、调用Service
- 不做：业务规则判断、数据持久化、网络请求（这些在Service/Repository层）

---

## 7. Service层

### 核心Service

**OrderService** - 订单业务
- createOrder(OrderRequest) → OrderResult
- completeOrder(orderNo, PaymentResult)
- suspendOrder(orderId) / resumeOrder(orderId)
- refundOrder(orderNo, RefundRequest) → OrderResult
- getOrder(orderNo) / getTodayOrders() / getSuspendedOrders()
- 信号：orderCreated / orderCompleted

**PaymentService** - 支付业务
- pay(orderNo, PaymentRequest) → PaymentResult
- isPaymentMethodAvailable(method)
- payWithMemberBalance(orderNo, memberId, amount)
- prepareMobilePayment(orderNo, method, amount) → QJsonObject
- checkMobilePaymentStatus(transactionId)
- 信号：paymentCompleted / paymentFailed
- 
**移动支付流程（微信/支付宝）：**
```
1. createOrder() → 本地记录订单，状态=待支付
2. prepareMobilePayment() → 调用云端API获取支付二维码URL
   - 返回：{ qrCodeUrl, transactionId, expireTime }
   - QML显示二维码，等待用户扫码
3. 轮询 checkMobilePaymentStatus(transactionId)
   - 间隔：2秒，超时：120秒
   - 返回：pending / success / failed / expired
4. success → completeOrder()
   failed/expired → 提示用户，允许重试或取消

**离线场景处理：**
- 无网络时，移动支付方式不可选（isPaymentMethodAvailable返回false）
- QML支付界面禁用移动支付选项并提示"需要网络连接"
```

**ProductService** - 商品业务
- findByBarcode(barcode) / search(keyword, limit) / getByCategory(categoryId) / getFavorites()
- updatePrice(productId, newPrice) / updateStock(productId, quantity, reason)
- 信号：productUpdated / stockLowWarning

**MemberService** - 会员业务
- findByCardNo(cardNo) / findByPhone(phone)
- addPoints(memberId, points) / usePoints(memberId, points)
- addBalance(memberId, amount) / deductBalance(memberId, amount)
- calculateDiscount(member, originalAmount)
- 信号：memberUpdated / pointsChanged / balanceChanged

**SyncService** - 同步服务
- syncAll() / syncProducts() / syncMembers() / syncOrders()
- status() / pendingCount() / lastSyncTime()
- resolveConflict(tableName, recordId, strategy)
- 信号：syncStarted / syncProgress / syncCompleted / syncError / conflictDetected

**AuthService** - 认证服务
- login(username, password) / logout()
- 信号：loginSuccess / loginFailed / tokenExpired

**ShiftService** - 班次管理
- startShift(cashierId, startCash) → Shift
- endShift(shiftId, endCash) → ShiftSummary
- getCurrentShift() → Shift
- getShiftOrders(shiftId) → QList<Order>
- 信号：shiftStarted / shiftEnded

### Service层特点
- 每个Service是单例，由Application管理生命周期
- Service之间可以互相调用（如PaymentService调用MemberService扣储值）
- 所有异步操作通过信号通知结果
- 不直接操作数据库/网络，通过Repository层

---

## 8. Repository层

### 双源模式
- 读操作：本地优先，本地没有则从云端拉取并缓存
- 写操作：先写本地SQLite，再入OfflineQueue等待同步

### BaseRepo模板
```cpp
// Repository基类 - 本地ID为int，云端ID为QString
template<typename T>
class BaseRepo {
public:
    // 按本地ID查询
    T findById(int id);
    // 按云端ID查询
    T findByCloudId(const QString &cloudId);
    
    QList<T> findAll();
    bool save(const T &item);           // item含int id（本地）和QString cloudId（云端）
    bool remove(int id);
    
protected:
    virtual T localFindById(int id) = 0;
    virtual T localFindByCloudId(const QString &cloudId) = 0;
    virtual QList<T> localFindAll() = 0;
    virtual bool localSave(const T &item) = 0;
    virtual bool localRemove(int id) = 0;
    virtual T remoteFindByCloudId(const QString &cloudId) = 0;
    virtual void markSyncPending(int id) = 0;
};
```

### ProductRepo
- 基类方法实现
- 商品特有：findByBarcode / search / getByCategory / getLowStock
- 批量同步：syncFromCloud(cloudProducts)

### OrderRepo
- 订单本地存储为主
- saveOrder / updateOrderStatus
- findByOrderNo / findByDateRange / findTodayOrders / findSuspended
- 同步上传：getPendingUpload / markSynced

### MemberRepo
- 会员查询：findByCardNo / findByPhone
- 余额操作：updateBalance / updatePoints

---

## 9. QML界面设计

### 收银台主界面布局

```
┌──────────────────────────────────────────────────────────────────┐
│  顶部状态栏                                                       │
│  [门店名称]  [收银员:张三]  [班次:08:00-17:00]  [在线✓]  [设置]   │
├──────────────────────────────┬───────────────────────────────────┤
│                              │                                   │
│    商品快捷面板               │         购物车区域                │
│  ┌────┐ ┌────┐ ┌────┐       │   ┌─────────────────────────────┐ │
│  │饮料│ │零食│ │日用│ ...    │   │ 商品名        单价  数量 小计│ │
│  └────┘ └────┘ └────┘       │   ├─────────────────────────────┤ │
│  ┌──────────────────────┐   │   │ 可乐500ml     3.00   2   6.00│ │
│  │ [商品图片]           │   │   │ 薯片原味      5.50   1   5.50│ │
│  │ [商品图片]           │   │   └─────────────────────────────┘ │
│  │ ...更多商品...       │   │                                   │
│  └──────────────────────┘   │   会员: 138****1234  折扣: -¥2.00 │
│                              │   应收: ¥44.50                   │
│  ┌──────────────────────┐   │                                   │
│  │ 条码输入框           │   │   [挂单(2)]  [清空]  [结算]       │
│  └──────────────────────┘   │                                   │
├──────────────────────────────┴───────────────────────────────────┤
│  [会员查询] [商品查询] [退货] [交班] [更多...]                    │
└──────────────────────────────────────────────────────────────────┘
```

### QML文件结构
```
src/cashier/qml/
├── CashierPage.qml
├── components/
│   ├── TopStatusBar.qml
│   ├── CategoryTabBar.qml
│   ├── ProductGrid.qml
│   ├── ProductCard.qml
│   ├── CartPanel.qml
│   ├── CartItemDelegate.qml
│   ├── BarcodeInput.qml
│   ├── MemberInfo.qml
│   ├── ActionButtonBar.qml
│   └── NumberPad.qml
├── dialogs/
│   ├── PaymentDialog.qml
│   ├── MemberSearchDialog.qml
│   ├── ProductSearchDialog.qml
│   ├── SuspendOrderDialog.qml
│   ├── RefundDialog.qml
│   └── ShiftDialog.qml
└── styles/
    ├── AppColors.qml
    ├── AppFonts.qml
    └── AppTheme.qml
```

### QML与C++绑定
- 所有ViewModel在C++中通过setContextProperty()暴露给QML
- 列表数据使用QAbstractListModel子类，支持动态刷新
- 复杂对象用Q_GADGET或Q_OBJECT包装，暴露为Q_PROPERTY
- **QML import版本策略**：Qt5使用版本化import（`import QtQuick 2.15`），Qt6使用无版本import（`import QtQuick`），通过QtCompat.h中的宏在构建时切换：
  ```cpp
  #if ZBW_QT6
  #define ZBW_QML_IMPORT(module) #module
  #else
  #define ZBW_QML_IMPORT_HELPER2(module, ver) #module " " #ver
  #define ZBW_QML_IMPORT_HELPER(module, ver) ZBW_QML_IMPORT_HELPER2(module, ver)
  #define ZBW_QML_IMPORT(module) ZBW_QML_IMPORT_HELPER(module, 2.15)
  #endif
  ```
  或在QML端使用条件判断：
  ```qml
  // 通过C++暴露的布尔属性 isQt6 控制组件版本
  ```

---

## 10. Qt5/Qt6双版本兼容策略

### 构建系统

**zbwpos.pro** - 顶层项目文件，自动检测Qt版本，包含common.pri和版本专用.pri

**common.pri** - 共享编译配置
- C++17标准
- 公共Qt模块：core gui widgets sql network
- 公共定义：QT_DEPRECATED_WARNINGS, QT_NO_CAST_TO_ASCII, QT_STRICT_ITERATORS

**qt5_compat.pri** - Qt5专用
- QT += qml quick quickcontrols2 quickwidgets
- DEFINES += USE_QT5

**qt6_compat.pri** - Qt6专用
- QT += qml quick quickcontrols2
- DEFINES += USE_QT6
- CONFIG += qtquickcompiler

### C++兼容层 (QtCompat.h)

提供统一的版本判断宏和API差异封装：
- ZBW_QT6 / ZBW_QT5 编译期宏
- 关键API差异封装：滚轮事件、QFileInfo、QML注册方式等
- 核心原则：差异集中在QtCompat.h和.pri配置文件，业务代码零感知

### QML兼容
- import版本号统一用2.15
- 避免使用Qt6新增的QML类型和属性
- QtCompat.js提供QML层兼容工具

### Android平台差异
```qmake
android {
    equals(QT_MAJOR_VERSION, 5) {
        ANDROID_MIN_SDK_VERSION = 21
        ANDROID_TARGET_SDK_VERSION = 28
    }
    equals(QT_MAJOR_VERSION, 6) {
        ANDROID_MIN_SDK_VERSION = 23
        ANDROID_TARGET_SDK_VERSION = 33
    }
    DEFINES += ZBW_PLATFORM_ANDROID
}
!android {
    DEFINES += ZBW_PLATFORM_DESKTOP
}
```

### 构建与发布策略

| 项目 | 策略 |
|------|------|
| 构建CI | 两套构建：Qt5.15 + Qt6.x，分别验证 |
| 代码差异 | 统一通过QtCompat.h宏隔离，避免散落各处 |
| QML | import版本用2.15，不使用Qt6专属特性 |
| 测试 | 两套Qt版本都跑完整测试 |
| 发布 | Win7 → Qt5.15构建；Win10+/Mac/Linux/Android → Qt6构建 |
| 硬件驱动 | 桌面端ESC/POS串口代码两版本通用；Android蓝牙代码两版本通用 |

---

## 11. 错误处理

### 分层错误处理策略
- **Repository层**：数据库异常、网络异常，转为Result对象向上传递
- **Service层**：业务规则校验失败，抛出业务异常；网络异常转为友好提示
- **ViewModel层**：捕获异常，更新statusMessage属性，QML自动显示
- **QML层**：只负责展示，不处理异常逻辑

### 同步冲突处理
- 冲突检测：云端更新时间 > 本地更新时间
- 冲突策略枚举：UseLocal / UseRemote / ManualMerge
- 冲突信号通知ViewModel，由用户决定

**自动解决规则（高优先级，无需用户介入）：**
1. 新增记录冲突：本地新增但云端也有同cloud_id → UseRemote（以云端为准）
2. 本地删除但云端已更新：UseRemote（恢复云端数据）
3. 订单类数据：UseLocal（订单以收银端为准，云端不应修改已提交订单）

**需要用户介入的场景：**
- 商品价格在云端和本地都被修改
- 会员信息在本地和云端都有更新

**ManualMerge流程：**
1. 检测到冲突 → emit conflictDetected信号
2. ViewModel弹窗展示：本地值、云端值、最后同步时间
3. 用户选择：保留本地 / 采用云端 / 手动编辑
4. 选择后调用 resolveConflict(tableName, recordId, strategy)
5. 记录解决日志，更新本地sync_status=0

---

## 12. 测试策略

### 单元测试
- Service层：Mock Repository，验证业务逻辑
- ViewModel层：验证属性变化和信号触发
- Repository层：使用内存SQLite，验证SQL和同步逻辑

### 集成测试
- 数据库Migration测试
- API客户端与Mock Server交互测试
- 同步流程端到端测试

### 平台测试
- Qt5.15构建 + Win7运行验证
- Qt6构建 + Win10/macOS/Linux/Android运行验证
