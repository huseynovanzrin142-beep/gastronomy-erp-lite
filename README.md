# 🍽️ GastroLogix: Enterprise Restaurant Management Engine

**GastroLogix** is a high-performance C++ backend system designed to bridge the gap between raw inventory management and customer-facing POS services. Built with strict adherence to **OOP (Object-Oriented Programming)** principles, it ensures data integrity through robust exception handling and automated resource tracking.

## 🌟 Key Engineering Features

### 1. Granular Inventory Architecture
*   **Ingredient Profiling:** Tracks macronutrients (Proteins, Carbs, Fats) and micronutrients (Vitamins, Minerals) per ingredient.
*   **Dynamic Stock Depletion:** Intelligent logic that reduces stock levels based on complex meal compositions during checkout.
*   **Cost Analysis:** Automatic calculation of manufacturing costs vs. sale prices to monitor profit margins.

### 2. Multi-Tier Product Hierarchy
*   **Atomic Level (`Ingredient`):** The base unit with pricing and stock.
*   **Composite Level (`Food`):** Recipes defined by weight-based ingredient maps.
*   **Bundle Level (`Meal`):** Curated sets of food items ready for consumer purchase.

### 3. Secure Session Management
*   **Role-Based Access Control (RBAC):** Distinct interfaces for `Admin` (Inventory/Budget control) and `User` (Ordering/History).
*   **Transaction Persistence:** Automatic logging of order history with localized timestamps and file-based export (`fstream`).

### 4. Technical Reliability
*   **Exception Safety:** Comprehensive `try-throw-catch` blocks prevent negative stock, invalid pricing, or null pointer dereferencing.
*   **Standard Compliance:** Utilizes modern C++11/14/17 features including `<chrono>` for time-accurate logging and `<map>` for efficient recipe lookups.

## 🛠️ Tech Stack
- **Language:** C++11 or higher
- **Paradigm:** Object-Oriented Programming (Encapsulation, Inheritance, Polymorphism)
- **Utilities:** STL Containers, Chrono Library, File I/O

## 🚀 Future Roadmap
- [ ] **SQL Integration:** Transition from file-based storage to a relational database.
- [ ] **Threaded Notifications:** Real-time low-stock alerts using multi-threading.
- [ ] **API Layer:** RESTful wrapper for potential mobile/web integration.
